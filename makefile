OBJ = evidence.o room.o ghost.o hunter.o building.o main.o

project: $(OBJ)
	gcc -Wall -o project $(OBJ) -lpthread

evidence.o: evidence.c defs.h
	gcc -Wall -c evidence.c -lpthread

ghost.o: ghost.c defs.h
	gcc -Wall -c ghost.c -lpthread

hunter.o: hunter.c defs.h
	gcc -Wall -c hunter.c -lpthread

room.o: room.c defs.h
	gcc -Wall -c room.c -lpthread

building.o: room.c defs.h
	gcc -Wall -c building.c -lpthread

main.o : main.c defs.h
	gcc -Wall -c main.c -lpthread
clean:
	rm -f $(OBJ) project

