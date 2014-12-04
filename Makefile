ARGS = ./src/args/*.c
DNAMPI = ./src/dna_mpi/*.c
POINTSMPI = ./src/points_mpi/*.c
CC = mpicc
FLAGS = -std=c11 -o
PYTHON = python3
GEN = ./src/gen
NUMPY = ./src/numpy
mkfile_path = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SETPATH = export PATH=${PATH}:$(mkfile_path)/src/numpy

.PHONY: clean

all: dna_mpi points_mpi test

src/numpy:
	mkdir ./src/numpy
	tar -xzvf ./.dependencies/numpy-1.9.1.tar.gz -C ./src
	cd ./src/numpy-1.9.1/ && $(PYTHON) setup.py install --root ../numpy
	rm -rf ./src/numpy-1.9.1/

test: dna_test points_test

dna_mpi: $(DNAMPI) $(ARGS)
	$(CC) $(FLAGS) dna_mpi $(DNAMPI) $(ARGS)

points_mpi: $(POINTSMPI) $(ARGS)
	$(CC) $(FLAGS) points_mpi $(POINTSMPI) $(ARGS)

dna_test: dna_large.dat dna_small.dat

dna_large.dat: src/numpy
	@echo "Generating large dna dataset. This could take some time."
	$(PYTHON) $(GEN)/dna_gen.py -c 6 -l 50 -n 200000 -s 0.9 -f dna_large.dat

dna_small.dat: src/numpy
	@echo "Generating small dna dataset."
	$(PYTHON) $(GEN)/dna_gen.py -c 6 -l 50 -n 20000 -s 0.9 -f dna_small.dat

points_test: points_large.dat points_small.dat

points_large.dat:
	@echo "Generating large points dataset. This could take some time."
	$(SETPATH); $(PYTHON) $(GEN)/points_gen.py -c 6 -p 200000 -o points_large.dat -v 50

points_small.dat:
	@echo "Generating small points dataset."
	$(SETPATH); $(PYTHON) $(GEN)/points_gen.py -c 6 -p 20000 -o points_small.dat -v 50

clean:
	rm -rf dna_mpi points_mpi 
	rm -rf points_large.dat points_small.dat dna_large.dat dna_small.da
	rm -rf $(NUMPY)