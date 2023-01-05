
# define the C++ compiler to use
CCXX=g++

# ignore sign compare warning
CFLAGS = -Wall -O3 -Wno-sign-compare -fstack-check -fstack-protector-all
# fast debug



all: main.o csv.o #rmse_metric.o

	$(CCXX) $(CFLAGS) main.o csv.o -o main
	./main

main.o: main.cpp csv.o

	$(CCXX) $(CFLAGS) -c main.cpp

clean:
	rm -f *.o main top_users_and_items
	rm -f *.csv

csv.o: csv.cpp csv.h

	$(CCXX) $(CFLAGS) -c csv.cpp

rmse_metric.o: rmse_metric.cpp rmse_metric.h

	$(CCXX) $(CFLAGS) -c rmse_metric.cpp

top_users_and_items:  csv.o top_users_and_items.cpp
	$(CCXX) $(CFLAGS) top_users_and_items.cpp -c
	$(CCXX) $(CFLAGS) top_users_and_items.o csv.o -o top_users_and_items
	./top_users_and_items