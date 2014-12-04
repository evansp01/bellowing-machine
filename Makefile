ARGS = ./src/args/*.c
DNAMPI = ./src/dna_mpi/*.c
POINTSMPI = ./src/points_mpi/*.c
CC = mpicc
FLAGS = -std=c11 -o

.PHONY: clean

all: dna_mpi points_mpi

dna_mpi: $(DNAMPI) $(ARGS)
	$(CC) $(FLAGS) dna_mpi $(DNAMPI) $(ARGS)

points_mpi: $(POINTSMPI) $(ARGS)
	$(CC) $(FLAGS) points_mpi $(POINTSMPI) $(ARGS)

clean:
	rm -rf dna_mpi points_mpi