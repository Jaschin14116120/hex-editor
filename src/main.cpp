#include <iostream>
#include <vector>
#include <iomanip>
#include <bitset>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <limits>
#include <cstdint>

#include "fileio.h"

using namespace std;

/*
    Hilfsfunktion: Prüft, ob ein Zeichen druckbar ist.
    Nicht druckbare Zeichen werden später als '.' dargestellt.
*/
static bool isPrintable(unsigned char c)
{
    return std::isprint(static_cast<int>(c)) != 0;
}

/*
    Ausgabe-Funktion: Gibt einen Ausschnitt des Bytepuffers tabellarisch aus.

    Parameter:
      data         - Bytepuffer
      startOffset  - Startadresse (Offset) in Bytes
      bytesPerLine - wie viele Bytes pro Zeile (z.B. 16)
      lines        - wie viele Zeilen (z.B. 16 für eine Seite)

    Darstellung pro Zeile:
      Offset | HEX (bytesPerLine Stück) | BIN (8 Bit pro Byte) | ASCII
*/
static void printTable(const vector<unsigned char>& data,
                       size_t startOffset,
                       size_t bytesPerLine,
                       size_t lines)
{
    // Headerzeile (optional, aber für Lesbarkeit hilfreich)
    // Hinweis: BIN ist platzintensiv. Für eine saubere Terminaldarstellung bleiben wir bei 16 Bytes pro Zeile,
    //          nehmen aber eine breite Ausgabe in Kauf (ggf. Terminalfenster verbreitern).
    cout << "Offset     HEX";
    // Abstand für HEX-Spalte (jede Byte-Ausgabe: "FF " => 3 Zeichen)
    cout << string(static_cast<size_t>(bytesPerLine * 3), ' ');
    cout << " BIN";
    // Abstand für BIN-Spalte (jede Byte-Ausgabe: "01010101 " => 9 Zeichen)
    cout << string(static_cast<size_t>(bytesPerLine * 9), ' ');
    cout << " ASCII" << "\n";

    // StartOffset auf eine Zeilengrenze runden (z.B. bei goto)
    size_t alignedStart = (startOffset / bytesPerLine) * bytesPerLine;

    for (size_t line = 0; line < lines; ++line)
    {
        size_t i = alignedStart + line * bytesPerLine;

        if (i >= data.size())
            break;

        // Offset in Hex (8 Stellen)
        cout << setw(8) << setfill('0') << hex << uppercase << i << "  ";
        cout << nouppercase;

        // HEX-Spalte
        for (size_t j = 0; j < bytesPerLine; ++j)
        {
            if (i + j < data.size())
            {
                cout << setw(2) << setfill('0') << hex << (int)data[i + j] << " ";
            }
            else
            {
                cout << "   ";
            }
        }

        cout << " ";

        // BIN-Spalte (8 Bit pro Byte)
        for (size_t j = 0; j < bytesPerLine; ++j)
        {
            if (i + j < data.size())
            {
                bitset<8> b(data[i + j]);
                cout << b << " ";
            }
            else
            {
                // Platzhalter (8 Bits + Leerzeichen)
                cout << "         ";
            }
        }

        cout << " ";

        // ASCII-Spalte
        for (size_t j = 0; j < bytesPerLine; ++j)
        {
            if (i + j < data.size())
            {
                unsigned char c = data[i + j];
                cout << (isPrintable(c) ? static_cast<char>(c) : '.');
            }
        }

        cout << "\n";
    }

    // Wichtig: Stream wieder in "decimal" zurücksetzen, sonst wirkt spätere Ausgabe komisch
    cout << dec << setfill(' ') << "\n";
}

/*
    Hilfsfunktion: Entfernt führende und nachfolgende Leerzeichen.
*/
static string trim(const string& s)
{
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start])))
        ++start;

    size_t end = s.size();
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1])))
        --end;

    return s.substr(start, end - start);
}

