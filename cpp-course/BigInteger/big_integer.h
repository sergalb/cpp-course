#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
//#include <gmp.h>
#include <iosfwd>
#include <stdint-gcc.h>
#include <vector>
#include <string>
#include "vector.h"

//typedef ui_vector vector;
struct big_integer {

    big_integer();

    big_integer(big_integer const &other);

    big_integer(int a);

    explicit big_integer(std::string str);

    ~big_integer();

    big_integer &operator=(big_integer const &other);

    big_integer &operator+=(big_integer const &rhs);

    big_integer &operator-=(big_integer const &rhs);

    big_integer &operator*=(big_integer const &rhs);

    big_integer &operator/=(big_integer const &rhs);

    big_integer &operator%=(big_integer const &rhs);

    big_integer &operator&=(big_integer const &rhs);

    big_integer &operator|=(big_integer const &rhs);

    big_integer &operator^=(big_integer const &rhs);

    big_integer &operator<<=(unsigned rhs);

    big_integer &operator>>=(unsigned rhs);

    big_integer operator+() const;

    big_integer operator-() const;

    big_integer operator~() const;

    big_integer &operator++();

    big_integer operator++(int);

    big_integer &operator--();

    big_integer operator--(int);

    friend bool operator==(big_integer const &a, big_integer const &b);

    friend bool operator!=(big_integer const &a, big_integer const &b);

    friend bool operator<(big_integer const &a, big_integer const &b);

    friend bool operator>(big_integer const &a, big_integer const &b);

    friend bool operator<=(big_integer const &a, big_integer const &b);

    friend bool operator>=(big_integer const &a, big_integer const &b);

    friend std::string to_string(big_integer const &b_i);


private:
    vector<uint_fast32_t> vec;
    void del_useless_zero();
    friend big_integer div_m_in_n(big_integer dividend, big_integer divider);
    friend big_integer div_m_in_1(big_integer &dividend, uint_fast32_t divider, bool sign, uint_fast32_t shift);
    friend unsigned int normalize(big_integer &dividend, big_integer &divider);
    friend void extend(big_integer &b_i, uint64_t to_size);

    friend big_integer abs(big_integer &arg);
    friend big_integer &
    bit_operation(big_integer &a, big_integer const &b, void (*operation)(uint_fast32_t &a, uint_fast32_t const b));
};

big_integer operator+(big_integer a, big_integer const &b);
big_integer operator-(big_integer a, big_integer const &b);
big_integer operator*(big_integer a, big_integer const &b);
big_integer operator/(big_integer &a, big_integer const &b);
big_integer operator%(big_integer a, big_integer const &b);
big_integer operator&(big_integer a, big_integer const &b);
big_integer operator|(big_integer a, big_integer const &b);
big_integer operator^(big_integer a, big_integer const &b);
big_integer operator&(big_integer a, uint_fast32_t b);
big_integer operator|(big_integer a, uint_fast32_t b);
big_integer operator^(big_integer a, uint_fast32_t b);
big_integer operator<<(big_integer a, unsigned int b);
big_integer operator>>(big_integer a, unsigned int b);
bool operator==(big_integer const &a, big_integer const &b);
bool operator!=(big_integer const &a, big_integer const &b);
bool operator<(big_integer const &a, big_integer const &b);
bool operator>(big_integer const &a, big_integer const &b);
bool operator<=(big_integer const &a, big_integer const &b);
bool operator>=(big_integer const &a, big_integer const &b);
std::string to_string(big_integer const &b_i);
std::ostream &operator<<(std::ostream &s, big_integer const &a);


void multiply_string(std::string &str, int carry);
void divide_string(std::string &str);
uint_fast32_t right_half(uint64_t arg);
uint_fast32_t left_half(uint64_t arg);
uint_fast32_t div_3_in_2(uint_fast32_t dividend1, uint_fast32_t dividend2, uint_fast32_t dividend3, uint_fast32_t divider1,
                    uint_fast32_t divider2);
std::pair<uint_fast32_t, uint_fast32_t>
div_2_in_1(uint_fast32_t dividend1, uint_fast32_t dividend2, uint_fast32_t divider);
uint64_t link(uint_fast32_t first, uint_fast32_t second);
uint_fast32_t inline normalize(big_integer &dividend, big_integer &divider);
uint_fast32_t inline normalize(big_integer &dividend, uint_fast32_t &divider);
void inline extend(big_integer &b_i, uint64_t to_size);
big_integer &bit_operation(big_integer &a, big_integer const &b, void (*operation)(uint_fast32_t &a, uint_fast32_t const b));
void or_(uint_fast32_t &a, uint_fast32_t const b);
void and_(uint_fast32_t &a, uint_fast32_t const b);
void xor_(uint_fast32_t &a, uint_fast32_t const b);

#endif // BIG_INTEGER_H


