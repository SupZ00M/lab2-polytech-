#include "BaseFile.hpp"
#include "IFile.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>

// Генерация случайных данных
class RandomDataGenerator {
private:
    std::vector<unsigned char> data;
    
public:
    RandomDataGenerator(size_t size) {
        data.resize(size);
        for (size_t i = 0; i < size; i++) {
            data[i] = rand() % 256;  // Случайные байты от 0 до 255
        }
    }
    
    const unsigned char* getData() const { return data.data(); }
    size_t getSize() const { return data.size(); }
    
    bool compare(const void* buf, size_t size) const {
        if (size != data.size()) return false;
        return memcmp(data.data(), buf, size) == 0;
    }
    
    void printFirstBytes(size_t n = 20) const {
        std::cout << "First " << n << " bytes: ";
        for (size_t i = 0; i < n && i < data.size(); i++) {
            printf("%02X ", data[i]);
        }
        std::cout << std::endl;
    }
};

// Базовый тест для любого IFile
bool test_file(IFile& file, const RandomDataGenerator& generator, const char* name) {
    const size_t BUFFER_SIZE = 256;
    const unsigned char* original_data = generator.getData();
    size_t data_size = generator.getSize();
    
    std::cout << "\n=== Testing " << name << " ===" << std::endl;
    std::cout << "Data size: " << data_size << " bytes" << std::endl;
    generator.printFirstBytes();
    
    // Запись данных маленькими буферами
    std::cout << "Writing data in " << BUFFER_SIZE << " byte chunks..." << std::endl;
    size_t offset = 0;
    while (offset < data_size) {
        size_t chunk_size = std::min(BUFFER_SIZE, data_size - offset);
        size_t written = file.write(original_data + offset, chunk_size);
        
        if (written != chunk_size) {
            std::cerr << "ERROR: Write failed at offset " << offset 
                      << " (wrote " << written << " of " << chunk_size << ")" << std::endl;
            return false;
        }
        offset += written;
    }
    
    // Перемещаемся в начало файла
    if (file.seek(0) != 0) {
        std::cerr << "ERROR: Seek to beginning failed!" << std::endl;
        return false;
    }
    
    // Чтение данных маленькими буферами
    std::cout << "Reading data in " << BUFFER_SIZE << " byte chunks..." << std::endl;
    std::vector<unsigned char> read_data;
    read_data.reserve(data_size);
    
    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = file.read(buffer, BUFFER_SIZE)) > 0) {
        read_data.insert(read_data.end(), buffer, buffer + bytes_read);
    }
    
    // Сравнение данных
    std::cout << "Comparing data... ";
    if (read_data.size() != data_size) {
        std::cout << "FAILED!" << std::endl;
        std::cerr << "ERROR: Size mismatch (expected " << data_size 
                  << ", got " << read_data.size() << ")" << std::endl;
        return false;
    }
    
    if (memcmp(original_data, read_data.data(), data_size) == 0) {
        std::cout << "PASSED!" << std::endl;
        return true;
    } else {
        std::cout << "FAILED!" << std::endl;
        std::cerr << "ERROR: Data mismatch!" << std::endl;
        
        // Показываем различия
        for (size_t i = 0; i < std::min(data_size, (size_t)50); i++) {
            if (original_data[i] != read_data[i]) {
                std::cerr << "First difference at offset " << i 
                          << ": expected 0x" << std::hex << (int)original_data[i]
                          << ", got 0x" << (int)read_data[i] << std::dec << std::endl;
                break;
            }
        }
        return false;
    }
}

// Тест для BaseFile
bool test_base_file(const RandomDataGenerator& generator,  char* filename) {
    BaseFile file(filename, "w+");
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open " << filename << std::endl;
        return false;
    }
    return test_file(file, generator, "BaseFile");
}

// Тест для Base32File
bool test_base32_file(const RandomDataGenerator& generator, char* filename) {
    Base32File file(filename, "w+");
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open " << filename << std::endl;
        return false;
    }
    return test_file(file, generator, "Base32File");
}

// Тест для RleFile
bool test_rle_file(const RandomDataGenerator& generator, char* filename) {
    RleFile file(filename, "w+");
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open " << filename << std::endl;
        return false;
    }
    return test_file(file, generator, "RleFile");
}

