
# define the C++ compiler to use
CCXX=g++


# fast debug
CFLAGS = -Wall -O3 -DDEBUG #-fstack-check -fstack-protector-all


all: main.o csv.o #rmse_metric.o

	$(CCXX) $(CFLAGS) main.o csv.o -o main

main.o: main.cpp csv.o

	$(CCXX) $(CFLAGS) -c main.cpp

clean:
	rm -f *.o main

csv.o: csv.cpp csv.h

	$(CCXX) $(CFLAGS) -c csv.cpp

rmse_metric.o: rmse_metric.cpp rmse_metric.h

	$(CCXX) $(CFLAGS) -c rmse_metric.cpp
