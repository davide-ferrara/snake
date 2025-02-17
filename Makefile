build:
	gcc main.c -lSDL2 -o snake

run:
	./snake

clean:
	rm -f snake
