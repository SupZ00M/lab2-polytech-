#ifndef IFILE_HPP
#define IFILE_HPP

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

#endif