OBJ = icpc.o

icpc: $(OBJ)
	cc -o icpc $(OBJ)

icpc.o: icpc.c



.PHONY:
	clean
	install
	uninstall

clean:
	-rm *.o icpc

install:
	-cp icpc /usr/bin

uninstall:
	-rm /usr/bin/icpc
