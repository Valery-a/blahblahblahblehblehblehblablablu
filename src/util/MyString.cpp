#include "MyString.h"
#include <cstring>
#include <stdexcept>

void MyString::copyFrom(const MyString& other) {
    data = new char[other.length + 1];
    std::strcpy(data, other.data);
    length = other.length;
}

void MyString::free() {
    delete[] data;
}

MyString::MyString() : data(new char[1]), length(0) {
    data[0] = '\0';
}

MyString::MyString(const char* str) {
    length = std::strlen(str);
    data = new char[length + 1];
    std::strcpy(data, str);
}

MyString::MyString(const MyString& other) {
    copyFrom(other);
}

MyString& MyString::operator=(const MyString& other) {
    if (this != &other) {
        free();
        copyFrom(other);
    }
    return *this;
}

MyString::~MyString() {
    free();
}

size_t MyString::size() const {
    return length;
}

const char* MyString::c_str() const {
    return data;
}

bool MyString::operator==(const MyString& other) const {
    return std::strcmp(data, other.data) == 0;
}

bool MyString::operator!=(const MyString& other) const {
    return !(*this == other);
}

MyString MyString::operator+(const MyString& other) const {
    size_t newLen = length + other.length;
    char* newData = new char[newLen + 1];
    std::strcpy(newData, data);
    std::strcat(newData, other.data);

    MyString result(newData);
    delete[] newData;
    return result;
}

char& MyString::operator[](size_t index) {
    if (index >= length) throw std::out_of_range("Index out of bounds");
    return data[index];
}

const char& MyString::operator[](size_t index) const {
    if (index >= length) throw std::out_of_range("Index out of bounds");
    return data[index];
}

std::ostream& operator<<(std::ostream& os, const MyString& str) {
    os << str.data;
    return os;
}

std::istream& operator>>(std::istream& is, MyString& str) {
    char buffer[1024];
    is >> buffer;
    str = MyString(buffer);
    return is;
}
