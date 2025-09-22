#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <Windows.h>

using namespace std;

void execCommand(int id, const vector<int>& params) {
    int result = 0;
    for (int param : params) {
        result += param;
    }
    wcout << L"Ok:" << id << ":" << result << endl;
}

int main(int argc, char* argv[]) {
    
    if (argc < 3) {
        wcerr << L"Usage: ComputingNode.exe <id> <parentId>" << endl;
        return 1;
    }

    int id = stoi(argv[1]);
    int parentId = stoi(argv[2]);

    // Ожидание команд от управляющего узла
    while (true) {
        
        wstring command;
        getline(wcin, command);

        if (command.substr(0, 4) == L"exec") {
            wistringstream ss(command.substr(5));
            int n;
            ss >> n;
            vector<int> params(n);

            for (int i = 0; i < n; i++) {
                ss >> params[i];
            }

            execCommand(id, params);
        }
        else {
            wcout << L"Error: Unknown command" << endl;
        }
    }

    return 0;
}