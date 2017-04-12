
.PHONY: all
all:
	cd build && \
		cmake .. -DCMAKE_BUILD_TYPE=Debug && \
		make

.PHONY: run_basic
run_basic: all
	./build/basic_snake

.PHONY: run_boost
run_boost: all
	./build/boost_snake

.PHONY: clean
clean:
	rm -rf build/*

