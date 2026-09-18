#include "BaseFile.hpp"
#include <iostream>
#include <cstring>
#include "My_string.hpp"

 
BaseFile::BaseFile() {
    file =  nullptr;
    path =  MyString("");
    mode =  MyString("");
    std::cout << "Base File constructor" << std::endl;
}

BaseFile::BaseFile(char* Path, char* Mode) {
    path =  Path;
    mode = Mode;
    file = fopen(Path, Mode);
    
    if (!file) {
        std::cerr << "Error: Cannot open file " << std::endl;
        path.print();
    }
    else {std::cout << "Base File constructor" << std::endl;}}
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
    for (size_t i = 0; i < (size_t)mode.get_length(); i++) {
        char c = mode.get(i);
        if (c == 'r' || c == '+') return 1;
    }
    return 0;
}

int BaseFile::can_write() {
    if (!is_open()) return 0;
    for (size_t i = 0; i < (size_t)mode.get_length(); i++) {
        char c = mode.get(i);
        if (c == 'w' || c == 'a' || c == '+') return 1;
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
    if (!is_open()) return -1;
    return fseek(file, offset, SEEK_SET) == 0 ? 0 : -1;
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



static  char DEFAULT_TABLE[32] = {
    'A','B','C','D','E','F','G','H','I','J',
    'K','L','M','N','O','P','Q','R','S','T',
    'U','V','W','X','Y','Z','1','2','3','4','5','6'
};



Base32File::Base32File() : BaseFile() {
    memcpy(custom_table, DEFAULT_TABLE, 32);
    std::cout << "Base32 File constructor" << std::endl;
}

Base32File::Base32File( char* path, char* mode) 
    : BaseFile(path,mode) {
    memcpy(custom_table, DEFAULT_TABLE, 32);
    std::cout << "Base32 File constructor" << std::endl;
}

Base32File::Base32File( char* path,char* mode, char table[32]) 
    : BaseFile((path), (mode)) {
    if (table) {
        memcpy(custom_table, table, 32);
    } else {
        memcpy(custom_table, DEFAULT_TABLE, 32);
    }
    std::cout << "Base32 File constructor" << std::endl;
}

Base32File::~Base32File() {
    std::cout << "File32 closed automatically" << std::endl;
}


int Base32File::encoded32_size(int raw_size) {
    return (raw_size * 8 + 4) / 5;  
}

int Base32File::decoded32_size(int enc_size) {
    return (enc_size * 5) / 8;
}

int Base32File::encode32( const char* raw, int size, char* encoded) {
    if (!raw || size <= 0 || !encoded) {
        return 1;
    }
    
   char table32[32] = {
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
            int index = (buf >> bit_count) & 31; 
            encoded[pos] = table32[index];
            pos++;
        }
    }
    
    if (bit_count > 0) {
        int index = (buf << (encode_bits - bit_count)) & 31;
        encoded[pos] = table32[index];
        pos++;
    }
    
    return 0;
}


int Base32File::decode32( char* encoded, int size, char* decoded) {
    if (!encoded || size <= 0 || !decoded) {
        return 1;
    }
    
     char table32[32] = {
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

RleFile::RleFile() :  BaseFile(), read_pos(0) {
    std::cout << "RLE File constructor" << std::endl;}

RleFile::RleFile( char* path,  char* mode) 
    : BaseFile((path),mode), read_pos(0) {std::cout << "RLE File constructor" << std::endl;}

RleFile::~RleFile() {std::cout << "RLE File closed automatically" << std::endl;}

size_t RleFile::write(const void* buf, size_t n) {
    if (!is_open() || !can_write() || !buf || n == 0) return 0;

    const char* data = static_cast<const char*>(buf);
    write_buffer.insert(write_buffer.end(), data, data + n);

    std::vector<char> compressed;
    for (size_t i = 0; i < write_buffer.size(); i++) {
        char current = write_buffer[i];
        size_t count = 1;
        while (i + count < write_buffer.size() &&
               write_buffer[i + count] == current && count < 255) {
            count++;
        }
        compressed.push_back(static_cast<char>(count));
        compressed.push_back(current);
        i += count - 1;
    }

    size_t written = write_raw(compressed.data(), compressed.size());
    if (written == compressed.size()) {
        write_buffer.clear();
        return n;                       // ← исходные байты
    }
    return (written * n) / compressed.size();
}

size_t RleFile::read(void* buf, size_t max) {
    if (!is_open() || !can_read() || !buf || max == 0) return 0;

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
        if (read_raw(&count, 1) != 1) break;
        if (read_raw(&value, 1) != 1) break;

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

int RleFile::seek(long offset) {
    read_buffer.clear();
    read_pos = 0;
    return BaseFile::seek(offset);
}

char* RleFile::decompress(size_t& out_size) {
    if (!is_open() || !can_read()) {
        out_size = 0;
        return nullptr;
    }
    
    seek(0);

    std::vector<char> decompressed;
    char single_char;
    size_t bytes_read;
    
    while ((bytes_read = read(&single_char, 1)) > 0) {
        decompressed.push_back(single_char);
    }
    
    out_size = decompressed.size();
    char* result = new char[out_size];
    for (size_t i = 0; i < out_size; i++) {
        result[i] = decompressed[i];
    }
    
    return result;
}

void write_int(BaseFile& file, int n) {
    
    if (n < 0) {
        file.write("-", 1);
        n = -n;
    }
    
    if (n == 0) {
        file.write("0", 1);
        return;
    }
    

    int divisor = 1;
    int temp = n;
    while (temp > 9) {
        divisor *= 10;
        temp /= 10;
    }
    

    while (divisor > 0) {
        int digit = n / divisor;
        char c = '0' + digit;
        file.write(&c, 1);
        n %= divisor;
        divisor /= 10;
    }
}