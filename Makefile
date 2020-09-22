CC = gcc
SRC_FILES := $(shell find -name "*.c")
OBJ_FILES := $(SRC_FILES:.c=.o)
DEP_FILES := $(SRC_FILES:.c=.d)
INCLUDE = -Iinclude
C_FLAGS = -Wall -O0 -MMD $(INCLUDE)
LD_FLAGS = -lglfw -lGLU -lGL -lGLEW -lXrandr -lXinerama -lXcursor -lXi -lXxf86vm -lX11 -lpthread -lrt -lm -ldl -g

all: chip8

chip8: $(OBJ_FILES)
	$(CC) -o chip8 $(OBJ_FILES) $(LD_FLAGS)

test: chip8
	./chip8 -test

demo: chip8
	./chip8 ./assets/roms/BRIX

.c.o:
	$(CC) $(C_FLAGS) -c $< -o $@ -g

clean:
	rm -f chip8 $(OBJ_FILES) $(DEP_FILES)

-include $(patsubst %.o, %.d, $(OBJ_FILES))
