NVCC  ?= nvcc
ARCH  ?= -arch=all-major
FLAGS := -std=c++17 $(ARCH) -Xcompiler=-Wall,-Wextra

TARGET  := drone_swarm
SOURCES := $(wildcard src/*.cu src/*.cpp)

.PHONY: release debug clean

release: NVCCFLAGS := -O3 -lineinfo
debug:   NVCCFLAGS := -O0 -g -G -DDEBUG

release debug:
	$(NVCC) $(FLAGS) $(NVCCFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
