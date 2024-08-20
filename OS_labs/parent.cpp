#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

wstring stringToWString(const string& str) {
    int len;
    int slength = (int)str.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, 0, 0);
    wstring r(len, L'\0');
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, &r[0], len);
    return r;
}

int main() {
    HANDLE hPipe1Read, hPipe1Write, hPipe2Read, hPipe2Write;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    // Создание pipe для связи с дочерним процессом
    if (!CreatePipe(&hPipe1Read, &hPipe1Write, &sa, 0)) {
        cerr << "Failed to create pipe1" << endl;
        return 1;
    }
    if (!CreatePipe(&hPipe2Read, &hPipe2Write, &sa, 0)) {
        cerr << "Failed to create pipe2" << endl;
        return 1;
    }

    // Получение имени файла от пользователя
    string fileName;
    cout << "Enter the output file name: ";
    getline(cin, fileName);

    // Создание строки запуска дочернего процесса
    std::string commandLine = "C:\\Users\\ivanb\\source\\repos\\OS_lab1_child\\x64\\Debug\\OS_lab1_child.exe " + fileName;
    std::wstring wCommandLine = stringToWString(commandLine);

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;

    si.hStdInput = hPipe1Read;
    si.hStdOutput = hPipe2Write;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.dwFlags |= STARTF_USESTDHANDLES;

    // Создание дочернего процесса
    if (!CreateProcess(NULL, &wCommandLine[0], NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        cerr << "Failed to create process" << endl;
        return 1;
    }

    // Закрытие ненужных дескрипторов
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hPipe1Read);
    CloseHandle(hPipe2Write);

    // Ввод и отправка данных
    string input;
    while (true) {
        cout << "Enter a number: ";
        getline(std::cin, input);

        if (input.empty()) break;

        DWORD written;
        if (!WriteFile(hPipe1Write, input.c_str(), input.size(), &written, NULL)) {
            cerr << "Failed to write to pipe" << endl;
            break;
        }

        // Если число отрицательное или простое, завершаем процесс
        int num = std::stoi(input);
        if (num < 0) break;
    }

    // Закрытие дескрипторов
    CloseHandle(hPipe1Write);
    CloseHandle(hPipe2Read);

    return 0;
}