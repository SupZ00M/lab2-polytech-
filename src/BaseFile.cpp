#include "BaseFile.hpp"
#include <iostream>
#include "My_string.hpp"
 
BaseFile::BaseFile() {
    file =  nullptr;
    path =  MyString("");
    mode =  MyString("");
}

BaseFile::BaseFile(char* Path, char* Mode) {
    path =  Path;
    mode = Mode;
    file = fopen(Path, Mode);
    
    if (!file) {
        std::cerr << "Error: Cannot open file " << std::endl;
        path.print();
    }}
BaseFile::BaseFile(FILE* File) {
    file = File;
    path = MyString("");
    mode =  MyString("");
}

BaseFile::~BaseFile() {
    if (file) {
        fclose(file);
        file = nullptr;
        std::cout << "File closed automatically" << std::endl;
    }
}
int BaseFile::is_open() const {
    return file != nullptr;
}

int BaseFile::can_read() const {
    if (!is_open()) return 0;
    return (mode.get(1)=='r' || mode.get(1)=='+' || mode.get(2)=='+');
}
int BaseFile::can_write() const {
    if (!is_open()) return 0;
    return (mode.get(1)=='w' || mode.get(1)=='+'|| mode.get(2)=='+'|| mode.get(1)=='a');
}
size_t BaseFile::write_raw(const void* buf, size_t n) {
    if (!is_open() || !can_write()) return 0;
    
    return fwrite(buf, 1, n, file);
}
size_t BaseFile::read_raw(void* buf, size_t max) {
    if (!is_open() || !can_read()) return 0;
    
    return fread(buf, 1, max, file);
}
size_t BaseFile::write(const void* buf, size_t n) {
    return write_raw(buf, n);
}

size_t BaseFile::read(void* buf, size_t max) {
    return read_raw(buf, max);
}

// Текущая позиция в файле
long BaseFile::tell() const {
    if (!is_open()) return -1;
    
    return ftell(file);
}

// Перемещение позиции в файле
int BaseFile::seek(long offset) {
    if (!is_open()) return 0;
    
    return fseek(file, offset, SEEK_SET) == 0;
}


BaseFile::BaseFile(BaseFile&& other) noexcept 
    : file(other.file), 
      path(std::move(other.path)), 
      mode(std::move(other.mode)) {
    other.file = nullptr;
}


BaseFile& BaseFile::operator=(BaseFile&& other) noexcept {
    if (this != &other) {
        if (file) fclose(file);
        
        file = other.file;
        path = std::move(other.path);
        mode = std::move(other.mode);
        other.file = nullptr;
    }
    return *this;
}