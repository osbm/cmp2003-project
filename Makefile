
# define the C++ compiler to use
CCXX=g++


# fast debug
CFLAGS = -Wall -O3 -DDEBUG #-fstack-check -fstack-protector-all

all:  build run

build: 
	$(CCXX) $(CFLAGS) main.cpp -o main

run: build
	./main

clean:
	rm -f *.o main
