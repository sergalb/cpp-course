#include "library.h"

#include <iostream>
#include <algorithm>
#include <cassert>
#include <math.h>

Node::Node() {
    left_son = nullptr;
    right_son = nullptr;
    dad = nullptr;
}

Node::Node(unsigned char value, uint64_t weight) : value(value), weight(weight) {
    left_son = nullptr;
    right_son = nullptr;
}

Node::Node(uint64_t weight, Node &left_son, Node &right_son) : weight(weight), left_son(&left_son),
                                                               right_son(&right_son) {}

Node::~Node() {
    if (left_son != nullptr) left_son->~Node();
    if (right_son != nullptr) right_son->~Node();
}

Node::Node(Node *dad) : dad(dad) {
    left_son = nullptr;
    right_son = nullptr;
}

bool compare(Node *first, Node *second) { return first->weight < second->weight; }

OpenFileException::OpenFileException() {}

OpenFileException::OpenFileException(OpenFileException::type_e type) : type(type){};

DamagedFileException::DamagedFileException() {}
