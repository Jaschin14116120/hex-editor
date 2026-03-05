#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <bitset>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>

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
    Hilfsfunktion: Liest eine komplette Datei im Binärmodus in einen Bytepuffer.
    Rückgabe:
      - true  => Datei erfolgreich geladen
      - false => Fehler (z.B. Datei nicht vorhanden / keine Rechte)
*/
static bool loadFileBinary(const string& filename, vector<unsigned char>& outBuffer)
{
    ifstream file(filename, ios::binary);

    if (!file)
        return false;

    outBuffer.assign(
        (istreambuf_iterator<char>(file)),
        istreambuf_iterator<char>()
    );

    return true;
}

/*
    Ausgabe-Funktion: Gibt einen Ausschnitt des Bytepuffers tabellarisch aus.

    Parameter:
      data         - Bytepuffer
      startOffset  - Startadresse (Offset) in Bytes
      bytesPerLine - wie viele Bytes pro Zeile (z.B. 16)
      lines        - wie viele Zeilen (z.B. 16 für eine Seite)

    Darstellung pro Zeile:
      Offset | HEX (bytesPerLine Stück) | ASCII
*/
static void printTable(const vector<unsigned char>& data,
                       size_t startOffset,
                       size_t bytesPerLine,
                       size_t lines)
{
    // Headerzeile (optional, aber für Lesbarkeit hilfreich)
    cout << "Offset     HEX";
    // Abstand so grob ausrichten
    cout << string(static_cast<size_t>(bytesPerLine * 3), ' ');
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
    Hilfetext: Zeigt die verfügbaren Befehle an.
*/
static void printHelp()
{
    cout << "Befehle:\n"
         << "  view                 - aktuelle Seite anzeigen\n"
         << "  next                 - naechste Seite\n"
         << "  prev                 - vorherige Seite\n"
         << "  goto <offset>         - zu Offset springen (dezimal oder hex, z.B. 0x10)\n"
         << "  help                 - diese Hilfe anzeigen\n"
         << "  quit                 - Programm beenden\n\n";
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
            break;
        }
        else if (cmd == "help")
        {
            printHelp();
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
        else
        {
            cout << "Unbekannter Befehl. 'help' zeigt die verfuegbaren Befehle.\n";
        }
    }

    cout << "Programm beendet.\n";
    return 0;
}
