CC=g++

CFLAGS=-c

all: diseaseAggregator

diseaseAggregator: main.o executive.o pipes.o worker.o
	$(CC) -o diseaseAggregator main.cpp executive.cpp pipes.cpp worker.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

executive.o: executive.cpp
	$(CC) $(CFLAGS) executive.cpp

pipes.o: pipes.cpp
	$(CC) $(CFLAGS) pipes.cpp

worker.o: worker.cpp
	$(CC) $(CFLAGS) worker.cpp

clean:
	$(info Cleaning...)
	rm -rf *o diseaseAggregator
	rm -rf log_file.*
	

