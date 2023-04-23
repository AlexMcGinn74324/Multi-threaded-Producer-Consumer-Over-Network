all: main prod

main: main.o utilityFuncs.o queue.o
	gcc -o main main.o utilityFuncs.o queue.o -Wall -Werror -lpthread

main.o: main.c
	gcc -c main.c -Wall -Werror -lpthread

utilityFuncs.o: utilityFuncs.c
	gcc -c utilityFuncs.c -Wall -Werror -lpthread

queue.o: queue.c
	gcc -c queue.c -Wall -Werror -lpthread

prod: prod.o
	gcc -o prod prod.o -Wall -Werror -lpthread
prod.o:
	gcc -c prod.c -Wall -Werror -lpthread

clean:
	rm -rf *.o out.txt prod main