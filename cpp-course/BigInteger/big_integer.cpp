//
// Created by Sergey on 20.05.2018.
//

#include <climits>
#include <algorithm>
#include "big_integer.h"
#include <string>
#include <iostream>


big_integer::big_integer() {
    vec.resize(2);
}

big_integer::big_integer(big_integer const &other) {
    vec = other.vec;
}

big_integer::big_integer(std::string str) {
    int negate = (str[0] == '-') ? -1 : 0;
    if (str[0] == '-' || str[0] == '+') {
        str.erase(0, 1);
    }
    while (!str.empty()) {
        uint_fast32_t cur = 0;
        for (int i = 0; i < 32 && !str.empty(); ++i) {
            //нечет
            if (((str.back()) - '0') & 1) {
                cur += 1 << i;
            }
            divide_string(str);
        }
        vec.push_back(cur);
    }
    vec.push_back(0);
    if (negate) {
        *this = -*this;
    }
}

big_integer::big_integer(int a) {
    vec.push_back(static_cast<unsigned>(a));
    vec.push_back((a < 0) ? UINT32_MAX : 0);
}

big_integer::~big_integer() {}

big_integer &big_integer::operator=(big_integer const &other) {
    vec = other.vec;
    return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs) {
    uint_fast32_t sign = vec.back();
    while (vec.size() < rhs.vec.size()) {
        vec.push_back(sign);
    }

    uint_fast32_t carry = 0;
    for (int i = 0; i < rhs.vec.size(); ++i) {
        uint_fast32_t sum = vec[i] + rhs.vec[i] + carry;
        carry = ((sum < rhs.vec[i]) || (sum < vec[i])) ? 1 : 0;
        if (i == rhs.vec.size() - 1 && rhs.vec[i] == UINT32_MAX) {
            carry = 0;
        }

        vec[i] = sum;
    }
    for (int i = rhs.vec.size(); i < vec.size(); ++i) {
        uint_fast32_t sum = vec[i] + carry;
        carry = ((sum < 0) || (sum < vec[i])) ? 1 : 0;
        vec[i] = sum;
        if (carry == 0) {
            break;
        }
    }
    if (vec.back() != 0 && vec.back() != UINT32_MAX) {
        vec.push_back((vec.back() > 0) ? 0 : 1);
    }
    this->del_useless_zero();
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    uint_fast32_t sign = vec.back();
    while (vec.size() < rhs.vec.size()) {
        vec.push_back(sign);
    }
    uint_fast32_t carry = 0;
    for (int i = 0; i < rhs.vec.size(); i++) {
        uint_fast32_t dif = vec[i] - rhs.vec[i] - carry;
        carry = ((vec[i] < rhs.vec[i]) || (vec[i] < dif)) ? 1 : 0;
        vec[i] = dif;
    }
    for (int i = rhs.vec.size(); i < vec.size(); i++) {
        uint_fast32_t dif = vec[i] - carry;
        carry = ((vec[i] < 0) || (vec[i] < dif)) ? 1 : 0;
        vec[i] = dif;
    }
    if (vec.back() != 0 && vec.back() != UINT32_MAX) {
        vec.push_back((vec.back() > 0) ? 0 : UINT32_MAX);
    }
    this->del_useless_zero();
    return *this;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    if (*this == 0) {
        return *this;
    }
    if (rhs == 0) {
        *this = 0;
        return *this;
    }
    bool sign = (vec.back() != rhs.vec.back()) ;
    big_integer const * a;
    big_integer const * b;
    big_integer negate_this;
    big_integer negate_rhs;
    if (rhs.vec.size() > vec.size()) {
        if (this->vec.back() == UINT32_MAX) {
            negate_this = -*this;
            b = &negate_this;
        } else {
            b = this;
        }
        if (rhs.vec.back() == UINT32_MAX) {
            negate_rhs = -rhs;
            a = &negate_rhs;
        } else {
            a = &rhs;
        }
    } else {
        if (this->vec.back() == UINT32_MAX) {
            negate_this = -*this;
            a = &negate_this;
        } else {
            a = this;
        }
        if (rhs.vec.back() == UINT32_MAX) {
            negate_rhs = -rhs;
            b = &negate_rhs;
        } else {
            b = &rhs;
        }
    }
    big_integer ans;
    ans.vec.resize(a->vec.size() + b->vec.size());
    for (int i = 0; i < b->vec.size() - 1; ++i) {
        big_integer cur_ans;
        cur_ans.vec.resize(a->vec.size() + b->vec.size() - 2);
        uint_fast32_t carry = 0;
        for (int j = 0; j < a->vec.size(); j++) {
            uint64_t mul_digit = static_cast<uint64_t >(a->vec[j]) *
                                 static_cast<uint64_t >(b->vec[i]);
            mul_digit += carry;
            cur_ans.vec[i + j] = static_cast<uint_fast32_t>(mul_digit & ((1 << 32) - 1));
            carry = static_cast<uint_fast32_t >((mul_digit) >> 32);
        }
        if (carry < INT32_MAX) {
            if (i + a->vec.size() - 1 >= cur_ans.vec.size()) {
                cur_ans.vec.push_back((carry));
            } else {
                cur_ans.vec[i + a->vec.size() - 1] += carry;
            }
        }
        ans += cur_ans;
    }
    if (ans.vec.back() != 0) {
        ans.vec[ans.vec.size() - 1] = 0;
    }
    if (sign) {
        ans = -ans;
    }
    ans.del_useless_zero();
    *this = ans;
    return *this;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    *this = div_m_in_n(*this, big_integer(rhs));
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    return *this -= rhs * (*this / rhs);
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    return bit_operation(*this, rhs, and_);
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    return bit_operation(*this, rhs, or_);
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    return bit_operation(*this, rhs, xor_);
}

