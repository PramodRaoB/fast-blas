CC         := gcc
#CFLAGS     := -fopenmp -Ofast -fopenmp-simd -funsafe-math-optimizations -mavx2
CFLAGS     := -fopenmp -O3 -fopenmp-simd -mavx2 -falign-loops=32 -g
#CFLAGS     := -fopenmp -O3 -simd -mavx2
#CFLAGS     := -fopenmp -O3
#CFLAGS     := -fopenmp -Ofast -funsafe-math-optimizations

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

$(OBJ_DIR)/cblas.o: $(SRC_DIR)/cblas.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/cblas.c -o $(OBJ_DIR)/cblas.o

$(BUILD_DIR)/my_sscal: $(OBJ_DIR)/my_sscal.o $(OBJ_DIR)/cblas.o $(OBJ_DIR)/helper.o
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/my_sscal

$(BUILD_DIR)/my_sgemv: $(OBJ_DIR)/my_sgemv.o $(OBJ_DIR)/cblas.o $(OBJ_DIR)/helper.o
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/my_sgemv

$(OBJ_DIR)/my_sscal.o: $(SRC_DIR)/drivers/my_sscal.c
	$(CC) $(CFLAGS) -c $^ -o $(OBJ_DIR)/my_sscal.o

$(OBJ_DIR)/my_sgemv.o: $(SRC_DIR)/drivers/my_sgemv.c
	$(CC) $(CFLAGS) -c $^ -o $(OBJ_DIR)/my_sgemv.o

$(BUILD_DIR)/benchmark: $(OBJ_DIR)/benchmark.o $(OBJ_DIR)/cblas.o $(OBJ_DIR)/helper.o
	$(CC) $(CFLAGS) $^ /usr/local/lib/libblis.a -L/usr/lib -lm -lpthread -o $(BUILD_DIR)/benchmark

$(OBJ_DIR)/benchmark.o: $(SRC_DIR)/benchmark.c
	$(CC) $(CFLAGS) -c $^ -o $(OBJ_DIR)/benchmark.o

clean:
	rm $(OBJ_DIR)/*.o