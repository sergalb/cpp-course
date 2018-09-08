#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <utility>
#include "gtest/gtest.h"
#include "lib/library.h"
#include "lib/Encoder.h"
#include "lib/Decoder.h"
#include <sstream>
namespace {
    std::string input(std::string str) {
        std::ofstream in_test("test.txt", std::ios::binary);
        in_test << str;
        in_test.close();
        encoder e;
        e.encode("test.txt", "output.txt");
        decoder d;
        d.decode("output.txt", "decode.txt");
        std::ifstream out_test("decode.txt", std::ios::binary);
        std::string check((std::istreambuf_iterator<char>(out_test)),
                        std::istreambuf_iterator<char>());
        out_test.close();
        return check;
    }
}

TEST(correctness, hello_world)
{
    std::string str = "hello world";
    std::string check = input(str);
    EXPECT_EQ(str, check);
}

TEST(correctness, empty)
{
    std::string str = "";
    std::string check = input(str);
    EXPECT_EQ(str, check);
}

namespace {
    bool generate(size_t size) {
        std::ofstream in_test("test.txt", std::ios::binary);
        char* in = new char[size];
        for (size_t i = 0; i < size; ++i) {
            in[i] = (char) rand();
        }
        in_test.write(in, size);
        in_test.close();
        encoder e;
        e.encode("test.txt", "output.txt");
        decoder d;
        d.decode("output.txt", "decode.txt");
        std::ifstream out_test("decode.txt", std::ios::binary);
        char* out = new char[size];
        out_test.read(out, size);
        bool result = true;
        for (size_t i = 0; i < size; ++i) {
            if (in[i] != out[i]) {
                result = false;
                std::cout << std::endl << i <<' ' << (unsigned int) in[i] << ' ' << (unsigned int) (unsigned char) out[i] <<std::endl;
                break;
            }
        }
        delete[] in;
        delete[] out;
        return result;
    }
}

TEST(correctness, big_random)
{
    for (size_t i = 0; i < 10; ++i) {
        EXPECT_TRUE(generate(1000));
    }
}


TEST(correctness, photo) {
    encoder e;
    e.encode("photo_in.jpg", "photo_out.jpg");
    decoder d;
    d.decode("photo_out.jpg", "photo_decode.jpg");
}

TEST(correctness, buffer) {
    EXPECT_TRUE(generate(BUFLEN * 4));
}

