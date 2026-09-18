#ifndef IFILE_HPP
#define IFILE_HPP
#include <vector>
#include <cstddef>

class IFile {
public:
    virtual int can_read() = 0;
    virtual int can_write() = 0;
    virtual size_t write(const void* buf, size_t n) = 0;
    virtual size_t read(void* buf, size_t max) = 0;
    virtual long tell() = 0;
    virtual int seek(long offset) = 0;
    
     virtual ~IFile(); 
    
};
class Base32File2 : public IFile {
private:
    IFile* m_file;  
    char custom_table[32];
    static const char DEFAULT_TABLE[32];
    
  
    int encoded32_size(int raw_size);
    int decoded32_size(int enc_size);
    int encode32(const char* raw, int size, char* encoded);
    int decode32(const char* encoded, int size, char* decoded);
    
    unsigned int bit_buffer = 0;   
    int bit_count = 0;             
    bool eof = false;  
     std::vector<char> pending;
public:
   
    Base32File2(IFile* file);
    Base32File2(IFile* file, const char table[32]);
    
  
    ~Base32File2();
    
    int can_read() override;
    int can_write() override;
    size_t write(const void* buf, size_t n) override;
    size_t read(void* buf, size_t max) override;
    long tell()  override;
    int seek(long offset) override;
};


class RleFile2 : public IFile {
private:
    IFile* m_file;  
    std::vector<char> write_buffer;
    std::vector<char> read_buffer;
    size_t read_pos;

    std::vector<char> compress(const char* data, size_t size);
    std::vector<char> decompress(const char* data, size_t size);
    
public:

    RleFile2(IFile* file);
    
  
    ~RleFile2();
    
   
    int can_read()  override;
    int can_write() override;
    size_t write(const void* buf, size_t n) override;
    size_t read(void* buf, size_t max) override;
    long tell() override;
    int seek(long offset) override;
};
#endif