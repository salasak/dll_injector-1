CC = g++
INCLUDE_PATH=include
SRC_PATH=src
OBJ_PATH=_obj
CFLAGS = -I$(INCLUDE_PATH) -g -O0 -Wall -std=c++14
LDFLAGS = -L.
TARGET = dll_injector.exe

OBJECTS = $(patsubst $(SRC_PATH)/%.cpp, $(OBJ_PATH)/%.o, \
          $(wildcard $(SRC_PATH)/*.cpp))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp
	$(CC) -c $^ -o $@ $(CFLAGS)

.PHONY: clean

clean:
	del $(TARGET) $(OBJ_PATH)\*.o

#CC=g++
#CFLAGS=-I. -g -O0 -Wall -std=c++11
#LFLAGS=-L.
#LIBS=
#BIN=bin

#rule: src/main.cpp
#	$(CC) -c src/main.cpp -o obj/main.o $(CFLAGS)
#	$(CC) -o $(BIN) obj/main.o $(LFLAGS)
