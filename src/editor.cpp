#include "editor.h"

/*
    Implementierung der Editor-Funktion zum Ändern eines Bytes.

    Diese Funktion kapselt die eigentliche Bearbeitungslogik des Editors.
    Dadurch bleibt die Benutzeroberfläche (main.cpp) von der internen
    Datenmanipulation getrennt.
*/
bool setByteValue(std::vector<unsigned char>& buffer,
                  std::size_t offset,
                  unsigned char newValue,
                  unsigned char& oldValue)
{
    // Prüfen, ob der Offset innerhalb des Puffers liegt
    if (offset >= buffer.size())
        return false;

    // Alten Wert speichern
    oldValue = buffer[offset];

    // Byte im Puffer überschreiben
    buffer[offset] = newValue;

    return true;
}
