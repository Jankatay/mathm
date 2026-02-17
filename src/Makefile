CPP= g++ -g -O0
CFLAGS= 
LFLAGS= -lgmp -lgmpxx
SRC= shunt.cpp

run: main
	./$<

main: $(SRC)
	$(CPP) $(CFLAGS) -o $@ $^ $(LFLAGS) -DTEST_SHUNT

.PHONY: clean
clean:
	rm -f main
