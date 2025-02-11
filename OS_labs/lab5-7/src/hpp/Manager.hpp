#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "Tree.hpp"

// Функции управляющего узла
void killNode(Node* root, int id);
void pingAll(Node* root, vector<int>& unavailableNodes);
void pingAllCommand(Node* root);

#endif // MANAGER_HPP