CC = clang++

TARGET = server

SRCS = main.cpp

ARGS = -std=c++20 -pedantic-errors -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(ARGS)

