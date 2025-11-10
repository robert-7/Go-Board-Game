SHELL := /bin/bash

CC := gcc
TARGET := main
SRCS := main.cpp
# `-lstdc++` was added to resolve [this issue](https://stackoverflow.com/questions/33263288/libstdc-dso-missing-from-command-line)
# `-lm` was added to resolve [this issue](https://stackoverflow.com/questions/16006145/ld-undefined-reference-to-symbol-log2glibc-2-2-5)
LIBS := -lGL -lGLU -lglut -lIL -lILU -lILUT -lstdc++ -lm

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
