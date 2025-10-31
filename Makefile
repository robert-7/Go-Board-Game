SHELL := /bin/bash

CC := gcc
TARGET := main
SRCS := main.cpp
LIBS := -lGL -lGLU -lglut -lIL -lILU -lILUT -lstdc++ -lm

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
