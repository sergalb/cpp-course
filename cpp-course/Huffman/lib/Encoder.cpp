#include "Encoder.h"
#include <algorithm>
#include <cassert>

void encoder::encode(std::string in_file, std::string outfile) {
    fin.open(in_file, std::ios::binary);
    if (!fin.is_open()) {
        throw OpenFileException(OpenFileException::type_e::Infile);
    }
    fout.open(outfile, std::ios::binary);
    if (!fout.is_open()) {
        fin.close();
        throw OpenFileException(OpenFileException::type_e::Outfile);
    }
    set_alp();
    fout.write(to_char_array(count_symbols), sizeof(count_symbols));
    if (count_symbols == 0) {
        fin.close();
        fout.close();
        return;
    }
    if (count_symbols == 1) ++count_symbols;
    round.resize((count_symbols * 2 - 2 + 7) / 8);
    create_tree();
    std::vector<bool> tmp_code;
    uint16_t zero = 0;
    dfs(tree, tmp_code, zero);
    uint64_t count = count_bit();
    fout.write(to_char_array(count), sizeof(count));
    fout.write(&round[0], sizeof(char) * round.size());
    fout.write(&order[0], sizeof(char) * order.size());
    fin.clear();
    fin.seekg(0, std::ios_base::beg);
    char *buffer_in = new char[BUFLEN];
    char *buffer_out = new char[BUFLEN];
    const size_t BIT_SIZE = BUFLEN * sizeof(char) * 8;
    size_t ind_in_buf = 0;
    while (!fin.eof()) {
        fin.read(buffer_in, BUFLEN);
        auto end = fin.gcount();
        for (size_t i = 0; i < end; ++i) {
            size_t remainder = write_to_buffer(codes[(unsigned char) buffer_in[i]], 0, buffer_out, ind_in_buf, BIT_SIZE);
            if (ind_in_buf == BIT_SIZE) {
                fout.write(buffer_out, BIT_SIZE/8);
                ind_in_buf = 0;
                write_to_buffer(codes[(unsigned char) buffer_in[i]], codes[(unsigned char) buffer_in[i]].size() - remainder, buffer_out, ind_in_buf, remainder);
            }
        }
    }
    if (ind_in_buf > 0) {
        //(ind_in_buf + 7) / 8 - округление вверх до целого числа байт
        fout.write(buffer_out, sizeof(char) * ((ind_in_buf + 7)/8));
    }
    delete [] buffer_in;
    delete [] buffer_out;
    fin.close();
    fout.close();
    tree->~Node();
}

size_t encoder::write_to_buffer(std::vector<bool> const &code, size_t code_start, char *buffer, size_t &ind, const size_t BIT_SIZE) {
    size_t reminder = 0;
    for (size_t i = code_start; i < code.size(); ++i) {
        if (ind == BIT_SIZE) {
            reminder = code.size() - i;
            break;
        }
        if (code[i]) {
            set_1(buffer, ind);
        } else {
            set_0(buffer, ind);
        }
        ++ind;
    }
    return reminder;
}

void encoder::set_alp() {
    count_symbols = 0;
    char *buffer = new char[BUFLEN];
    bool eof = fin.eof();
    while (!eof) {
        fin.read(buffer, BUFLEN);
        eof = fin.eof();
        auto end = fin.gcount();
        for (size_t i = 0; i < end; ++i) {
            if (alp[(unsigned char) buffer[i]]++ == 0) ++count_symbols;
        }
    }
    delete [] buffer;
}

void encoder::create_tree() {
    std::vector<Node*> first_ar;
    std::vector<Node*> second_ar;
    for (size_t let = 0; let < ALPSIZE; ++let) {
        if (alp[let] != 0) first_ar.emplace_back(new Node(let, alp[let]));
    }
    std::sort(std::begin(first_ar), std::end(first_ar), compare);
    size_t first_ind = 0, second_ind = 0;
    if (first_ar.size() == 1) {
        tree = new Node;
        Node *fake = new Node('\0', UINT64_MAX);
        tree->left_son = first_ar.back();
        tree->right_son = fake;
        return;
    }
    ///опасная second_ar.size() -1
    while (first_ind < first_ar.size() || second_ind < second_ar.size() - 1) {
        Node *left_son = (first_ind < first_ar.size() && (second_ind >= second_ar.size() ||
                                                          first_ar[first_ind]->weight <= second_ar[second_ind]->weight))
                         ? first_ar[first_ind++] : second_ar[second_ind++];
        Node *right_son = (first_ind < first_ar.size() && (second_ind >= second_ar.size() ||
                                                           first_ar[first_ind]->weight <= second_ar[second_ind]->weight))
                          ? first_ar[first_ind++] : second_ar[second_ind++];
        Node *dad = new Node(left_son->weight + right_son->weight, *left_son, *right_son);
        second_ar.push_back(dad);
    }
    tree = second_ar.back();
}

void encoder::
dfs(Node *cur, std::vector<bool> code, uint16_t &iter) {
    assert((cur->left_son != nullptr) == (cur->right_son != nullptr));
    if (cur->left_son != nullptr) {
        set_1(&round[0], iter++);
        code.push_back(true);
        dfs(cur->left_son, code, iter);
        set_0(&round[0], iter++);
        code.back() = false;
        dfs(cur->right_son, code, iter);
    } else {
        order.push_back(cur->value);
        codes[cur->value] = code;
    }
}

uint64_t encoder::count_bit() {
    uint64_t count = 0;
    for(size_t i = 0; i < ALPSIZE; ++i)
        count += alp[i] * codes[i].size();
    return count;
}


void encoder::set_0(char *array, size_t ind) {
    array[ind / (sizeof(char) * 8)] &= ~(1 << (sizeof(char) * 8 - 1 - (ind % (sizeof(char) * 8))));
}

void encoder::set_1(char *array, size_t ind){
    array[ind / (sizeof(char) * 8)] |= 1 << (sizeof(char) * 8 - 1 - (ind % (sizeof(char) * 8)));
}

template <typename T>
char *encoder::to_char_array(T number) {
    size_t size = (sizeof(T) + sizeof(char) - 1)/ sizeof(char);
    char *ch_ar = new char[size];
    size_t step = (sizeof(T) - sizeof(char)) * 8;
    for (size_t i = 0; i < size; ++i) {
        ch_ar[i] &= 0;
        ch_ar[i] |= number >> step;
        step -= sizeof(char) * 8;
    }
    return ch_ar;
}