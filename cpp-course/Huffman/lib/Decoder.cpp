#include "Decoder.h"

void decoder::decode(std::string infile, std::string outfile) {
    fin.open(infile, std::ios::binary);
    if (!fin.is_open()) {
        throw OpenFileException(OpenFileException::type_e::Infile);
    }
    fout.open(outfile, std::ios::binary);
    if (!fout.is_open()) {
        fin.close();
        throw OpenFileException(OpenFileException::type_e::Outfile);
    }
    size_t count_symb_size = (sizeof(count_symbols) + sizeof(char) - 1)/ sizeof(char);
    size_t count_bit_size = (sizeof(count_bit) + sizeof(char) - 1)/ sizeof(char);
    char *count_bit_ch = new char[count_bit_size];
    char *count_symbols_ch = new char[count_symb_size];
    fin.read(count_symbols_ch, sizeof(count_symbols));
    if (fin.gcount() < count_symb_size) {
        fin.close();
        fout.close();
        delete[] count_bit_ch;
        delete[] count_symbols_ch;
        throw DamagedFileException();
    }
    count_symbols = to_number<uint16_t>(count_symbols_ch, count_symb_size);
    if (count_symbols == 0) {
        fin.close();
        fout.close();
        delete[] count_bit_ch;
        delete[] count_symbols_ch;
        return;
    }
    fin.read(count_bit_ch, sizeof(count_bit));
    if (fin.gcount() < count_bit_size) {
        fin.close();
        fout.close();
        delete[] count_bit_ch;
        delete[] count_symbols_ch;
        throw DamagedFileException();
    }
    count_bit = to_number<int64_t>(count_bit_ch, count_bit_size);
    delete[] count_bit_ch;
    delete[] count_symbols_ch;
    if (count_symbols == 1) ++count_symbols;
    size_t round_size = (2 * count_symbols - 2 + 7) / 8;
    char round[round_size];
    fin.read(round, round_size);
    if (fin.gcount() < round_size) {
        fin.close();
        fout.close();
        throw DamagedFileException();
    }
    tree = new Node;
    create_tree(round, 2 * count_symbols - 2);
    dfs(tree);
    char* buffer_in = new char[BUFLEN];
    char* buffer_out = new char[BUFLEN];
    size_t ind_in_buf = 0;
    Node *old_cur = nullptr;
    while (count_bit > 0) {
        if (fin.eof()) {
            delete [] buffer_in;
            delete [] buffer_out;
            fin.close();
            fout.close();
            tree->~Node();
            throw DamagedFileException();
        }
        fin.read(buffer_in, BUFLEN);
        auto count_byte = fin.gcount();
        size_t end = static_cast<size_t>((count_byte * 8 > count_bit) ? count_bit : count_byte * 8);
        count_bit -= count_byte * 8;
        for (size_t i = 0; i < end;) {
            Node *cur;
            if (old_cur != nullptr) {
                cur = old_cur;
                old_cur = nullptr;
            } else {
                cur = tree;
            }
            while (cur->left_son != nullptr) {
                if (i == end) {
                    old_cur = cur;
                    break;
                }
                cur = (check_bit(buffer_in, i++)) ? cur->left_son : cur->right_son;
            }
            if (old_cur != nullptr) {
                break;
            }
            buffer_out[ind_in_buf++] = cur->value;
            if (ind_in_buf == BUFLEN) {
                fout.write(buffer_out, BUFLEN);
                ind_in_buf = 0;
            }
        }
    }
    size_t pos = fin.tellg();
    fin.seekg(0, std::ios::end);
    if (!fin.eof() && pos != fin.tellg()) {
        delete [] buffer_in;
        delete [] buffer_out;
        fin.close();
        fout.close();
        tree->~Node();
        throw DamagedFileException();
    }
    if (ind_in_buf > 0) {
        fout.write(buffer_out, ind_in_buf);
    }
    delete [] buffer_in;
    delete [] buffer_out;
    fin.close();
    fout.close();
    tree->~Node();

}

void decoder::create_tree(char *round, size_t bit_count) {
    Node *cur = tree;
    for (size_t i = 0; i < bit_count; ++i) {
        if (check_bit(round, i)) {
            Node *left_son = new Node(cur);
            cur->left_son = left_son;
            cur = left_son;
        } else {
            while(true) {
                cur = cur->dad;
                if (cur->dad == nullptr || cur->right_son == nullptr) break;
            }
            Node *right_son = new Node(cur);
            cur->right_son = right_son;
            cur = right_son;
        }
    }
}

void decoder::dfs(Node *cur) {
    if ((cur->left_son != nullptr) != (cur->right_son != nullptr)) {
        fin.close();
        fout.close();
        throw DamagedFileException();
    }
    if (cur->left_son != nullptr) {
        dfs(cur->left_son);
        dfs(cur->right_son);
    } else {
        char* ch = new char[1];
        fin.read(ch, 1);
        cur->value = static_cast<unsigned char>(*ch);
    }
}

bool decoder::check_bit(char *array, size_t ind) {
    return (array[ind / (sizeof(char) * 8)] >> (sizeof(char) * 8 - 1 - ind % (sizeof(char) * 8))) & 1;
}

template<typename T>
T decoder::to_number(char *array, size_t size) {
    T number = 0;
    size_t step = (sizeof(T) - sizeof(char)) * 8;
    for (size_t i = 0; i < size; ++i) {
        number |= (T)(unsigned char) array[i] << step;
        step -= sizeof(char) * 8;
    }
    return number;
}