// Тест для Base32File2 (композиция)
bool test_base32_file2(const RandomDataGenerator& generator,  char* filename) {
    Base32File2 file(new BaseFile(filename, "w+"));
    if (!file.can_write()) {
        std::cerr << "ERROR: Cannot open " << filename << std::endl;
        return false;
    }
    return test_file(file, generator, "Base32File2 (composition)");
}

// Тест для RleFile2 (композиция)
bool test_rle_file2(const RandomDataGenerator& generator, char* filename) {
    RleFile2 file(new BaseFile(filename, "w+"));
    if (!file.can_write()) {
        std::cerr << "ERROR: Cannot open " << filename << std::endl;
        return false;
    }
    return test_file(file, generator, "RleFile2 (composition)");
}

// Тест для композиции: Base32 -> RLE -> File
bool test_composition_b32_rle(const RandomDataGenerator& generator, char* filename) {
    // Цепочка: Base32File2 -> RleFile2 -> BaseFile
    Base32File2 file(new RleFile2(new BaseFile(filename, "w+")));
    if (!file.can_write()) {
        std::cerr << "ERROR: Cannot open " << filename << std::endl;
        return false;
    }
    return test_file(file, generator, "Composition: Base32 -> RLE -> File");
}

// Тест для композиции: RLE -> Base32 -> File
bool test_composition_rle_b32(const RandomDataGenerator& generator, char* filename) {
    // Цепочка: RleFile2 -> Base32File2 -> BaseFile
    RleFile2 file(new Base32File2(new BaseFile(filename, "w+")));
    if (!file.can_write()) {
        std::cerr << "ERROR: Cannot open " << filename << std::endl;
        return false;
    }
    return test_file(file, generator, "Composition: RLE -> Base32 -> File");
}

// Очистка временных файлов
void cleanup_files() {
    const char* files[] = {
        "test_base.bin",
        "test_base32.bin",
        "test_rle.bin",
        "test_base32_2.bin",
        "test_rle_2.bin",
        "test_b32_rle.bin",
        "test_rle_b32.bin"
    };
    
    for (const char* filename : files) {
        remove(filename);
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    CLASS TESTING SUITE" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Инициализация генератора случайных чисел
    srand(static_cast<unsigned>(time(nullptr)));
    
    // Генерация большого массива случайных данных (50 Кб)
    const size_t DATA_SIZE = 50 * 1024;  // 50 Кб
    std::cout << "\nGenerating " << DATA_SIZE << " bytes of random data..." << std::endl;
    RandomDataGenerator generator(DATA_SIZE);
    
    int passed = 0;
    int total = 0;
    
    // Тест 1: BaseFile
    total++;
    if (test_base_file(generator, "test_base.bin")) passed++;
    
    // Тест 2: Base32File
    total++;
    if (test_base32_file(generator, "test_base32.bin")) passed++;
    
    // Тест 3: RleFile
    total++;
    if (test_rle_file(generator, "test_rle.bin")) passed++;
    
    // Тест 4: Base32File2 (композиция)
    total++;
    if (test_base32_file2(generator, "test_base32_2.bin")) passed++;
    
    // Тест 5: RleFile2 (композиция)
    total++;
    if (test_rle_file2(generator, "test_rle_2.bin")) passed++;
    
    // Тест 6: Композиция Base32 -> RLE -> File
    total++;
    if (test_composition_b32_rle(generator, "test_b32_rle.bin")) passed++;
    
    // Тест 7: Композиция RLE -> Base32 -> File
    total++;
    if (test_composition_rle_b32(generator, "test_rle_b32.bin")) passed++;
    
    // Итоги
    std::cout << "\n========================================" << std::endl;
    std::cout << "           RESULTS" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Passed: " << passed << " / " << total << std::endl;
    
    if (passed == total) {
        std::cout << "\nALL TESTS PASSED!" << std::endl;
    } else {
        std::cout << "\nSOME TESTS FAILED!" << std::endl;
    }
    
    // Очистка
    cleanup_files();
    
    return (passed == total) ? 0 : 1;
}