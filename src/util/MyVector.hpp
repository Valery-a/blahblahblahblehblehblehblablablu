#ifndef MYVECTOR_HPP
#define MYVECTOR_HPP

#include <stdexcept>

template <typename T>
class MyVector {
private:
    T*      data;
    size_t  capacity;
    size_t  count;

public:
    MyVector()
      : data(new T[4]), capacity(4), count(0)
    {}

    MyVector(const MyVector& other)
      : data(new T[other.capacity]),
        capacity(other.capacity),
        count(other.count)
    {
        for (size_t i = 0; i < count; ++i) {
            data[i] = other.data[i];
        }
    }

    MyVector& operator=(MyVector other) {
        swap(*this, other);
        return *this;
    }

    ~MyVector() {
        delete[] data;
    }

    void push_back(const T& item) {
        if (count == capacity) {
            resize();
        }
        data[count++] = item;
    }

    void removeAt(size_t index) {
        if (index >= count) {
            throw std::out_of_range("Index out of range");
        }
        for (size_t i = index; i + 1 < count; ++i) {
            data[i] = data[i + 1];
        }
        --count;
    }

    T& operator[](size_t index) {
        if (index >= count) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }
    const T& operator[](size_t index) const {
        if (index >= count) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    size_t size() const {
        return count;
    }

    bool empty() const {
        return count == 0;
    }

    friend void swap(MyVector& a, MyVector& b) noexcept {
        T*   tmpData    = a.data;
        a.data         = b.data;
        b.data         = tmpData;
        size_t tmpCap  = a.capacity;
        a.capacity     = b.capacity;
        b.capacity     = tmpCap;
        size_t tmpCnt  = a.count;
        a.count        = b.count;
        b.count        = tmpCnt;
    }

private:
    void resize() {
        size_t newCap = capacity * 2;
        T* newData = new T[newCap];
        for (size_t i = 0; i < count; ++i) {
            newData[i] = data[i];
        }
        delete[] data;
        data     = newData;
        capacity = newCap;
    }
};

#endif
