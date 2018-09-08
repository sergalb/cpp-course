#ifndef HUFFMAN_DECODER_H
#define HUFFMAN_DECODER_H

#include "library.h"

class decoder {
public:
    void decode(std::string infile, std::string outfile);
private:
    void create_tree(char *round, size_t bit_count);
    void dfs(Node *cur);
    bool check_bit(char *array, size_t ind);
    template <typename T>
    T to_number(char* array, size_t size);
private:
    int64_t count_bit;
    uint16_t count_symbols;
    Node* tree;
    std::ifstream fin;
    std::ofstream fout;
};

#endif //HUFFMAN_DECODER_H
