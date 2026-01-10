BUILD ?= "build"

all: build

.PHONY: build
build:
	mkdir -p $(BUILD)
	cd $(BUILD); cmake ..
	cd $(BUILD); $(MAKE)

.PHONY: clean
clean:
	rm -rf $(BUILD)

.PHONY: run
run:
	cd $(BUILD); ./companion_server /dev/ttyUSB1 115200

.PHONY: format
format:
	find meshcore/ -iname '*.h' -o -iname '*.c' -o -iname '*.cpp' | xargs clang-format -i
	echo "main.c" | xargs clang-format -i
