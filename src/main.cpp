#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <bitset>

using namespace std;

void printBytes(const vector<unsigned char>& data, size_t start, size_t count)
{
    size_t end = min(start + count, data.size());

    for (size_t i = start; i < end; i += 16)
    {
        cout << setw(8) << setfill('0') << hex << i << "  ";

        // HEX Darstellung
        for (size_t j = 0; j < 16; j++)
        {
            if (i + j < data.size())
                cout << setw(2) << (int)data[i + j] << " ";
            else
                cout << "   ";
        }

        cout << " ";

        // ASCII Darstellung
        for (size_t j = 0; j < 16; j++)
        {
            if (i + j < data.size())
            {
                unsigned char c = data[i + j];
                if (isprint(c))
                    cout << c;
                else
                    cout << ".";
            }
        }

        cout << endl;
    }
}

int main(int argc, char* argv[])
{
    string filename;

    if (argc >= 2)
    {
        filename = argv[1];
    }
    else
    {
        cout << "Dateipfad eingeben: ";
        cin >> filename;
    }

    ifstream file(filename, ios::binary);

    if (!file)
    {
        cout << "Fehler: Datei konnte nicht geöffnet werden." << endl;
        return 1;
    }

    vector<unsigned char> buffer(
        (istreambuf_iterator<char>(file)),
        istreambuf_iterator<char>()
    );

    cout << "Datei geladen. Groesse: " << buffer.size() << " Bytes\n\n";

    printBytes(buffer, 0, buffer.size());

    return 0;
}
