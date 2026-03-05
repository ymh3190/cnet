CC = gcc
CFLAGS = -Wall -O2
SRC = cross.c

ifeq ($(OS),Windows_NT)
TARGET = cross.exe
LIBS = -lws2_32
RM = del
else
TARGET = cross
LIBS =
RM = rm -f
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	$(RM) cross cross.exe