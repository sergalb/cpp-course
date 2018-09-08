#include "lib/library.h"
#include "lib/Encoder.h"
#include "lib/Decoder.h"
#include <iterator>
#include <iostream>

int main ()
{
    while (true) {
        enum {
            ENCODE, DECODE
        } choose;
        while (true) {
            std::cout << "Do you want encode or decode file [e/d]" << std::endl;
            char ch;
            std::cin >> ch;
            if (ch == 'e' || ch == 'E') {
                choose = ENCODE;
                break;
            } else if (ch == 'd' || ch == 'D') {
                choose = DECODE;
                break;
            }
        }
        std::cout << "Enter name of input and output files" << std::endl;
        std::string infile, outfile;
        std::cin >> infile >> outfile;
        if (choose == ENCODE) {
            try {
                encoder e;
                e.encode(infile, outfile);
                std::cout << "Encoding was completed successful" << std::endl;
            } catch (OpenFileException &f_e) {
                std::cout << "Couldn't open ";
                if (f_e.type == OpenFileException::type_e::Infile) {
                    std::cout << infile <<std::endl;
                } else {
                    std::cout << outfile << std::endl;
                }
            }
        } else {
            try {
                decoder d;
                d.decode(infile, outfile);
                std::cout << "Decoding was completed successful" << std::endl;
            } catch (OpenFileException &f_e) {
                std::cout << "Couldn't open";
                if (f_e.type == OpenFileException::type_e::Infile) {
                    std::cout << infile <<std::endl;
                } else {
                    std::cout << outfile << std::endl;
                }
            } catch (DamagedFileException &d_e) {
                std::cout << "Input file is damaged" << std::endl;
            }
        }
        std::cout << "Do you want continue work? [y/n]";
        char exit;
        std::cin >> exit;
        if (exit == 'n' || exit == 'N') {
            break;
        }
    }
}