#ifndef HUFFMAN_LIBRARY_H
#define HUFFMAN_LIBRARY_H

#include <fstream>
#include <bitset>
#include <vector>
#include <algorithm>
#include <cassert>

static constexpr size_t ALPSIZE = 256;
static constexpr size_t BUFLEN = 1024 * 1024;

struct Node {
    Node();
    Node(uint64_t weight, Node &left_son, Node &right_son);
    Node(unsigned char value, uint64_t weight);
    ~Node();
    unsigned char value;
    uint64_t weight;
    Node* left_son;
    Node* right_son;
    Node* dad;
    explicit Node(Node *dad);
};

bool compare(Node *first, Node *second);

struct OpenFileException{
    enum type_e {Infile, Outfile} type;
    OpenFileException();
    explicit OpenFileException(type_e type);

};

struct DamagedFileException{
    DamagedFileException();
};
#endif