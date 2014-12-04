ARGS = ./src/args/*.c
DNAMPI = ./src/dna_mpi/*.c
POINTSMPI = ./src/points_mpi/*.c
CC = mpicc
FLAGS = -std=c11 -o
PYTHON = python3
GEN = ./src/gen

.PHONY: clean

all: dna_mpi points_mpi test

test: dna_test points_test

dna_mpi: $(DNAMPI) $(ARGS)
	$(CC) $(FLAGS) dna_mpi $(DNAMPI) $(ARGS)

points_mpi: $(POINTSMPI) $(ARGS)
	$(CC) $(FLAGS) points_mpi $(POINTSMPI) $(ARGS)

dna_test: dna_large.dat dna_small.dat

dna_large.dat:
	@echo "Generating large dna dataset. This could take some time."
	$(PYTHON) $(GEN)/dna_gen.py -c 6 -l 50 -n 200000 -s 0.9 -f dna_large.dat

dna_small.dat:
	@echo "Generating small dna dataset."
	$(PYTHON) $(GEN)/dna_gen.py -c 6 -l 50 -n 20000 -s 0.9 -f dna_small.dat

points_test: points_large.dat points_small.dat

points_large.dat:
	@echo "Generating large points dataset. This could take some time."
	$(PYTHON) $(GEN)/points_gen.py -c 6 -p 200000 -o points_large.dat -v 50

points_small.dat:
	@echo "Generating small points dataset."
	$(PYTHON) $(GEN)/points_gen.py -c 6 -p 20000 -o points_small.dat -v 50

clean:
	rm -rf dna_mpi points_mpi points_large.dat points_small.dat dna_large.dat dna_small.dat