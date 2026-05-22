build/debug.out: src/lab2.cpp build/My_string.o build/BaseFile.o build/IFile.o
	g++ -g -o build/debug.out src/lab2.cpp  src/My_string.cpp src/BaseFile.cpp src/IFile.cpp
debug: build/debug.out
	gdb build/debug.out
build/My_string.o: src/My_string.cpp src/My_string.hpp
	g++ -g -c -o build/My_string.o src/My_string.cpp
build/BaseFile.o: src/BaseFile.cpp src/BaseFile.hpp  
	g++ -g -c -o build/BaseFile.o src/BaseFile.cpp
build/IFile.o: src/IFile.cpp src/IFile.hpp
	g++ -g -c -o build/IFile.o src/IFile.cpp