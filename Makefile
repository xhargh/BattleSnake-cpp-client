
.PHONY: all
all:
	cd build && \
		cmake .. -DCMAKE_BUILD_TYPE=Debug && \
		make

.PHONY: run
run: all
	./build/battlesnake-client

.PHONY: clean
clean:
	rm -rf build/*

