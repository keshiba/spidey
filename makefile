BIN_DIR			= ./bin
INCLUDE_DIR		= ./include
BUILD_DIR		= ./build
SOURCE_DIR		= ./src
CPP				= g++
LIBS			= -lboost_system
MKDIR_P			= mkdir -p
CLAG			= -Wall
APP_NAME		= spidey
INCLUDE_FLAGS	= -I $(INCLUDE_DIR) \
				  -I $(INCLUDE_DIR)/http \
				  -I $(INCLUDE_DIR)/exceptions \

.PHONY: all clean

all: $(APP_NAME)

$(APP_NAME): $(BUILD_DIR)/main.o 
	$(CPP) $(CFLAG) $(LIBS) -o $(BIN_DIR)/$(APP_NAME) \
		$(BUILD_DIR)/main.o $(BUILD_DIR)/app.o $(BUILD_DIR)/appexception.o \
		$(BUILD_DIR)/http_client.o


$(BUILD_DIR)/main.o: $(BUILD_DIR)/app.o $(SOURCE_DIR)/main.cpp
	$(CPP) $(CFLAG) $(LIBS) $(INCLUDE_FLAGS) -c -o $(BUILD_DIR)/main.o \
		$(SOURCE_DIR)/main.cpp


$(BUILD_DIR)/app.o: $(BUILD_DIR)/http_client.o $(BUILD_DIR)/appexception.o $(SOURCE_DIR)/app.cpp
	$(CPP) $(CFLAG) $(LIBS) $(INCLUDE_FLAGS) -c -o $(BUILD_DIR)/app.o \
		$(SOURCE_DIR)/app.cpp


$(BUILD_DIR)/http_client.o: $(SOURCE_DIR)/http/http_client.cpp $(BUILD_DIR)/appexception.o
	$(CPP) $(CFLAG) $(LIBS) $(INCLUDE_FLAGS) -c -o $(BUILD_DIR)/http_client.o \
		$(SOURCE_DIR)/http/http_client.cpp

	
$(BUILD_DIR)/appexception.o: $(SOURCE_DIR)/exceptions/appexception.cpp
	$(CPP) $(CFLAG) $(LIBS) $(INCLUDE_FLAGS) -c -o $(BUILD_DIR)/appexception.o \
		$(SOURCE_DIR)/exceptions/appexception.cpp

clean:
	rm -rf $(BIN_DIR)/$(APP_NAME) $(BUILD_DIR)/*.o