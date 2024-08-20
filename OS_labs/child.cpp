#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

bool isComposite(int num) {
    if (num <= 1) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return true;
    }
    return false;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "No file name provided!" << std::endl;
        return 1;
    }

    std::ofstream outFile(argv[1], std::ios::app);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    HANDLE hPipe1Read = GetStdHandle(STD_INPUT_HANDLE);
    char buffer[128];
    DWORD bytesRead;

    while (true) {
        if (!ReadFile(hPipe1Read, buffer, sizeof(buffer) - 1, &bytesRead, NULL) || bytesRead == 0) {
            break;
        }
        buffer[bytesRead] = '\0';

        int number = std::stoi(buffer);
        if (number < 0 || !isComposite(number)) {
            break;
        }

        outFile << number << std::endl;
    }

    outFile.close();
    return 0;
}