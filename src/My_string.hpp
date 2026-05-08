#ifndef MY_STRING_HPP
#define MY_STRING_HPP

#include <iostream>

class MyString {
private:
    char* data;
    int length;

public:
    MyString();
    MyString(const char* str);
    MyString(const MyString& other);
    ~MyString();

    char get(int i) const;
    void set(int i, char c);
    void set_new_string(const char* str);
    void print() const;
    void read_line();

    int get_length() const { return length; }
};

#endif