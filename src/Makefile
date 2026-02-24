CPP= g++ -g -O0
CFLAGS= 
LFLAGS= -lgmp -lgmpxx
SRC= cli.cpp convert.cpp shunt.cpp token.cpp

run: main
	./$<

main: $(SRC)
	$(CPP) $(CFLAGS) -o $@ $< $(LFLAGS) -DCLI

.PHONY: clean
clean:
	rm -f main
