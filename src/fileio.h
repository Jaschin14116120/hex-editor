#ifndef FILEIO_H
#define FILEIO_H

#include <string>
#include <vector>

/*
    Laedt eine Datei im Binaermodus in einen Bytepuffer.
    Rueckgabe:
      - true  => Datei erfolgreich geladen
      - false => Fehler beim Oeffnen oder Lesen
*/
bool loadFileBinary(const std::string& filename, std::vector<unsigned char>& outBuffer);

/*
    Speichert einen Bytepuffer im Binaermodus in eine Datei.
    Rueckgabe:
      - true  => Datei erfolgreich gespeichert
      - false => Fehler beim Oeffnen oder Schreiben
*/
bool saveFileBinary(const std::string& filename, const std::vector<unsigned char>& buffer);

#endif
