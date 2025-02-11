#include "Tree.hpp"
#include "Manager.hpp"

using namespace std;

int main() {
	Node* root = nullptr;

	wcout << L"First parent id = 1" << endl;
	createNode(root, 1, -1);

	wstring command;

	while (true) {
		wcout << L"> ";
		getline(wcin, command);

		if (command.substr(0, 6) == L"create") {
			int id, parentId;
			wistringstream ss(command.substr(7));
			ss >> id >> parentId;
			createNode(root, id, parentId);
		}
		else if (command.substr(0, 4) == L"kill") {
			int id;
			wistringstream ss(command.substr(5));
			ss >> id;
			killNode(root, id);
		}
		else if (command == L"pingall") {
			pingAllCommand(root);
		}
		else if (command == L"print") {
			printTree(root);
		}
		else {
			wcout << L"Error: Unknown command" << endl;
		}
	}

	return 0;
}