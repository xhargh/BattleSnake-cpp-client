
.PHONY: all
all: build

.PHONY: build
build:
	cd build && \
		cmake .. -DCMAKE_BUILD_TYPE=Debug && \
		make

.PHONY: build_opt
build_opt:
	cd build && \
		cmake .. -DCMAKE_BUILD_TYPE=Optimize && \
		make

.PHONY: run_basic
run_basic:
	make
	./build/basic_snake

.PHONY: run_boost
run_boost:
	make
	./build/boost_snake

.PHONY: clean
clean:
	rm -rf build/*

