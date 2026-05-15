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
    
    virtual ~BaseFile();
    
    int is_open();
    int can_read();
    int can_write();
     
    size_t write_raw(const void* buf, size_t n_bytes);
    size_t read_raw(void* buf, size_t max_bytes);
    

   virtual size_t write(const void* buf, size_t n_bytes);
   virtual size_t read(void* buf, size_t max_bytes);
    
    long tell();
    int seek(long offset);
    
    BaseFile(const BaseFile&) = delete;
    BaseFile& operator=(const BaseFile&) = delete;
    
    BaseFile(BaseFile&& other) noexcept;
    BaseFile& operator=(BaseFile&& other) noexcept;
};

#include "BaseFile.hpp"
#include "My_string.hpp"

class Base32File : public BaseFile {
private:
    char custom_table[32];  // Таблица кодировки
    
public:

    Base32File();
    Base32File(const char* path, const char* mode);
    Base32File(const char* path, const char* mode, const char table[32]);

    ~Base32File();
    

    static int encoded32_size(int raw_size);
    static int decoded32_size(int enc_size);
    static int encode32(const char* raw, int size, char* encoded);
    static int decode32(const char* encoded, int size, char* decoded);
    

    size_t write(const void* buf, size_t n) override;
    size_t read(void* buf, size_t max) override;
};


#endif











