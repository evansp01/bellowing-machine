ARGS = ./src/args/*.c
DNAMPI = ./src/dna_mpi/*.c
POINTSMPI = ./src/points_mpi/*.c
POINTSSEQ = ./src/points_seq/*.c
DNASEQ = ./src/dna_seq/*.c
CC = mpicc
FLAGS = -std=c11 -o
PYTHON = python3
PYTHON2 = python2
GEN = ./src/gen
GCC = gcc

.PHONY: clean
.PHONY: clean-data

all: dna_mpi points_mpi points_seq dna_seq

test: dna_test points_test

handin:
	git archive -o handin.tar master

dna_mpi: $(DNAMPI) $(ARGS)
	$(CC) $(FLAGS) dna_mpi $(DNAMPI) $(ARGS)

dna_seq: $(DNASEQ) $(ARGS)
	$(GCC) $(FLAGS) dna_seq $(DNASEQ) $(ARGS)

points_mpi: $(POINTSMPI) $(ARGS)
	$(CC) $(FLAGS) points_mpi $(POINTSMPI) $(ARGS)

points_seq: $(POINTSSEQ) $(ARGS)
	$(GCC) $(FLAGS) points_seq $(POINTSSEQ) $(ARGS)

dna_test: dna_large.dat dna_small.dat

dna_large.dat:
	@echo "Generating large dna dataset. This could take some time."
	$(PYTHON) $(GEN)/dna_gen.py -c 2 -l 50 -n 500000 -s 0.9 -f dna_large.dat

dna_small.dat:
	@echo "Generating small dna dataset."
	$(PYTHON) $(GEN)/dna_gen.py -c 2 -l 50 -n 50000 -s 0.9 -f dna_small.dat

points_test: points_large.dat points_small.dat points_huge.dat

points_huge.dat:
	@echo "Generating huge points dataset. This could take some time."
	$(PYTHON2) $(GEN)/points_gen.py -c 2 -p 5000000 -o points_huge.dat -v 50

points_large.dat:
	@echo "Generating large points dataset. This could take some time."
	$(PYTHON2) $(GEN)/points_gen.py -c 2 -p 500000 -o points_large.dat -v 50

points_small.dat:
	@echo "Generating small points dataset."
	$(PYTHON2) $(GEN)/points_gen.py -c 2 -p 50000 -o points_small.dat -v 50

clean-data:
	rm -rf points_large.dat points_small.dat dna_large.dat dna_small.dat points_huge.dat

clean:
	rm -rf dna_mpi points_mpi points_seq dna_seq