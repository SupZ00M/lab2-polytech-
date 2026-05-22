#ifndef BASEFILE_H
#define BASEFILE_H

#include <vector>
#include <cstdio>
#include "My_string.hpp"
#include "IFile.hpp"

class BaseFile : public IFile {
private:
    FILE* file;
    MyString path;
    MyString mode;  

public:
    BaseFile();
    
    BaseFile(char* Path,char* Mode);
  
    BaseFile(FILE* File);
    
    virtual ~BaseFile();
    
    int is_open() ;
    int can_read() override;
    int can_write() override;
     
    size_t write_raw( const void* buf, size_t n_bytes);
    size_t read_raw(void* buf, size_t max_bytes);
    

   virtual size_t write(const void* buf, size_t n_bytes) override;
   virtual size_t read(void* buf, size_t max_bytes) override;
    
    long tell() override;
    virtual int seek(long offset) override;
    
    BaseFile(const BaseFile&)  = delete;
    BaseFile& operator=(const BaseFile&)  = delete;
    
    BaseFile(BaseFile&& other)  noexcept;
    BaseFile& operator=(BaseFile&& other)  noexcept;
};


class Base32File : public BaseFile {
private:
    char custom_table[32];  // Таблица кодировки
    
public:

    Base32File();
    Base32File( char* path, char* mode);
    Base32File(char* path, char* mode,  char table[32]);

    ~Base32File();
    

    static int encoded32_size(int raw_size);
    static int decoded32_size(int enc_size);
    static int encode32(const char* raw, int size, char* encoded);
    static int decode32(char* encoded, int size, char* decoded);
    

    size_t write( const void* buf, size_t n);
    size_t read(void* buf, size_t max);
};

class RleFile : public BaseFile {
private:
    std::vector<char> write_buffer;   
    std::vector<char> read_buffer;    
    size_t read_pos;                  
    
public:

    RleFile();
    RleFile( char* path, char* mode);
    ~RleFile();
    
   
    size_t write(const void* buf, size_t n) override;
    size_t read(void* buf, size_t max) override;
    int seek(long offset);
    char* decompress(size_t& out_size);
};
void write_int(BaseFile& file, int n);
#endif











