CC         := icc
CFLAGS     := -O3 -qopenmp

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

$(OBJ_DIR)/helper.o: $(SRC_DIR)/helper.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/helper.c -o $(OBJ_DIR)/helper.o

$(OBJ_DIR)/sscal.o: $(SRC_DIR)/sscal.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/sscal.c -o $(OBJ_DIR)/sscal.o

$(BUILD_DIR)/my_sscal: $(OBJ_DIR)/my_sscal.o $(OBJ_DIR)/sscal.o $(OBJ_DIR)/helper.o
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/my_sscal

$(OBJ_DIR)/my_sscal.o: $(SRC_DIR)/drivers/my_sscal.c
	$(CC) $(CFLAGS) -c $^ -o $(OBJ_DIR)/my_sscal.o

$(BUILD_DIR)/benchmark: $(OBJ_DIR)/benchmark.o $(OBJ_DIR)/sscal.o $(OBJ_DIR)/helper.o
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/benchmark

$(OBJ_DIR)/benchmark.o: $(SRC_DIR)/benchmark.c
	$(CC) $(CFLAGS) -c $^ -o $(OBJ_DIR)/benchmark.o

clean:
	rm $(OBJ_DIR)/*.o