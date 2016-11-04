BUILD_DIR = ./build
BIN = terrain
HEADERS = $(wildcard src/*.h) src/maths/maths.h
SOURCES = $(wildcard src/*.c) src/maths/maths.c
OBJS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:%.o=%.d)
CFLAGS = -Iobjects -Isrc -Isrc/maths -lGL -lGLEW -lglfw -lm -lassimp 

all: $(BUILD_DIR) $(BIN)

$(BUILD_DIR):
	mkdir ${BUILD_DIR}
	mkdir ${BUILD_DIR}/src
	mkdir ${BUILD_DIR}/src/maths

$(BUILD_DIR)/%.o: %.c
	gcc $< $(CFLAGS) -c -MMD -o $@

$(BIN): $(OBJS)
	gcc -o $@ $(OBJS) $(CFLAGS)

-include $(DEPS)

clean:
	rm -f terrain
	rm -rf build
