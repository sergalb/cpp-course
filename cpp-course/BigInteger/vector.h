//
// Created by BalahninS on 28.07.2018.
//

#ifndef BIGINTEGER_VECTOR_H
#define BIGINTEGER_VECTOR_H


#include <cstddef>
#include <memory>
#include <iostream>
constexpr size_t LIMIT = 2;

template <typename T>
class vector {
public:
    vector();
    vector(vector<T> const &other);
    vector(size_t size, T value = T());
    ~vector();
    vector& operator=(vector const &other);
    void resize(size_t new_size);
    size_t size() const;
    void push_back(T element);
    void pop_back();
    T const &back() const;
    T& back();
    void erase();
    T const &operator[] (size_t ind) const;
    T& operator [] (size_t ind);
    //todo correct definition for iterators
    T* begin();
    T* end();
    bool operator==(vector const &other) const;

private:
    T* copy_at_heap(T* old_arr, size_t old_size, size_t new_capacity);

private:
    size_t _size;
    enum kind_data_e {
        HEAP, STACK
    } kind_data;
    std::shared_ptr<T> heap_data;
    size_t capacity = 0 ;
    struct deleter {
        void operator()(T* ptr_del) const {
            delete [] ptr_del;

        }
    };
    T stack_data[LIMIT];
    T* cur_data;
};

#include "vector.hpp"
#endif //BIGINTEGER_VECTOR_H
