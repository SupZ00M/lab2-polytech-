#include "My_string.hpp"
#include <cstring>
#include <string>
#include <exception>

MyString::MyString() {
    length = 0;
    data = new char[1];
    data[0] = '\0';
}

MyString::MyString(const char* str) {
    if (!str) {
        length = 0;
        data = new char[1];
        data[0] = '\0';
    }
    else {
        length = strlen(str);
        data = new char[length + 1];
        strcpy(data, str);
    }
}

MyString::MyString(const MyString& other) {
    length = other.length;
    data = new char[length + 1];
    strcpy(data, other.data);
}

MyString::~MyString() {
    delete[] data;
}

char MyString::get(int i) const {
    if (i < 0 || i >= length) throw std::runtime_error("Index out of range");
    return data[i];
}

void MyString::set(int i, char c) {
    if (i < 0 || i >= length) throw std::runtime_error("Index out of range");
    data[i] = c;
}

void MyString::set_new_string(const char* str) {
    delete[] data;
    if (!str) {
        length = 0;
        data = new char[1];
        data[0] = '\0';
    }
    else {
        length = strlen(str);
        data = new char[length + 1];
        strcpy(data, str);
    }
}

void MyString::print() const {
    std::cout << data << "\n";
}

void MyString::read_line() {
    std::string temp;
    std::getline(std::cin, temp);
    set_new_string(temp.c_str());
}
