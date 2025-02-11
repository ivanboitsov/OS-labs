#ifndef TREE_HPP
#define TREE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <Windows.h>

using namespace std;

struct Node {
    int id;
    unsigned long pid;
    bool isAvailable;
    Node* left, * right, * parent;
};

// Функции для работы с деревом
int getTreeHeight(Node* currentNode);
bool isTreeBalanced(Node* currentNode);
Node* findNodeById(Node* currentNode, int id);
Node* createNode(Node*& root, int id, int parentId);
void markNodeAndChildrenAsUnavailable(Node* node);
void printTree(Node* currentNode, const wstring& prefix = L"", bool isLeft = true);

#endif // TREE_HPP