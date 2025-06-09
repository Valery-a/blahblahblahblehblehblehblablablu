#ifndef MYVECTOR_HPP
#define MYVECTOR_HPP

#include <stdexcept>

template <typename T>
class MyVector {
private:
    T* data;
    size_t capacity;
    size_t count;

    void resize();

public:
    MyVector();
    ~MyVector();

    void push_back(const T& item);
    void removeAt(size_t index);
    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    size_t size() const;
    bool empty() const;
};

template <typename T>
MyVector<T>::MyVector() : capacity(4), count(0) {
    data = new T[capacity];
}

template <typename T>
MyVector<T>::~MyVector() {
    delete[] data;
}

template <typename T>
void MyVector<T>::resize() {
    capacity *= 2;
    T* newData = new T[capacity];

    for (size_t i = 0; i < count; ++i) {
        newData[i] = data[i];
    }

    delete[] data;
    data = newData;
}

template <typename T>
void MyVector<T>::push_back(const T& item) {
    if (count == capacity) {
        resize();
    }
    data[count++] = item;
}

template <typename T>
void MyVector<T>::removeAt(size_t index) {
    if (index >= count) throw std::out_of_range("Index out of range");

    for (size_t i = index; i < count - 1; ++i) {
        data[i] = data[i + 1];
    }
    --count;
}

template <typename T>
T& MyVector<T>::operator[](size_t index) {
    if (index >= count) throw std::out_of_range("Index out of range");
    return data[index];
}

template <typename T>
const T& MyVector<T>::operator[](size_t index) const {
    if (index >= count) throw std::out_of_range("Index out of range");
    return data[index];
}

template <typename T>
size_t MyVector<T>::size() const {
    return count;
}

template <typename T>
bool MyVector<T>::empty() const {
    return count == 0;
}

#endif
