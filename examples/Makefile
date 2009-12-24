all: test

test: test-sthread.cpp
	g++ -o test-sthread test-sthread.cpp -lrt -lpthread

clean:
	rm -rf test-sthread *~
