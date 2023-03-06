SDL2=-lSDL2main -lSDL2

.PHONY: default run clean

default:
	g++ src/*.cpp -Ilib -Ilib/imgui $(SDL2) -ldl -o chip8

run:
	g++ src/*.cpp -Ilib -Ilib/imgui $(SDL2) -ldl -o chip8
	./chip8

clean:
	rm ./chip8