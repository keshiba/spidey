BIN_DIR		= ./bin
BUILD_DIR	= ./build
SOURCE		= ./src
CPP			= g++
MKDIR_P		= mkdir -p
CLAG		= -Wall
APP_NAME	= spidey

.PHONY: all clean

all: $(APP_NAME)

$(APP_NAME): $(BUILD_DIR)/main.o
	$(CPP) $(CFLAG) -o $(BIN_DIR)/spidey $(BUILD_DIR)/main.o

$(BUILD_DIR)/main.o: $(SOURCE)/main.cpp
	$(CPP) $(CFLAG) -c -o $(BUILD_DIR)/main.o $(SOURCE)/main.cpp

clean:
	rm -rf $(BIN_DIR)/$(APP_NAME) $(BUILD_DIR)/*.o