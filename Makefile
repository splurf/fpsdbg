# Compiler config
CC = gcc
SRC = src/*
INCLUDE = include/*
LIBS = -lGL -lGLEW `pkg-config glfw3 --static --libs`
BUILD = $(CC) src/main.c -Iinclude $(LIBS)

# Project name
PROJECT = fpsdbg

# Common directories
TARGET_DIR = target
DEBUG_DIR = $(TARGET_DIR)/debug
RELEASE_DIR = $(TARGET_DIR)/release

debug: $(DEBUG_DIR)/$(PROJECT)		# unoptimized
release: $(RELEASE_DIR)/$(PROJECT)	# optimized

$(DEBUG_DIR)/$(PROJECT): $(SRC) $(INCLUDE)
	@mkdir -p $(DEBUG_DIR)
	$(BUILD) -g -o $@

$(RELEASE_DIR)/$(PROJECT): $(SRC) $(INCLUDE)
	@mkdir -p $(RELEASE_DIR)
	$(BUILD) -o $@ -Ofast

clean:
	@rm -rf $(TARGET_DIR)

fmt: $(SRC)
	@astyle -xjpqnxgHSA14 --squeeze-lines=2 --squeeze-ws $^