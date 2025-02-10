#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;

int main(int argc, char* argv[]) {
    
    if (argc < 3) {
        wcerr << L"Usage: ComputingNode.exe <id> <parentId>" << endl;
        return 1;
    }

    int id = stoi(argv[1]);
    int parentId = stoi(argv[2]);

    while (true) {
        Sleep(1000);
    }

    return 0;
}