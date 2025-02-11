#include "Manager.hpp"

using namespace std;

void killNode(Node* root, int id) {
	Node* node = findNodeById(root, id);
	if (!node) {
		wcout << L"Error: Node with ID " << id << L" not found." << endl;
		return;
	}

	// Завершаем процесс вычислительного узла
	HANDLE process = OpenProcess(PROCESS_TERMINATE, FALSE, node->pid);
	if (process) {
		if (TerminateProcess(process, 0)) {
			wcout << L"Ok: Node with ID " << id << L" terminated." << endl;
		}
		else {
			wcerr << L"Error: Failed to terminate node with ID " << id << endl;
		}
		CloseHandle(process);
	}
	else {
		wcerr << L"Error: Node with ID " << id << L" not found." << endl;
	}

	// Помечаем узел и его дочерние узлы как недоступные
	markNodeAndChildrenAsUnavailable(node);
}

void pingAll(Node* currentNode, vector<int>& unavailableNodes) {
	if (!currentNode) {
		return;
	}

	// Проверяем, доступен ли узел
	if (!currentNode->isAvailable) {
		unavailableNodes.push_back(currentNode->id);
	}
	else {
		// Проверяем, жив ли процесс
		HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, currentNode->pid);
		if (process) {
			CloseHandle(process);
		}
		else {
			unavailableNodes.push_back(currentNode->id);
		}
	}

	// Рекурсивно проверяем левое и правое поддерево
	pingAll(currentNode->left, unavailableNodes);
	pingAll(currentNode->right, unavailableNodes);
}

void pingAllCommand(Node* root) {
	vector<int> unavailableNodes;
	pingAll(root, unavailableNodes);

	if (unavailableNodes.empty()) {
		wcout << L"Ok: -1" << endl;
	}
	else {
		wcout << L"Ok: ";
		for (size_t i = 0; i < unavailableNodes.size(); ++i) {
			wcout << unavailableNodes[i];
			if (i != unavailableNodes.size() - 1) {
				wcout << L";";
			}
		}
		wcout << endl;
	}
}