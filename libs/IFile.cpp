#include "IFile.hpp"
#include <cstring>
#include <iostream>
#include <vector>

IFile::~IFile() = default;

const char Base32File2::DEFAULT_TABLE[32] = {
    'A','B','C','D','E','F','G','H','I','J',
    'K','L','M','N','O','P','Q','R','S','T',
    'U','V','W','X','Y','Z','1','2','3','4','5','6'
};


Base32File2::Base32File2(IFile* file) : m_file(file), bit_buffer(0), bit_count(0), eof(false) {
    memcpy(custom_table, DEFAULT_TABLE, 32);
}

Base32File2::Base32File2(IFile* file, const char table[32]) : m_file(file), bit_buffer(0), bit_count(0), eof(false)  {
    if (table) {
        memcpy(custom_table, table, 32);
    } else {
        memcpy(custom_table, DEFAULT_TABLE, 32);
    }
}

Base32File2::~Base32File2() {
    if (!pending.empty() && m_file && m_file->can_write()) {
        int encoded_size = encoded32_size((int)pending.size());
        std::vector<char> encoded(encoded_size);
        if (encode32(pending.data(), (int)pending.size(), encoded.data()) == 0) {
            m_file->write(encoded.data(), encoded.size());
        }
        pending.clear();
    }
    delete m_file;
}


int Base32File2::can_read() {
    return m_file && m_file->can_read();
}

int Base32File2::can_write() {
    return m_file && m_file->can_write();
}

int Base32File2::encoded32_size(int raw_size) {
    return (raw_size * 8 + 4) / 5;
}

int Base32File2::decoded32_size(int enc_size) {
    return (enc_size * 5) / 8;
}

int Base32File2::encode32(const char* raw, int size, char* encoded) {
    if (!raw || size <= 0 || !encoded) return 1;
    
    int bit_count = 0;
    int buf = 0;
    int pos = 0;
    
    for (int i = 0; i < size; i++) {
        buf = (buf << 8) | (unsigned char)raw[i];
        bit_count += 8;
        
        while (bit_count >= 5) {
            bit_count -= 5;
            int index = (buf >> bit_count) & 31;
            encoded[pos++] = custom_table[index];
        }
    }
    
    if (bit_count > 0) {
        int index = (buf << (5 - bit_count)) & 31;
        encoded[pos++] = custom_table[index];
    }
    
    return 0;
}

int Base32File2::decode32(const char* encoded, int size, char* decoded) {
    if (!encoded || size <= 0 || !decoded) return 1;
    
    int bit_count = 0;
    int buffer = 0;
    int pos = 0;
    
    for (int i = 0; i < size; i++) {
        int val = -1;
        for (int j = 0; j < 32; j++) {
            if (encoded[i] == custom_table[j]) {
                val = j;
                break;
            }
        }
        
        if (val == -1) return 2;
        
        buffer = (buffer << 5) | val;
        bit_count += 5;
        
        while (bit_count >= 8) {
            bit_count -= 8;
            decoded[pos++] = (buffer >> bit_count) & 0xFF;
        }
    }
    
    return 0;
}

size_t Base32File2::write(const void* buf, size_t n) {
    if (!can_write() || !buf || n == 0) return 0;

    const char* data = static_cast<const char*>(buf);
    pending.insert(pending.end(), data, data + n);
    return n;                   
}

size_t Base32File2::read(void* buf, size_t max) {
    if (!can_read() || !buf || max == 0) return 0;

    char* output = static_cast<char*>(buf);
    size_t output_pos = 0;

    while (output_pos < max) {
        if (bit_count >= 8) {
            bit_count -= 8;
            output[output_pos++] = static_cast<char>((bit_buffer >> bit_count) & 0xFF);
            continue;
        }
        if (eof) break;

        char c;
        if (m_file->read(&c, 1) != 1) { eof = true; break; }

        int val = -1;
        for (int j = 0; j < 32; j++) {
            if (c == custom_table[j]) { val = j; break; }
        }
        if (val == -1) { eof = true; break; }

        bit_buffer = (bit_buffer << 5) | static_cast<unsigned int>(val);
        bit_count += 5;
    }

    return output_pos;
}


