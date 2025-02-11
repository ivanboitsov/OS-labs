#include "Tree.hpp"

// Нахождение высоты дерева
int getTreeHeight(Node* currentNode) {
	if (!currentNode) {
		return 0;
	}

	int leftHeight = getTreeHeight(currentNode->left);
	int rightHeight = getTreeHeight(currentNode->right);
	return 1 + max(leftHeight, rightHeight);
}

// Проверка на сбалансированность дерева
bool isTreeBalanced(Node* currentNode) {
	if (!currentNode) {
		return true;
	}

	int leftHeight = getTreeHeight(currentNode->left);
	int rightHeight = getTreeHeight(currentNode->right);

	if (abs(leftHeight - rightHeight) > 1) {
		return false;
	}

	return isTreeBalanced(currentNode->left) && isTreeBalanced(currentNode->right);
}

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

// Создание узла
Node* createNode(Node*& root, int id, int parentId) {
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

	// Временно добавляем узел для проверки баланса
	Node* tempNode = new Node{ -1, ULONG_MAX, false, nullptr, nullptr, parent };
	if (parent) {
		if (!parent->left) {
			parent->left = tempNode;
		}
		else {
			parent->right = tempNode;
		}
	}
	else {
		root = tempNode;
	}

	// Проверяем баланс дерева
	bool isBalanced = isTreeBalanced(root);

	// Удаляем временный узел
	if (parent) {
		if (parent->left == tempNode) {
			parent->left = nullptr;
		}
		else {
			parent->right = nullptr;
		}
	}
	else {
		root = nullptr;
	}
	delete tempNode;

	if (!isBalanced) {
		wcout << L"Error: Tree would become unbalanced" << endl;
		return nullptr;
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
		Node* newNode = new Node{ id, pi.dwProcessId, true, nullptr, nullptr, parent };
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

void markNodeAndChildrenAsUnavailable(Node* node) {
	if (!node) {
		return;
	}

	// Помечаем узел как недоступный
	node->isAvailable = false;

	// Рекурсивно помечаем дочерние узлы
	markNodeAndChildrenAsUnavailable(node->left);
	markNodeAndChildrenAsUnavailable(node->right);
}

// Вывод дерева
void printTree(Node* currentNode, const wstring& prefix, bool isLeft) {
	if (!currentNode) {
		return;
	}

	// Выводим текущий узел
	wcout << prefix;
	wcout << (isLeft ? L"+-- " : L"\\-- ");
	if (currentNode->isAvailable) {
		wcout << L"Node " << currentNode->id << L" (PID: " << currentNode->pid << L") " << "[available]" << endl;
	}
	else {
		wcout << L"Node " << currentNode->id << L" (PID: " << currentNode->pid << L") " << "[unavailable]" << endl;
	}

	// Рекурсивно обходим левое и правое поддерево
	printTree(currentNode->left, prefix + (isLeft ? L"|   " : L"    "), true);
	printTree(currentNode->right, prefix + (isLeft ? L"|   " : L"    "), false);
}