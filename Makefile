SDL3=$(shell pkg-config --cflags --libs sdl3)

.PHONY: default run clean

default:
	g++ src/*.cpp -Ilib -Ilib/imgui -Ilib/tinyfiledialogs $(SDL3) -ldl -lconfig -o chip8

run:
	g++ src/*.cpp -Ilib -Ilib/imgui -Ilib/tinyfiledialogs $(SDL3) -ldl -lconfig -o chip8
	./chip8

clean:
	rm ./chip8