big_integer &big_integer::operator<<=(unsigned rhs) {
    if (rhs == 0) {
        return *this;
    }
    uint_fast32_t sign = vec.back();
    uint_fast32_t big = rhs / 32, small = rhs % 32, r = 0;
    for (size_t i = 0; i < vec.size(); ++i) {
        uint_fast32_t tmp = vec[i] >> (32 - small);
        vec[i] <<= small;
        if (small != 0) {
            vec[i] += r;
        }
        r = tmp;
    }
    this->del_useless_zero();
    vector<uint_fast32_t> tmp_vec(big, 0);
    for (size_t i  = 0; i <vec.size(); ++i) {
        tmp_vec.push_back(vec[i]);
    }
    vec = tmp_vec;
    tmp_vec.~vector();
    if (vec.back() != sign) {
        vec.push_back(sign);
    }
    this->del_useless_zero();
    return *this;
}

big_integer &big_integer::operator>>=(unsigned rhs) {
    if (rhs == 0) {
        return *this;
    }
    uint_fast32_t small = rhs % 32;
    for (int i = 0; i < vec.size() - 1; i++) {
        uint_fast32_t tmp = vec[i + 1] % (1 << small);
        tmp <<= 32 - small;
        vec[i] = (vec[i]) >> small;
        vec[i] += tmp;
    }
    this->del_useless_zero();
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    big_integer res(~(*this));
    res++;
    return res;
}

big_integer big_integer::operator~() const {
    big_integer b_i(*this);
    for (size_t i = 0; i < b_i.vec.size(); ++i) {
        b_i.vec[i] = ~b_i.vec[i];
    }
    b_i.del_useless_zero();
    return b_i;
}

big_integer &big_integer::operator++() {
    *this += 1;
    return *this;
}

big_integer big_integer::operator++(int) {
    big_integer b_i(*this);
    ++(*this);
    return b_i;
}

big_integer &big_integer::operator--() {
    *this -= 1;
    return *this;
}

big_integer big_integer::operator--(int) {
    big_integer b_i(*this);
    --(*this);
    return b_i;
}

