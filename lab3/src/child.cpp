#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <cctype>
#include <algorithm>
#include <cmath>

using namespace std;

bool is_prime(int number) {
    if (number <= 1) return false;
    if (number <= 3) return true;
    if (number % 2 == 0 || number % 3 == 0) return false;
    for (int i = 5; i * i <= number; i += 6) {
        if (number % i == 0 || number % (i + 2) == 0) return false;
    }
    return true;
}

// Функция для проверки, является ли строка числом
bool is_number(const string& s) {
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    const char* filename = argv[1];
    ofstream file(filename, ios::app);

    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return 1;
    }

    HANDLE hMapFile;
    LPCSTR pBuf;

    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,   // доступ на чтение/запись
        FALSE,                 // наследование дескриптора
        L"Global\\MyFileMappingObject"); // имя отображаемого файла

    if (hMapFile == NULL) {
        cerr << "Could not open file mapping object (" << GetLastError() << ")." << endl;
        return 1;
    }

    pBuf = (LPCSTR)MapViewOfFile(hMapFile, // дескриптор отображаемого файла
        FILE_MAP_ALL_ACCESS,  // доступ на чтение/запись
        0,
        0,
        256);

    if (pBuf == NULL) {
        cerr << "Could not map view of file (" << GetLastError() << ")." << endl;
        CloseHandle(hMapFile);
        return 1;
    }

    // Чтение числа из отображаемого файла
    string input(pBuf);
    if (!is_number(input)) {
        cerr << "Invalid input: '" << input << "' is not a number." << endl;
        UnmapViewOfFile(pBuf);
        CloseHandle(hMapFile);
        file.close();
        return 1;
    }

    int number;
    try {
        number = stoi(input);
    }
    catch (const invalid_argument& e) {
        cerr << "Invalid input: " << e.what() << endl;
        UnmapViewOfFile(pBuf);
        CloseHandle(hMapFile);
        file.close();
        return 1;
    }
    catch (const out_of_range& e) {
        cerr << "Number out of range: " << e.what() << endl;
        UnmapViewOfFile(pBuf);
        CloseHandle(hMapFile);
        file.close();
        return 1;
    }

    if (number < 0 || is_prime(number)) {
        UnmapViewOfFile(pBuf);
        CloseHandle(hMapFile);
        file.close();
        return 0;
    }
    else {
        file << number << endl;
    }

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    file.close();

    return 1;
}