long Base32File2::tell() {
    return m_file ? m_file->tell() : -1;
}

int Base32File2::seek(long offset) {
    if (!pending.empty() && m_file && m_file->can_write()) {
        int encoded_size = encoded32_size((int)pending.size());
        std::vector<char> encoded(encoded_size);
        if (encode32(pending.data(), (int)pending.size(), encoded.data()) == 0) {
            m_file->write(encoded.data(), encoded.size());
        }
        pending.clear();
    }

    bit_buffer = 0;
    bit_count  = 0;
    eof        = false;
    return m_file ? m_file->seek(offset) : -1;
}

RleFile2::RleFile2(IFile* file) : m_file(file), read_pos(0) {}

RleFile2::~RleFile2() {

    if (!write_buffer.empty() && m_file && m_file->can_write()) {
        std::vector<char> compressed = compress(write_buffer.data(), write_buffer.size());
        if (!compressed.empty()) {
            m_file->write(compressed.data(), compressed.size());
        }
        write_buffer.clear();
    }
    delete m_file; 
}

int RleFile2::can_read() {
    return m_file && m_file->can_read();
}

int RleFile2::can_write() {
    return m_file && m_file->can_write();
}

std::vector<char> RleFile2::compress(const char* data, size_t size) {
    std::vector<char> result;
    
    for (size_t i = 0; i < size; i++) {
        char current = data[i];
        size_t count = 1;
        
        while (i + count < size && data[i + count] == current && count < 255) {
            count++;
        }
        
        result.push_back(static_cast<char>(count));
        result.push_back(current);
        i += count - 1;
    }
    
    return result;
}

std::vector<char> RleFile2::decompress(const char* data, size_t size) {
    std::vector<char> result;
    
    for (size_t i = 0; i + 1 < size; i += 2) {
        unsigned char count = static_cast<unsigned char>(data[i]);
        char value = data[i + 1];
        
        for (unsigned char j = 0; j < count; j++) {
            result.push_back(value);
        }
    }
    
    return result;
}

size_t RleFile2::write(const void* buf, size_t n) {
    if (!can_write() || !buf || n == 0) return 0;
    
    const char* data = static_cast<const char*>(buf);
    write_buffer.insert(write_buffer.end(), data, data + n);
    
    std::vector<char> compressed = compress(write_buffer.data(), write_buffer.size());
    
    size_t written = m_file->write(compressed.data(), compressed.size());
    
    if (written == compressed.size()) {
        write_buffer.clear();
        return n;
    }
    
    return 0;
}

size_t RleFile2::read(void* buf, size_t max) {
    if (!can_read() || !buf || max == 0) return 0;

    char* output = static_cast<char*>(buf);
    size_t output_pos = 0;

    while (read_pos < read_buffer.size() && output_pos < max) {
        output[output_pos++] = read_buffer[read_pos++];
    }
    if (output_pos >= max) return output_pos;

    read_buffer.clear();
    read_pos = 0;

    while (output_pos < max) {
        unsigned char count;
        char value;
        if (m_file->read(&count, 1) != 1) break;
        if (m_file->read(&value, 1) != 1) break;

        for (unsigned char i = 0; i < count; i++) {
            read_buffer.push_back(value);
        }

        while (read_pos < read_buffer.size() && output_pos < max) {
            output[output_pos++] = read_buffer[read_pos++];
        }

        if (read_pos >= read_buffer.size()) {
            read_buffer.clear();
            read_pos = 0;
        }
    }

    return output_pos;
}
long RleFile2::tell() {
    return m_file ? m_file->tell() : -1;
}

int RleFile2::seek(long offset) {
    read_buffer.clear();
    read_pos = 0;
    write_buffer.clear();
    return m_file ? m_file->seek(offset) : 0;
}