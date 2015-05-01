CC=clang++
CFLAGS=-O3 -Wall -std=c++11

all: unsupervised_pos hmm_test
unsupervised_pos: unsupervised_pos.o hmm.o 
	$(CC) -o $@ unsupervised_pos.o hmm.o
hmm_test: hmm_test.o hmm.o
	$(CC) -o $@ hmm_test.o hmm.o
hmm.o: hmm.cpp
	$(CC) $(CFLAGS) -c $<
unsupervised_pos.o: unsupervised_pos.cpp
	$(CC) $(CFLAGS) -c $<
hmm_test.o: hmm_test.cpp
	$(CC) $(CFLAGS) -c $<
clean:
	rm -f hmm_test unsupervised_pos *.o
