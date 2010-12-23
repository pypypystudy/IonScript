# UNIVERSAL MAKEFILE FOR EXECUTABLES LIBRARIES
# Credits: Massimo "Keebus" Tristano <massimo.tristano@gmail.com>
# Licensed under GNU GPL license.

TARGET_NAME := isi

SOURCE_DIR := source
BIN_DIR := .
BUILD_DIR := build

INCLUDES := -I$(SOURCE_DIR) -I../library/source

#Release Configuration. Simply type "make" to compile with this configuration.
CFLAGS := -O3 -Wall
LDFLAGS := -L../library/bin/
LDLIBS := -lIonScript

#Debug Configuration. Type "make debug" to compile with this configuration.
CFLAGS_D := -g -O0 -Wall -DDEBUG
LDFLAGS_D := -L../library/bin/
LDLIBS_D := -lIonScript_d

#######DONT EDIT THIS PART IF YOU DONT KNOW EXACTLY WHAT YOU'RE DOING###########
CC = g++

ifdef WIN32
	TARGET:= $(BIN_DIR)/$(TARGET_NAME).exe
else
	TARGET:= $(BIN_DIR)/$(TARGET_NAME)
endif

ifdef WIN32
	TARGET_D:= $(BIN_DIR)/$(TARGET_NAME)_d.exe
else
	TARGET_D:= $(BIN_DIR)/$(TARGET_NAME)_d
endif

SRC := $(shell find ./$(SOURCE_DIR)/ -name "*.cpp" -print)
OBJS := $(SRC:./$(SOURCE_DIR)/%.cpp=./$(BUILD_DIR)/release/%.o)
OBJS_D := $(SRC:./$(SOURCE_DIR)/%.cpp=./$(BUILD_DIR)/debug/%.o)

SRCDIR := $(shell find ./$(SOURCE_DIR) -type d -exec ls -d {} \;)
DIR :=$(SRCDIR:./$(SOURCE_DIR)%=./$(BUILD_DIR)/release%)
DIR_D := $(SRCDIR:./$(SOURCE_DIR)%=./$(BUILD_DIR)/debug%)

###########ALL##############
all: release
	@echo ">> All done :)"

##########RELEASE############
release: release-info directories $(TARGET)
	@echo ">> Done :)"

release-info:
	@echo ">> Building $(TARGET) with the release configuration..."
	-rm $(TARGET)

directories:
	@mkdir -p $(DIR) $(BIN_DIR) $(BUILD_DIR)
		
$(TARGET) : $(OBJS)
	@echo ">> Making release executable..."
	$(CC) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $(TARGET)

$(OBJS): ./$(BUILD_DIR)/release/%.o : ./$(SOURCE_DIR)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
##########DEBUG############
debug: debug-info directories_d $(TARGET_D)
	@echo ">> Done :)"

debug-info:
	@echo ">> Building $(TARGET_NAME) with the debug configuration..."
	-rm $(TARGET_D)

directories_d:
	@mkdir -p $(DIR_D) $(BIN_DIR) $(BUILD_DIR)
		
$(TARGET_D) : $(OBJS_D)
	@echo ">> Making debug executable..."
	$(CC) $(OBJS_D) $(LDFLAGS_D) $(LDLIBS_D) -o $(TARGET_D)

$(OBJS_D): ./$(BUILD_DIR)/debug/%.o : ./$(SOURCE_DIR)/%.cpp
	$(CC) $(CFLAGS_D) $(INCLUDES) -c $< -o $@

##########EXTRA###########
clean:
	@echo ">> Cleaning..."
	-rm -rf $(TARGET) $(TARGET_D) $(BUILD_DIR) $(BIN_DIR)
	@echo ">> Done :)"

polish:
	@echo ">> Polishing..."
	-rm -rf $(BUILD_DIR)
	@echo ">> Done :)"

package: release
	@echo ">> Making package..."
	-rm -rf $(TARGET_D)
	@mv $(BIN_DIR) $(TARGET_NAME)
	-tar czf $(TARGET_NAME)-$(VERSION).tar.gz $(TARGET_NAME)
	@mv $(TARGET_NAME) $(BIN_DIR)

install: release
	-cp isi /usr/bin