/*
    Hilfsfunktion: Parsen eines Offsets (Sprungadresse).
    Unterstützt:
      - Dezimal: "1234"
      - Hex:     "0x4D2" oder "4D2" (wenn Buchstaben A-F enthalten sind)

    Rückgabe:
      - true/false ob erfolgreich geparst
*/
static bool parseOffset(const string& token, size_t& outOffset)
{
    string t = trim(token);
    if (t.empty())
        return false;

    // Prüfen, ob 0x/0X Prefix vorhanden ist
    bool has0x = (t.size() > 2 && t[0] == '0' && (t[1] == 'x' || t[1] == 'X'));

    // Prüfen, ob hextypische Buchstaben enthalten sind (A-F/a-f)
    bool hasHexLetters = false;
    for (char c : t)
    {
        if ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
        {
            hasHexLetters = true;
            break;
        }
    }

    int base = 10;
    string numberPart = t;

    if (has0x)
    {
        base = 16;
        numberPart = t.substr(2);
    }
    else if (hasHexLetters)
    {
        base = 16;
    }

    // Jetzt versuchen wir zu konvertieren
    try
    {
        // stoull kann mit Basis arbeiten
        unsigned long long val = stoull(numberPart, nullptr, base);
        outOffset = static_cast<size_t>(val);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

/*
    Hilfsfunktion: Parst einen Byte-Wert (0..255) aus verschiedenen Eingabeformen.

    Unterstützt:
      - Dezimal: "65"
      - Hex:     "0x41" oder "41" (wenn Buchstaben A-F enthalten sind)
      - Binär:   "0b01000001"
      - Zeichen: "'A'"  (genau ein Zeichen in Hochkommas)

    Rückgabe:
      - true/false ob erfolgreich
*/
static bool parseByteValue(const string& token, unsigned char& outValue)
{
    string t = trim(token);
    if (t.empty())
        return false;

    // 1) Zeichenliteral: 'A'
    if (t.size() == 3 && t.front() == '\'' && t.back() == '\'')
    {
        outValue = static_cast<unsigned char>(t[1]);
        return true;
    }

    // 2) Binär: 0b10101010
    if (t.size() > 2 && t[0] == '0' && (t[1] == 'b' || t[1] == 'B'))
    {
        string bits = t.substr(2);

        // Erlaubt sind nur '0' und '1', und maximal 8 Bits
        if (bits.empty() || bits.size() > 8)
            return false;

        for (char c : bits)
            if (c != '0' && c != '1')
                return false;

        unsigned int val = 0;
        for (char c : bits)
        {
            val = (val << 1) | (c == '1' ? 1u : 0u);
        }

        outValue = static_cast<unsigned char>(val);
        return true;
    }

    // 3) Hex mit 0x
    bool has0x = (t.size() > 2 && t[0] == '0' && (t[1] == 'x' || (t[1] == 'X')));

    // 4) Hex ohne 0x, wenn A-F vorkommt
    bool hasHexLetters = false;
    for (char c : t)
    {
        if ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
        {
            hasHexLetters = true;
            break;
        }
    }

    int base = 10;
    string numberPart = t;

    if (has0x)
    {
        base = 16;
        numberPart = t.substr(2);
    }
    else if (hasHexLetters)
    {
        base = 16;
    }

    try
    {
        unsigned long long val = stoull(numberPart, nullptr, base);
        if (val > 255ULL)
            return false;

        outValue = static_cast<unsigned char>(val);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

/*
    Hilfetext: Zeigt die verfügbaren Befehle an.
*/
static void printHelp()
{
    cout << "Befehle:\n"
         << "  view                   - aktuelle Seite anzeigen\n"
         << "  next                   - naechste Seite\n"
         << "  prev                   - vorherige Seite\n"
         << "  goto <offset>           - zu Offset springen (dezimal oder hex, z.B. 0x10)\n"
         << "  set <offset> <wert>     - Byte setzen (dez/hex/bin/'A')\n"
         << "  save                   - Datei speichern (ueberschreibt die Ausgangsdatei)\n"
         << "  saveas <dateiname>      - Datei unter neuem Namen speichern\n"
         << "  status                 - Status anzeigen (Datei/Offset/ungespeichert)\n"
         << "  help                   - diese Hilfe anzeigen\n"
         << "  quit                   - Programm beenden\n\n";
}

int main(int argc, char* argv[])
{
    // -----------------------------
    // 1) Dateiname ermitteln
    // -----------------------------
    string filename;

    if (argc >= 2)
    {
        filename = argv[1];
    }
    else
    {
        cout << "Dateipfad eingeben: ";
        cin >> filename;
        // Rest der Zeile leeren, damit getline später sauber funktioniert
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // -----------------------------
    // 2) Datei laden
    // -----------------------------
    // Das eigentliche Laden wird nun nicht mehr direkt in main.cpp implementiert,
    // sondern ueber das ausgelagerte Modul fileio.cpp abgewickelt.
    // main.cpp kennt dadurch nur noch die Funktionsschnittstelle aus fileio.h.
    vector<unsigned char> buffer;
    if (!loadFileBinary(filename, buffer))
    {
        cout << "Fehler: Datei konnte nicht geoeffnet werden.\n";
        return 1;
    }

    cout << "Datei geladen: " << filename << "\n";
    cout << "Groesse: " << buffer.size() << " Bytes\n\n";

    // -----------------------------
    // 3) Anzeige-/Navigationsparameter
    // -----------------------------
    const size_t bytesPerLine = 16;    // 16 Bytes je Zeile (klassisch)
    const size_t linesPerPage = 16;    // 16 Zeilen je Seite => 256 Bytes je Seite
    const size_t pageSize = bytesPerLine * linesPerPage;

    size_t currentOffset = 0;          // Start bei 0
    bool dirty = false;                // true, wenn Buffer geaendert wurde, aber noch nicht gespeichert ist

    // -----------------------------
    // 4) Kommando-Schleife (CLI)
    // -----------------------------
    printHelp();
    printTable(buffer, currentOffset, bytesPerLine, linesPerPage);

    while (true)
    {
        cout << "> ";
        string line;
        if (!getline(cin, line))
            break;

        line = trim(line);
        if (line.empty())
            continue;

        // Kommando + Argumente trennen
        istringstream iss(line);
        string cmd;
        iss >> cmd;

        // Kommandos in Kleinbuchstaben vereinheitlichen
        for (char& c : cmd)
            c = static_cast<char>(tolower(static_cast<unsigned char>(c)));

        if (cmd == "quit" || cmd == "exit")
        {
            // Hinweis, falls ungespeicherte Aenderungen vorhanden sind
            if (dirty)
                cout << "Hinweis: Es gibt ungespeicherte Aenderungen.\n";

            break;
        }
        else if (cmd == "help")
        {
            printHelp();
        }
        else if (cmd == "status")
        {
            cout << "Datei:   " << filename << "\n";
            cout << "Groesse: " << buffer.size() << " Bytes\n";
            cout << "Offset:  " << currentOffset << "\n";
            cout << "Aenderungen ungespeichert: " << (dirty ? "JA" : "NEIN") << "\n\n";
        }
        else if (cmd == "view")
        {
            printTable(buffer, currentOffset, bytesPerLine, linesPerPage);
        }
        else if (cmd == "next")
        {
            // Naechste Seite: Offset + pageSize, aber nicht ueber das Dateiende hinaus
            if (currentOffset + pageSize < buffer.size())
                currentOffset += pageSize;
            else
                cout << "Hinweis: Ende der Datei erreicht.\n";

            printTable(buffer, currentOffset, bytesPerLine, linesPerPage);
        }
        else if (cmd == "prev")
        {
            // Vorherige Seite: Offset - pageSize, aber nicht unter 0
            if (currentOffset >= pageSize)
                currentOffset -= pageSize;
            else
                currentOffset = 0;

            printTable(buffer, currentOffset, bytesPerLine, linesPerPage);
        }
        else if (cmd == "goto")
        {
            string token;
            iss >> token;

            if (token.empty())
            {
                cout << "Fehler: Bitte einen Offset angeben. Beispiel: goto 0x40\n";
                continue;
            }

            size_t target = 0;
            if (!parseOffset(token, target))
            {
                cout << "Fehler: Offset konnte nicht gelesen werden.\n";
                continue;
            }

            if (target >= buffer.size())
            {
                cout << "Fehler: Offset liegt ausserhalb der Datei.\n";
                continue;
            }

            currentOffset = target;
            printTable(buffer, currentOffset, bytesPerLine, linesPerPage);
        }
        else if (cmd == "set")
        {
            // Erwartet: set <offset> <wert>
            // Beispiele:
            //   set 16 FF
            //   set 16 255
            //   set 16 0x41
            //   set 16 0b01000001
            //   set 16 'A'
            string offToken;
            string valToken;
            iss >> offToken >> valToken;

            if (offToken.empty() || valToken.empty())
            {
                cout << "Fehler: Nutzung: set <offset> <wert>\n";
                cout << "Beispiele: set 16 FF | set 16 255 | set 16 0b10101010 | set 16 'A'\n";
                continue;
            }

            size_t off = 0;
            if (!parseOffset(offToken, off))
            {
                cout << "Fehler: Offset konnte nicht gelesen werden.\n";
                continue;
            }

            if (off >= buffer.size())
            {
                cout << "Fehler: Offset liegt ausserhalb der Datei.\n";
                continue;
            }

            unsigned char newVal = 0;
            if (!parseByteValue(valToken, newVal))
            {
                cout << "Fehler: Wert konnte nicht gelesen werden (erwartet: dez/hex/bin/'A').\n";
                continue;
            }

            // Aenderung durchfuehren
            unsigned char oldVal = buffer[off];
            buffer[off] = newVal;
            dirty = true;

            // Rueckmeldung an den Benutzer (nachvollziehbar / testbar)
            cout << "Byte geaendert bei Offset " << off << ": "
                 << "0x" << hex << setw(2) << setfill('0') << (int)oldVal
                 << " -> 0x" << setw(2) << (int)newVal
                 << dec << setfill(' ') << "\n";

            // Seite neu anzeigen, damit man die Aenderung sofort sieht
            printTable(buffer, currentOffset, bytesPerLine, linesPerPage);
        }
        else if (cmd == "save")
        {
            if (!dirty)
            {
                cout << "Hinweis: Keine Aenderungen vorhanden. (dirty = false)\n";
                continue;
            }

            // Speichern erfolgt nun ebenfalls ueber das ausgelagerte File-I/O-Modul.
            if (!saveFileBinary(filename, buffer))
            {
                cout << "Fehler: Datei konnte nicht gespeichert werden (keine Rechte / Pfad ungueltig?).\n";
                continue;
            }

            dirty = false;
            cout << "Datei gespeichert: " << filename << "\n\n";
        }
        else if (cmd == "saveas")
        {
            string newName;
            iss >> newName;

            if (newName.empty())
            {
                cout << "Fehler: Bitte einen Dateinamen angeben. Beispiel: saveas neue_datei.bin\n";
                continue;
            }

            if (!saveFileBinary(newName, buffer))
            {
                cout << "Fehler: Datei konnte nicht gespeichert werden (keine Rechte / Pfad ungueltig?).\n";
                continue;
            }

            dirty = false;
            cout << "Datei gespeichert als: " << newName << "\n\n";
        }
        else
        {
            cout << "Unbekannter Befehl. 'help' zeigt die verfuegbaren Befehle.\n";
        }
    }

    cout << "Programm beendet.\n";
    return 0;
}
