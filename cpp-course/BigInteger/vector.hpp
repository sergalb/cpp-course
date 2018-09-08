//
// Created by BalahninS on 28.07.2018.
//

#include <memory.h>
#include "vector.h"
#include <cassert>


template<typename T>
vector<T>::vector() {
    kind_data = STACK;
    memset(stack_data, 0, LIMIT * sizeof(T));
    cur_data = stack_data;
    _size = 0;
}

template<typename T>
vector<T>::vector(vector<T> const &other) : _size(other._size), capacity(other.capacity), kind_data(other.kind_data) {
    if (other.size() <= LIMIT) {
        //todo std::copy
        for (int i = 0; i < other.size(); ++i)
            stack_data[i] = other.stack_data[i];
        cur_data = stack_data;
    } else {
        heap_data = other.heap_data;
        cur_data = heap_data.get();
    }

}

template<typename T>
vector<T>::vector(size_t size, T value) : _size(size){
    if (size > LIMIT) {
        kind_data = HEAP;
        heap_data = std::shared_ptr<T>(new T[size * 2], deleter());
        capacity = 2 * size;
        cur_data = heap_data.get();
    } else {
        kind_data = STACK;
        capacity = 0;
        cur_data = stack_data;
    }
    for (size_t i = 0; i < size; ++i) {
        cur_data[i] = value;
    }
}
template<typename T>
vector<T>::~vector() {
    if (kind_data == HEAP) {
        heap_data.reset();
    }
}

template<typename T>
vector<T>& vector<T>::operator=(vector const &other) {
    if (&other != this) {
        if (other.size() <= LIMIT) {
            //todo std::copy
            for (int i = 0; i < other.size(); ++i)
                stack_data[i] = other[i];
            cur_data = stack_data;
        } else {
            heap_data = other.heap_data;
            cur_data = heap_data.get();
        }
        _size = other._size;
        kind_data = other.kind_data;
        capacity = other.capacity;
    }
    return *this;
}

template<typename T>
void vector<T>::push_back(T element) {
    if (_size == LIMIT)  {
        kind_data = HEAP;
        heap_data = std::shared_ptr<T>(copy_at_heap(stack_data, _size, 2 * LIMIT), deleter());
        cur_data = heap_data.get();
    } else if (kind_data == HEAP && (heap_data.use_count() > 1 || _size == capacity)) {
        size_t new_capacity = (_size == capacity) ? capacity * 2 : capacity;
        heap_data.reset(copy_at_heap(heap_data.get(), _size, new_capacity), deleter());
        cur_data = heap_data.get();
    }
    cur_data[_size++] = element;
}

template<typename T>
void vector<T>::pop_back() {
    assert(_size > 0);
    if (_size == LIMIT + 1) {
        //todo ускорить
        for (int i = 0; i < LIMIT; ++i)
            stack_data[i] = cur_data[i];
        kind_data = STACK;
        capacity = 0;
        heap_data.reset();
        cur_data = stack_data;
    } else if (_size > LIMIT + 1 && heap_data.use_count() > 1) {
        heap_data.reset(copy_at_heap(cur_data, _size, capacity), deleter());
        cur_data = heap_data.get();
    }
    --_size;
}


template<typename T>
T const &vector<T>::operator[](size_t ind) const {
    assert(ind >= 0 && ind < _size);
    return cur_data[ind];
}

template<typename T>
T &vector<T>::operator[](size_t ind) {
    assert(ind >= 0 && ind < _size);
    if (kind_data == HEAP && heap_data.use_count() > 1) {
        heap_data.reset(copy_at_heap(cur_data, _size, capacity), deleter());
        cur_data = heap_data.get();
    }
    return cur_data[ind];
}


template<typename T>
T const &vector<T>::back() const {
    return cur_data[_size - 1];
}

template<typename T>
T &vector<T>::back() {
    if (kind_data == HEAP && heap_data.use_count() > 1) {
        heap_data.reset(copy_at_heap(cur_data, _size, capacity), deleter());
        cur_data = heap_data.get();
    }
    return cur_data[_size - 1];
}

template<typename T>
void vector<T>::resize(size_t new_size) {
    if (new_size > LIMIT) {
        if (kind_data == STACK) {
            kind_data = HEAP;
            heap_data = std::shared_ptr<T>(copy_at_heap(stack_data, _size, new_size * 2), deleter());
        } else {
            heap_data.reset(copy_at_heap(cur_data, _size, new_size * 2), deleter());
        }
        cur_data = heap_data.get();
    } else {
        for (int i = 0; i < LIMIT; ++i) {
            stack_data[i] = cur_data[i];
        }
        kind_data = STACK;
        capacity = _size;
        cur_data = stack_data;
    }
    while (_size < new_size) {
        cur_data[_size++] = T();
    }
}

template<typename T>
size_t vector<T>::size() const {
    return _size;
}

//todo поменять на std::copy с итераторами
template<typename T>
T *vector<T>::copy_at_heap(T *old_arr, size_t old_size, size_t new_capacity) {
    T* tmp = new T [new_capacity];
    memcpy(tmp, old_arr, sizeof(T) * old_size);
    capacity = new_capacity;
    return tmp;
}

template<typename T>
T *vector<T>::begin() {
    return cur_data;
}

template<typename T>
T *vector<T>::end() {
    return cur_data + _size;
}

template<typename T>
bool vector<T>::operator==(vector const &other) const {
    if (_size != other.size()) {
        return false;
    }
    if (cur_data == other.cur_data) {
        return true;
    }
    for (int i = 0; i < _size; ++i) {
        if (cur_data[i] != other[i]) {
            return false;
        }
    }
    return true;
}
