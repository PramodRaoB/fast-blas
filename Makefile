CC         := icc
CFLAGS     := -O3

BUILD_DIR := build
SRC_DIR := src
OBJ_DIR := $(SRC_DIR)/obj

EXECS :=
OBJS :=

all: dirs $(BUILD_DIR)/helper.o
	echo "Making all"

dirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)

$(BUILD_DIR)/helper.o : helper.c
	$(CC) $(CFLAGS) -c helper.c -o $(OBJ_DIR)/helper.o

clean:
	rm $(OBJ_DIR)/*.o