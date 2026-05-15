#include "BaseFile.hpp"
#include <iostream>
#include <cstring>
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
int BaseFile::is_open() {
    return file != nullptr;
}

int BaseFile::can_read() {
    if (!is_open()) return 0;
    
    // Получаем C-строку для анализа
    const char* mode_str = mode.c_str();  
    
    // Проверяем наличие 'r' в режиме
    for (size_t i = 0; i < mode.get_length(); i++) {
        if (mode.get(i) == 'r') return 1;
        if (mode.get(i) == '+') return 1;  
    }
    
    return 0;
}

int BaseFile::can_write() {
    if (!is_open()) return 0;
    
    const char* mode_str = mode.c_str();
    
   
    for (size_t i = 0; i < mode.get_length(); i++) {
        char ch = mode.get(i);
        if (ch == 'w' || ch == 'a') return 1;
        if (ch == '+') return 1;
    }
    
    return 0;
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

long BaseFile::tell() {
    if (!is_open()) return -1;
    
    return ftell(file);
}

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



static const char DEFAULT_TABLE[32] = {
    'A','B','C','D','E','F','G','H','I','J',
    'K','L','M','N','O','P','Q','R','S','T',
    'U','V','W','X','Y','Z','1','2','3','4','5','6'
};



Base32File::Base32File() : BaseFile() {
    memcpy(custom_table, DEFAULT_TABLE, 32);
}

Base32File::Base32File(const char* path, const char* mode) 
    : BaseFile(const_cast<char*>(path), const_cast<char*>(mode)) {
    memcpy(custom_table, DEFAULT_TABLE, 32);
}

Base32File::Base32File(const char* path, const char* mode, const char table[32]) 
    : BaseFile(const_cast<char*>(path), const_cast<char*>(mode)) {
    if (table) {
        memcpy(custom_table, table, 32);
    } else {
        memcpy(custom_table, DEFAULT_TABLE, 32);
    }
}

Base32File::~Base32File() {
}


int Base32File::encoded32_size(int raw_size) {
    return (raw_size * 8 + 4) / 5;  
}

int Base32File::decoded32_size(int enc_size) {
    return (enc_size * 5) / 8;
}

int Base32File::encode32(const char* raw, int size, char* encoded) {
    if (!raw || size <= 0 || !encoded) {
        return 1;
    }
    
    const char table32[32] = {
        'A','B','C','D','E','F','G','H','I','J',
        'K','L','M','N','O','P','Q','R','S','T',
        'U','V','W','X','Y','Z','1','2','3','4','5','6'
    };
    
    int bit_count = 0;
    int buf = 0;
    int pos = 0;
    int char_size = sizeof(char) * 8;  
    int encode_bits = 5;
    
    for (int i = 0; i < size; i++) {

        buf = (buf << char_size) | (unsigned char)raw[i];
        bit_count += char_size;
        
    
        while (bit_count >= encode_bits) {
            bit_count -= encode_bits;
            // Извлекаем старшие 5 бит
            int index = (buf >> bit_count) & 31;  // 31 = 0x1F = 11111b
            encoded[pos] = table32[index];
            pos++;
        }
    }
    
    // Если остались биты, добавляем последний символ
    if (bit_count > 0) {
        int index = (buf << (encode_bits - bit_count)) & 31;
        encoded[pos] = table32[index];
        pos++;
    }
    
    return 0;
}


int Base32File::decode32(const char* encoded, int size, char* decoded) {
    if (!encoded || size <= 0 || !decoded) {
        return 1;
    }
    
    const char table32[32] = {
        'A','B','C','D','E','F','G','H','I','J',
        'K','L','M','N','O','P','Q','R','S','T',
        'U','V','W','X','Y','Z','1','2','3','4','5','6'
    };
    
    int bit_count = 0;
    int buffer = 0;
    int pos = 0;
    int char_size = sizeof(char) * 8;  // 8 бит
    int encode_bits = 5;
    
    for (int i = 0; i < size; i++) {
        int val = -1;
        for (int j = 0; j < 32; j++) {
            if (encoded[i] == table32[j]) {
                val = j;
                break;
            }
        }
        
    
        if (val == -1) {
            return 2; 
        }
        
    
        buffer = (buffer << encode_bits) | val;
        bit_count += encode_bits;
        

        while (bit_count >= char_size) {
            bit_count -= char_size;
            // Извлекаем старшие 8 бит
            decoded[pos] = (buffer >> bit_count) & 0xFF;
            pos++;
        }
    }
    
    return 0;
}


size_t Base32File::write(const void* buf, size_t n) {
    if (!is_open() || !can_write() || !buf || n == 0) {
        return 0;
    }
    

    int encoded_size = encoded32_size(n);
    char* encoded_buffer = new char[encoded_size];
    

    if (encode32(static_cast<const char*>(buf), n, encoded_buffer) != 0) {
        delete[] encoded_buffer;
        return 0;
    }
    

    size_t written = write_raw(encoded_buffer, encoded_size);
    
    delete[] encoded_buffer;
    
    if (written == 0) return 0;
    if (written == encoded_size) return n;
    

    return (written * n) / encoded_size;
}

size_t Base32File::read(void* buf, size_t max) {
    if (!is_open() || !can_read() || !buf || max == 0) {
        return 0;
    }
    
 
    int encoded_needed = encoded32_size(max);
    

    char* encoded_buffer = new char[encoded_needed + 8];
    
   
    size_t read_encoded = read_raw(encoded_buffer, encoded_needed);
    if (read_encoded == 0) {
        delete[] encoded_buffer;
        return 0;
    }
    

    int decoded_max = decoded32_size(read_encoded);
    char* decoded_buffer = new char[decoded_max + 1];
    
    int result = decode32(encoded_buffer, read_encoded, decoded_buffer);
    if (result != 0) {
        delete[] encoded_buffer;
        delete[] decoded_buffer;
        return 0;
    }
    

    size_t to_copy = (decoded_max < max) ? decoded_max : max;
    memcpy(buf, decoded_buffer, to_copy);
    
    delete[] encoded_buffer;
    delete[] decoded_buffer;
    
    return to_copy;
}