bool operator==(big_integer const &a, big_integer const &b) {
    if (a.vec.size() != b.vec.size()) {
        return false;
    }
    for (int i = 0; i < a.vec.size(); ++i) {
        if (a.vec[i] != b.vec[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return b >= a;
}

bool operator>=(big_integer const &a, big_integer const &b) {
    big_integer dif = big_integer(a) - b;
    return (dif.vec.back() == 0);
}

bool operator<(big_integer const &a, big_integer const &b) {
    return (a <= b) && (a != b);
}

bool operator>(big_integer const &a, big_integer const &b) {
    return (a >= b) && (a != b);
}

big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer operator/(big_integer &a, big_integer const &b) {
    return div_m_in_n(a, b);
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer operator<<(big_integer a, unsigned int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, unsigned int b) {
    return a >>= b;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer operator&(big_integer a, uint_fast32_t b) {
    big_integer b_i(b);
    return a & b_i;
}

big_integer operator|(big_integer a, uint_fast32_t b) {
    big_integer b_i(b);
    return a | b_i;
}

big_integer operator^(big_integer a, uint_fast32_t b) {
    big_integer b_i(b);
    return a ^ b_i;
}

std::string to_string(big_integer const &a) {
    bool negate = a.vec.back() == -1;
    big_integer b_i;
    if (negate) {
        b_i = -a;
    } else {
        b_i = a;
    }
    std::string ans = "0";
    for (int i = b_i.vec.size() - 2; i >= 0; i--) {
        for (int shift = sizeof(uint_fast32_t) * 8 - 1; shift >= 0; shift--) {
            multiply_string(ans, ((b_i.vec[i] >> shift) & 1));
        }
    }
    if (negate) {
        ans.insert(ans.begin(), '-');
    }
    return ans;
}

big_integer div_m_in_n(big_integer dividend, big_integer divider) {
    auto m = dividend.vec.size();
    auto n = divider.vec.size();
    if (m < n) {
        return 0;
    }
    bool sign = (dividend.vec.back() != divider.vec.back());
    if (dividend.vec.back() == UINT32_MAX) {
        dividend = -dividend;
    }
    if (divider.vec.back() == UINT32_MAX) {
        divider = -divider;
    }
    if (m - 1 == 1) {
        std::string neg;
        if (sign) {
            neg = "-";
        }
        return big_integer(neg + std::to_string(dividend.vec[0] / divider.vec[0]));
    }
    unsigned shift = normalize(dividend, divider);
    m = dividend.vec.size();
    n = divider.vec.size();
    long long k = m - n - 1;
    if (m - 1 == 2) {
        if (n - 1 == 1) {
            auto res = link(dividend.vec[1], dividend.vec[0]) / static_cast<uint64_t >(divider.vec[0]);
            big_integer quotient;
            quotient.vec[0] = right_half(res);
            quotient.vec[1] = left_half(res);
            quotient.vec.push_back(0);
            if (sign) {
                quotient = -quotient;
            }
            if (quotient.vec.back() == UINT32_MAX && quotient.vec[1] == UINT32_MAX) {
                quotient.vec.pop_back();
            }
            quotient.del_useless_zero();
            return quotient;
        }
        if (n - 1 == 2) {
            auto res = static_cast<int>(right_half(
                    link(dividend.vec[1], dividend.vec[0]) / link(divider.vec[1], divider.vec[0])));
            big_integer ans(res);
            if (sign) {
                ans = -ans;
            }
            return big_integer(res);
        }
    }
    if (n - 1 == 1) {
        return div_m_in_1(dividend, divider.vec[0], sign, shift);
    }
    big_integer quotient;
    quotient.vec.resize(k + 3);
    big_integer shifted = divider << ((k + 1) * 32);
    if (dividend >= shifted) {
        quotient.vec[quotient.vec.size() - 2] = 1;
        dividend -= shifted;
    } else {
        quotient.vec.pop_back();
    }
    while (k >= 0) {
        if (dividend.vec[n + k - 1] == divider.vec[n - 2] && dividend.vec[n + k - 2] == divider.vec[n - 3]) {
            quotient.vec[k] = INT_MAX;
        } else {
            quotient.vec[k] = (div_3_in_2(dividend.vec[n + k - 1], dividend.vec[n + k - 2],
                                          dividend.vec[n + k - 3], divider.vec[n - 2],
                                          divider.vec[n - 3]));
        }
        big_integer ml(quotient.vec[k]);
        big_integer tmp(divider << (k * 32));
        dividend -= abs(ml) * tmp;
        if (dividend.vec.size() == 2 && dividend.vec[0] == 0 && dividend.vec[1] == 0)
            break;
        if (dividend.vec.back() == UINT32_MAX) {
            dividend += divider;
        }
        --k;
    }
    quotient.del_useless_zero();
    if (sign) {
        quotient = -quotient;
    }
    return quotient;
}

big_integer div_m_in_1(big_integer &dividend, uint_fast32_t divider, bool sign, uint_fast32_t shift) {
    uint_fast32_t remainder = 0;
    big_integer quotient;
    quotient.vec.resize(dividend.vec.size());
    for (int i = dividend.vec.size() - 2; i >= 0; --i) {
        std::pair<uint_fast32_t, uint_fast32_t> cur_res = div_2_in_1(remainder, dividend.vec[i], divider);
        quotient.vec[i] = cur_res.first;
        remainder = cur_res.second;
    }
    quotient.del_useless_zero();
    if (sign) {
        quotient = -quotient;
    }
    dividend >>= shift;
    return quotient;
}

uint_fast32_t div_3_in_2(uint_fast32_t dividend1, uint_fast32_t dividend2, uint_fast32_t dividend3, uint_fast32_t divider1, uint_fast32_t divider2) {
    uint64_t dividend = link(dividend1, dividend2);
    uint64_t divider = link(divider1, divider2);
    uint64_t quotient = dividend / divider1;
    uint64_t remainder = ((dividend - quotient * divider1) << 32) | dividend3;
    uint64_t dq = quotient * divider2;
    if (dividend1 == divider1) {
        uint_fast32_t s1 = divider2 - dividend2;
        uint64_t s = (static_cast<uint64_t >(s1) << 32) - dividend3;
        if (s <= divider) {
            quotient = INT_MAX;
            remainder = divider - s;
        } else {
            quotient = INT_MAX - 1;
            remainder = divider << 1 - s;
        }
    }
    if (remainder < dq) {
        --quotient;
        remainder += divider;
        if (remainder >= divider && remainder < dq) {
            --quotient;
        }
    }
    return static_cast<uint_fast32_t>(quotient);
}

std::pair<uint_fast32_t, uint_fast32_t>
div_2_in_1(uint_fast32_t dividend1, uint_fast32_t dividend2, uint_fast32_t divider) {
    auto quotient = static_cast<uint_fast32_t>(link(dividend1, dividend2) / static_cast<uint64_t >(divider));
    auto remainder = static_cast<uint_fast32_t>(link(dividend1, dividend2) % static_cast<uint64_t>(divider));
    return {quotient, remainder};
}

void multiply_string(std::string &str, int carry) {
    for (int i = str.length() - 1; i >= 0; i--) {
        char tmp = str[i];
        str[i] = static_cast<char >((((str[i] - '0') * 2 + carry) % 10) + '0');
        carry = (tmp - '0' >= 5) ? 1 : 0;
    }
    if (carry == 1) {
        str.insert(str.begin(), '1');
    }
}

void divide_string(std::string &str) {
    int remainder = 0;
    int first_zero = 0;
    auto len = str.length();
    for (int i = 0; i < len; i++) {
        int digit = (str[i] - '0');
        int new_digit = ((str[i] - '0') + remainder) / 2;
        str[i] = static_cast<char>(new_digit + '0');
        if (i == 0) {
            if (new_digit == 0) {
                first_zero = -1;
            } else {
                str[i] = (static_cast<char>(new_digit + '0'));
            }
        } else {
            str[i + first_zero] = (static_cast<char>(new_digit + '0'));
        }
        remainder = (digit & 1) ? 10 : 0;
    }
    if (first_zero == -1) {
        str.pop_back();
    }
}

void big_integer:: del_useless_zero() {
    while (vec.size() > 2) {
        if (vec[vec.size() - 2] == 0) {
            if ((vec.back() == 0)) {
                vec.pop_back();
            } else if (vec.back() == UINT32_MAX) {
                vec.pop_back();
                vec.pop_back();
                vec.push_back(UINT32_MAX);
            }
        } else {
            break;
        }
    }
}

uint_fast32_t right_half(uint64_t arg) {
    return static_cast<uint_fast32_t>(arg) & ((1 << 32) - 1);
}

uint_fast32_t left_half(uint64_t arg) {
    return static_cast<uint_fast32_t>(arg >> 32);
}

uint64_t link(uint_fast32_t first, uint_fast32_t second) {
    return static_cast<uint64_t>(first) << 32 | second;
}

uint_fast32_t high_bit_bs(uint_fast32_t n) {
    uint_fast32_t size = sizeof(n) * 4;
    int res = 0;
    while (n != 1) {
        uint_fast32_t l = n >> size;
        if (l) {
            n = l;
            res += size;
        } else {
            n ^= l << size;
        }
        size >>= 1;
    }
    return 31 - res;
}

uint_fast32_t normalize(big_integer &dividend, big_integer &divider) {
    if (divider.vec.back() == UINT32_MAX) {
        return 0;
    }
    uint_fast32_t shift = high_bit_bs(divider.vec[divider.vec.size() - 2]);
    dividend <<= shift;
    divider <<= shift;
    return shift;
}

void extend(big_integer &b_i, uint64_t to_size) {
    uint_fast32_t first_bit = b_i.vec.back();
    while (b_i.vec.size() != to_size) {
        b_i.vec.push_back(first_bit);
    }

}

big_integer abs(big_integer &arg) {
    if (arg >= 0) {
        return arg;
    } else {
        big_integer ans = arg;
        ans.vec[ans.vec.size() - 1] = 0;
        return ans;
    }
}

void or_(uint_fast32_t &a, uint_fast32_t const b) {
    a |= b;
}

void and_(uint_fast32_t &a, uint_fast32_t const b) {
    a &= b;
}

void xor_(uint_fast32_t &a, uint_fast32_t const b) {
    a ^= b;
}

big_integer &bit_operation(big_integer &a, big_integer const &b, void (*operation)(uint_fast32_t &a, uint_fast32_t const b)) {
    uint_fast32_t first_bits;
    if (a.vec.size() > b.vec.size()) {
        first_bits = b.vec.back();
    }
    if (a.vec.size() < b.vec.size()) {
        extend(a, b.vec.size());
    }
    for (int i = 0; i < b.vec.size(); ++i) {
        operation(a.vec[i], b.vec[i]);
    }
    if (b.vec.size() < a.vec.size()) {
        for (auto i = b.vec.size(); i < a.vec.size(); ++i) {
            operation(a.vec[i], first_bits);
        }
    }
    a.del_useless_zero();
    return a;
}
