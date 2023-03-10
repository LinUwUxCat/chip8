SDL2=-lSDL2main -lSDL2

.PHONY: default run clean

default:
	g++ src/*.cpp -Ilib -Ilib/imgui -Ilib/tinyfiledialogs $(SDL2) -ldl -lconfig -o chip8

run:
	g++ src/*.cpp -Ilib -Ilib/imgui -Ilib/tinyfiledialogs $(SDL2) -ldl -lconfig -o chip8
	./chip8

clean:
	rm ./chip8