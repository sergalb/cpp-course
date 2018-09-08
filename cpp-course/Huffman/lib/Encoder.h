#ifndef HUFFMAN_ENCODER_H
#define HUFFMAN_ENCODER_H

#ifndef HUFFMAN_LIBRARY_H
#include "library.h"
#endif



class encoder {
public:
    void encode(std::string in_file, std::string out_file);

private:
    void set_alp();
    void create_tree();
    void dfs(Node *cur, std::vector<bool> code, uint16_t &iter);
    //возвращает количество бит, которое не поместилось в буффер при записи
    size_t write_to_buffer(std::vector<bool> const &code, size_t code_start, char* buffer, size_t &ind, const size_t BIT_SIZE);
    uint64_t count_bit();
    void set_1(char *array, size_t ind);
    void set_0(char *array, size_t ind);
    template <typename T>
    char* to_char_array(T number);
private:
    uint64_t alp[ALPSIZE] = {};
    uint16_t count_symbols;
    Node* tree;
    std::ifstream fin;
    std::ofstream fout;
    std::vector<char> round;
    std::vector<char> order;
    std::vector<bool> codes[ALPSIZE];
};

#endif //HUFFMAN_ENCODER_H
