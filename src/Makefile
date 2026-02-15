CPP= g++ -g
CFLAGS= 
LFLAGS= -lgmp -lgmpxx
SRC= parser.cpp

run: main
	./$<

main: $(SRC)
	$(CPP) $(CFLAGS) -o $@ $^ $(LFLAGS) -DTEST_PARSER

.PHONY: clean
clean:
	rm -f main
