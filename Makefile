compile:
	gcc -o ./bin/main ./src/main.c -lGL -lm -ldl -lX11 -lpthread -lXrandr -lXext -lraylib

run:
	./bin/main

clean:
	rm -rf ./bin/main

compile-run: compile run

.PHONY: compile run clean compile-run
