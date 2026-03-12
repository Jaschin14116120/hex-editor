#ifndef EDITOR_H
#define EDITOR_H

#include <vector>
#include <cstddef>

/*
    Setzt ein Byte im Puffer an einer bestimmten Position.

    Parameter:
      buffer   - Referenz auf den Bytepuffer der geladenen Datei
      offset   - Position des zu ändernden Bytes
      newValue - neuer Bytewert
      oldValue - Rückgabewert: vorheriger Bytewert

    Rückgabe:
      - true  => Änderung erfolgreich
      - false => Offset liegt außerhalb des Puffers
*/
bool setByteValue(std::vector<unsigned char>& buffer,
                  std::size_t offset,
                  unsigned char newValue,
                  unsigned char& oldValue);

#endif
