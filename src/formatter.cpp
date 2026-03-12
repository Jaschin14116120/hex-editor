#include "formatter.h"

#include <iostream>
#include <iomanip>
#include <bitset>
#include <cctype>

using namespace std;

/*
    Hilfsfunktion: Prüft, ob ein Zeichen druckbar ist.
    Nicht druckbare Zeichen werden später als '.' dargestellt.
*/
bool isPrintable(unsigned char c)
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
void printTable(const vector<unsigned char>& data,
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
