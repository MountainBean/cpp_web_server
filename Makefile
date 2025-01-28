CC = clang++

TARGET = server

SRCS = main.cpp

ARGS = -std=c++20

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(ARGS)

