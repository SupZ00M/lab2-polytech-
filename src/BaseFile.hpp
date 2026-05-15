#ifndef BASEFILE_H
#define BASEFILE_H

#include <cstdio>
#include "My_string.hpp"

class BaseFile {
private:
    FILE* file;
    MyString path;
    MyString mode;  

public:
    BaseFile();
    
    BaseFile(char* Path,char* Mode);
  
    BaseFile(FILE* File);
    
    ~BaseFile();
    
    int is_open() const;
    int can_read() const;
    int can_write() const;
     
    size_t write_raw(const void* buf, size_t n_bytes);
    size_t read_raw(void* buf, size_t max_bytes);
    

    size_t write(const void* buf, size_t n_bytes);
    size_t read(void* buf, size_t max_bytes);
    
    long tell() const;
    int seek(long offset);
    
    BaseFile(const BaseFile&) = delete;
    BaseFile& operator=(const BaseFile&) = delete;
    
    // Разрешаем перемещение
    BaseFile(BaseFile&& other) noexcept;
    BaseFile& operator=(BaseFile&& other) noexcept;
};

#endif // BASEFILE_H