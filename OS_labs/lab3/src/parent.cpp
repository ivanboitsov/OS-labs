#include <iostream>
#include <windows.h>
#include <string>

using namespace std;

// Функция для преобразования строки в широкую строку
wstring stringToWString(const string& str) {
    int len;
    int slength = (int)str.length() + 1;
    len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), slength, 0, 0);
    wstring r(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), slength, &r[0], len);
    return r;
}

int main() {
    setlocale(LC_ALL, "Russian");

    HANDLE hMapFile;
    LPCTSTR pBuf;

    // Создаем отображаемый файл
    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    // используем файл подкачки
        NULL,                    // стандартные атрибуты защиты
        PAGE_READWRITE,          // доступ на чтение/запись
        0,                       // размер файла в старших 32 битах
        256,                     // размер файла в младших 32 битах
        TEXT("Global\\MyFileMappingObject")); // имя отображаемого файла

    if (hMapFile == NULL) {
        cerr << "Could not create file mapping object (" << GetLastError() << ")." << endl;
        system("pause");
        return 1;
    }

    pBuf = (LPTSTR)MapViewOfFile(hMapFile,   // дескриптор отображаемого файла
        FILE_MAP_ALL_ACCESS, // доступ на чтение/запись
        0,
        0,
        256);

    if (pBuf == NULL) {
        cerr << "Could not map view of file (" << GetLastError() << ")." << endl;
        CloseHandle(hMapFile);
        system("pause");
        return 1;
    }

    // Создание дочернего процесса
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    string filename;
    cout << "Введите имя файла для записи результатов: ";
    getline(cin, filename);

    string commandLine = "C:\\Users\\ivanb\\source\\repos\\OS_lab3_child\\x64\\Debug\\OS_lab3_child.exe " + filename;
    wstring wCommandLine = stringToWString(commandLine);

    // Преобразование строки в формат, приемлемый для CreateProcess
    if (!CreateProcess(NULL, &wCommandLine[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        cerr << "CreateProcess failed (" << GetLastError() << ")." << endl;
        UnmapViewOfFile(pBuf);
        CloseHandle(hMapFile);
        system("pause");
        return 1;
    }

    string input;
    while (true) {
        cout << "Введите число: ";
        getline(cin, input);

        // Копируем введенное число в отображаемую область памяти
        CopyMemory((PVOID)pBuf, input.c_str(), (input.size() + 1) * sizeof(TCHAR));

        // Ждем завершения дочернего процесса
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD exitCode;
        if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
            if (exitCode == 0) break;
        }
        else {
            cerr << "Failed to get exit code (" << GetLastError() << ")." << endl;
        }
    }

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    system("pause");
    return 0;
}
