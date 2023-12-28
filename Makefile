SRC = src/main.c
OUT = bin/fpsdbg
INCLUDE = -Iinclude
LIBS = -lGL -lm -lGLU -lGLEW `pkg-config glfw3 --static --libs`
BUILD_CMD = gcc $(SRC) -o $(OUT) $(INCLUDE) $(LIBS)

debug:
	$(BUILD_CMD) -Wall

release:
	$(BUILD_CMD) -Ofast

clean:
	@rm -f $(OUT)