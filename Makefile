FLAGS=-g -O0

output: main.o builtin.o
	gcc $(FLAGS) main.o builtin.o -o output

main.o: main.c main.h
	gcc $(FLAGS) -c main.c

builtin.o: builtin.c builtin.h
	gcc $(FLAGS) -c builtin.c

clean:
	rm main.o builtin.o output
