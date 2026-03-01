CPP= g++ -g -O0
CFLAGS= -Igmp -Lgmp -static
LFLAGS= -lgmpxx -lgmp # The order of these two matter believe it or not
SRC= cli.cpp convert.cpp shunt.cpp token.cpp

run: main
	./$<

main: $(SRC)
	$(CPP) $(CFLAGS) -o $@ $< $(LFLAGS) -DCLI

.PHONY: clean
clean:
	rm -f main
