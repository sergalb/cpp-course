//
// Created by Sergey on 08.09.2018.
//

#ifndef FIXED_VECTOR_FIXEDVECTOR_H
#define FIXED_VECTOR_FIXEDVECTOR_H

#include <cstddef>
#include <type_traits>
#include <algorithm>
#include <cassert>
#include <cstdint>

template <typename T, size_t N>
class fixed_vector {
public:
    using iterator = T *;
    using const_iterator = T const *;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    fixed_vector() noexcept;
    fixed_vector(fixed_vector<T, N> const &other);
    fixed_vector& operator=(fixed_vector const &other);
    ~fixed_vector() noexcept;
    T &operator[](size_t i);
    T const &operator[](size_t i) const;
    void push_back(T const &element);
    void pop_back();
    const_iterator insert(const_iterator pos, T const &val);
    const_iterator erase(const_iterator pos);
    T &back();
    T const &back() const;
    T &front();
    T const &front() const;
    void swap(fixed_vector<T, N> & other);
    size_t max_size();
    size_t size() const ;
    size_t capacity() const;
    bool empty() const;
    void clear();
    iterator begin();
    const_iterator begin() const ;
    iterator end();
    const_iterator end() const;
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const ;
    reverse_iterator rend();
    const_reverse_iterator rend() const;

private:
    void shift(const_iterator begin, const_iterator end, bool to_right);

private:
    size_t _size;
    size_t _capacity;
    typename std::aligned_storage<sizeof(T),alignof(T)>::type data[N];
};

template<typename T, size_t N>
fixed_vector<T, N>::fixed_vector() noexcept : _size(0), _capacity(N){};

template<typename T, size_t N>
fixed_vector<T, N>::fixed_vector(fixed_vector<T, N> const &other) : _size(other._size), _capacity(N) {
    std::copy(other.begin(), other.end(), this->begin());
}

template<typename T, size_t N>
fixed_vector<T, N>& fixed_vector<T, N>::operator=(fixed_vector const &other) {
    _size = other.size();
    _capacity = N;
    std::copy(other.begin(), other.end(), this->begin());
    return *this;
}

template<typename T, size_t N>
fixed_vector<T, N>::~fixed_vector() noexcept {
    for(size_t pos = 0; pos < _size; ++pos) {
        reinterpret_cast<const T*>(data + pos)->~T();
    }
}

template<typename T, size_t N>
T &fixed_vector<T, N>::operator[](size_t i) {
    return *reinterpret_cast<T*>(data + i);
}

template<typename T, size_t N>
T const &fixed_vector<T, N>::operator[](size_t i) const {
    return *reinterpret_cast<const T*>(data + i);
}

template<typename T, size_t N>
T &fixed_vector<T, N>::back() {
    assert(_size > 0);
    return (*this)[_size - 1];
}

template<typename T, size_t N>
T const &fixed_vector<T, N>::back() const {
    assert(_size > 0);
    return (*this)[_size - 1];
}

template<typename T, size_t N>
T &fixed_vector<T, N>::front() {
    assert(_size > 0);
    return (*this)[0];
}

template<typename T, size_t N>
T const &fixed_vector<T, N>::front() const {
    assert(_size > 0);
    return (*this)[0];
}

template<typename T, size_t N>
void fixed_vector<T, N>::push_back(T const &element) {
    assert(_size < _capacity);
    new(data + _size++) T(std::forward<T const>(element));
};

template<typename T, size_t N>
void fixed_vector<T, N>::pop_back() {
    assert(_size > 0);
    this->back().~T();
    --_size;
}

template<typename T, size_t N>
typename fixed_vector<T,N>::const_iterator fixed_vector<T, N>::insert(const_iterator pos, T const &val) {
    assert(_size < _capacity);
    if (pos == end()) {
        push_back(val);
        return pos;
    }
    shift(pos, end() + 1, true);
    new(const_cast<T *>(pos)) T(std::forward<T const>(val));
    ++_size;
    return pos;

}

template<typename T, size_t N>
typename fixed_vector<T,N>::const_iterator fixed_vector<T, N>::erase(const_iterator pos) {
    assert(_size > 0);
    if (pos == end()) {
        pop_back();
        return pos;
    }
    pos->~T();
    shift(end() - 1, pos, false);
    --_size;
    return pos;
}

template<typename T, size_t N>
void fixed_vector<T, N>::swap(fixed_vector<T, N> &other) {
    std::swap(this->data, other.data);
}

template<typename T, size_t N>
typename fixed_vector<T,N>::iterator  fixed_vector<T, N>::begin() {
    return reinterpret_cast<T *>(data);
}

template<typename T, size_t N>
typename fixed_vector<T,N>::const_iterator fixed_vector<T, N>::begin() const {
    return reinterpret_cast<T const *>(data);
}

template <typename T, size_t N>
typename fixed_vector<T,N>::iterator fixed_vector<T, N>::end() {
    return reinterpret_cast<T *>(data + _size);
}

template <typename T, size_t N>
typename fixed_vector<T,N>::const_iterator fixed_vector<T, N>::end() const{
    return reinterpret_cast<T const *>(data + _size);
}

template<typename T, size_t N>
typename fixed_vector<T,N>::reverse_iterator  fixed_vector<T, N>::rbegin() {
    return reverse_iterator(end());
}

template<typename T, size_t N>
typename fixed_vector<T,N>::const_reverse_iterator  fixed_vector<T, N>::rbegin() const {
    return const_reverse_iterator(end());
}

template <typename T, size_t N>
typename fixed_vector<T,N>::reverse_iterator fixed_vector<T, N>::rend() {
    return reverse_iterator(begin());
}

template <typename T, size_t N>
typename fixed_vector<T,N>::const_reverse_iterator fixed_vector<T, N>::rend() const{
    return const_reverse_iterator(begin());
}

template<typename T, size_t N>
size_t fixed_vector<T, N>::max_size() {
    return N;
}

template<typename T, size_t N>
size_t fixed_vector<T, N>::size() const{
    return _size;
}

template<typename T, size_t N>
size_t fixed_vector<T, N>::capacity() const {
    return _capacity;
}

template<typename T, size_t N>
void fixed_vector<T, N>::shift(typename fixed_vector<T,N>::const_iterator begin, typename fixed_vector<T,N>::const_iterator end, bool to_right) {
    int direction = to_right ? 1 : -1;
    for (typename fixed_vector<T,N>::const_iterator i = end; i != begin; i -= direction) {
        if(i != end) {
            (*i).~T();
        }
        new(const_cast<T *>(i)) T(std::forward<T const>(*(i - direction)));
    }
}

template<typename T, size_t N>
bool fixed_vector<T, N>::empty() const {
    return _size == 0;
}

template<typename T, size_t N>
void fixed_vector<T, N>::clear() {
    while(!empty()) {
        pop_back();
    }
}


#endif //FIXED_VECTOR_FIXEDVECTOR_H
