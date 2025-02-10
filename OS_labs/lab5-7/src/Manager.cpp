#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>

using namespace std;

struct Node {
	int id;
	unsigned long pid;
	Node* left, * right, * parent;
};

// Корень дерева
Node* root = nullptr;

// Рекурсивный обход для поиска узла в дереве
Node* findNodeById(Node* currentNode, int id) {
    if (!currentNode) {
        return nullptr; // Узел не найден
    }

    if (currentNode->id == id) {
        return currentNode; // Узел найден
    }

    // Ищем в левом поддереве
    Node* leftResult = findNodeById(currentNode->left, id);
    if (leftResult) {
        return leftResult;
    }

    // Ищем в правом поддереве
    return findNodeById(currentNode->right, id);
}

Node* createNode(int id, int parentId) {
	// Проверяем существование узла с таким id
	if (findNodeById(root, id)) {
		wcout << L"Error: Already exist" << endl;
		return nullptr;
	}

	// Находим родительский узел
	Node* parent = nullptr;
	if (parentId != -1) {
		parent = findNodeById(root, parentId);
		if (!parent) {
			wcout << L"Error: Parent not found" << endl;
			return nullptr;
		}
		// Проверка на свободное место у родителя
		if (parent->left && parent->right) {
			wcout << L"Error: Parent is full" << endl;
			return nullptr;
		}
	}

	// Создаем новый процесс (вычислительный узел) 
	// взято здесь https://learn.microsoft.com/en-us/windows/win32/procthread/creating-processes
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Создаем командную строку в формате Unicode
	wstring commandLine = L"C:\\Users\\ivanb\\source\\repos\\ComputingNode\\x64\\Debug\\ComputingNode.exe " + to_wstring(id) + L" " + to_wstring(parentId);

	if (CreateProcess(
		NULL,            // Имя исполняемого файла
		&commandLine[0], // Командная строка
		NULL,            // Атрибуты безопасности процесса
		NULL,            // Атрибуты безопасности потока
		FALSE,           // Наследование handles
		0,               // Флаги создания
		NULL,            // Окружение
		NULL,            // Текущий каталог
		&si,             // STARTUPINFO
		&pi              // PROCESS_INFORMATION
	)) {
		Node* newNode = new Node{ id, pi.dwProcessId, nullptr, nullptr, parent };
		if (parent) {
			if (!parent->left) {
				parent->left = newNode;
			}
			else {
				parent->right = newNode;
			}
		}
		else {
			root = newNode;
		}
		wcout << L"Ok: " << pi.dwProcessId << endl;
		return newNode;
	}
	else {
		wcerr << L"Error: Failed to create node" << endl;
		return nullptr;
	}
}

int main() {

	wcout << L"Create first root with id = 1" << endl;
	createNode(1, -1);

	wstring command;

	while (true) {
		wcout << L"> ";
		getline(wcin, command);

		if (command.substr(0, 6) == L"create") {
			int id, parentId;
			wistringstream ss(command.substr(7));
			ss >> id >> parentId;
			createNode(id, parentId);
		}
		else {
			wcout << L"Error: Unknown command" << endl;
		}
	}

	return 0;
}