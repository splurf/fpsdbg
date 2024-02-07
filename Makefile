# project name
PROJECT = fpsdbg

# compiler config
CC = gcc
CFLAGS_COMMON = -Wall -pedantic
CFLAGS_DEBUG = -g
CFLAGS_RELEASE = -Ofast

# dependencies
LIBS = -lGL -lGLEW `pkg-config glfw3 --static --libs`

# local directories
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj
BIN_DIR = bin

# system directories
USR_LOC_BIN_DIR = /usr/local/bin

# source code and objects
SRC = $(wildcard $(SRC_DIR)/*)
INC = $(wildcard $(INC_DIR)/*)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))


# build executable for debug
debug: CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_DEBUG)
debug: $(BIN_DIR)/$(PROJECT)

# build executable for release
release: CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_RELEASE)
release: $(BIN_DIR)/$(PROJECT)


# install to /usr/local/bin
install: release
	@cp $(BIN_DIR)/$(PROJECT) $(USR_LOC_BIN_DIR)/$(PROJECT)
	@echo "Successfully installed $(PROJECT)."

# remove from /usr/local/bin, if it exists
uninstall:
ifeq (,$(wildcard $(USR_LOC_BIN_DIR)/$(PROJECT)))
	@echo "$(PROJECT) doesn't exist."
else
	@rm -f $(USR_LOC_BIN_DIR)/$(PROJECT)
	@echo "Successfully removed $(PROJECT)."
endif


# link objects into executable
$(BIN_DIR)/$(PROJECT): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $(OBJ) $(CFLAGS) $(LIBS)

# compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC)
	@mkdir -p $(OBJ_DIR)
	$(CC) -o $@ -c $< -I$(INC_DIR) $(CFLAGS)


# remove all build files and executables
clean:
	@rm -rf $(BIN_DIR) $(OBJ_DIR)


# format the code using `astyle`
fmt: $(SRC) $(INC)
	@astyle -xWxjpqnxgHSA14 --squeeze-lines=2 --squeeze-ws $^


.PHONY: all install uninstall clean fmt