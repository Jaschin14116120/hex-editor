#include "fileio.h"

#include <fstream>

/*
    Implementierung der Funktion zum Laden einer Datei im Binaermodus.
*/
bool loadFileBinary(const std::string& filename, std::vector<unsigned char>& outBuffer)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file)
        return false;

    outBuffer.assign(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    return true;
}

/*
    Implementierung der Funktion zum Speichern eines Bytepuffers in eine Datei.
*/
bool saveFileBinary(const std::string& filename, const std::vector<unsigned char>& buffer)
{
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);

    if (!file)
        return false;

    if (!buffer.empty())
    {
        file.write(reinterpret_cast<const char*>(buffer.data()),
                   static_cast<std::streamsize>(buffer.size()));
    }

    if (!file.good())
        return false;

    return true;
}
