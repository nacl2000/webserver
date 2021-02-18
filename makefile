CC = g++
CPPFLAG = -Wall -g -std=c++11
TARGET = $(patsubst %.cpp, %, $(wildcard *.cpp))
HEADER = $(wildcard headers/*.h)
% : %.cpp $(HEADER)
	$(CC) $(CPPFLAG) $< -o $@
.PHONY : all clean
all : $(TARGET)
clean : 
	rm -f *.out
