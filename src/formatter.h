#ifndef FORMATTER_H
#define FORMATTER_H

#include <vector>
#include <cstddef>

/*
    Prüft, ob ein Zeichen druckbar ist.
    Nicht druckbare Zeichen werden später als '.' dargestellt.
*/
bool isPrintable(unsigned char c);

/*
    Gibt einen Ausschnitt des Bytepuffers tabellarisch aus.

    Parameter:
      data         - Bytepuffer
      startOffset  - Startadresse (Offset) in Bytes
      bytesPerLine - wie viele Bytes pro Zeile (z.B. 16)
      lines        - wie viele Zeilen (z.B. 16 für eine Seite)

    Darstellung pro Zeile:
      Offset | HEX (bytesPerLine Stück) | BIN (8 Bit pro Byte) | ASCII
*/
void printTable(const std::vector<unsigned char>& data,
                std::size_t startOffset,
                std::size_t bytesPerLine,
                std::size_t lines);

#endif
