ARGS = ./src/args/*.c
DNAMPI = ./src/dna_mpi/*.c
POINTSMPI = ./src/points_mpi/*.c

.PHONY: clean

all: dna_mpi points_mpi

dna_mpi: $(DNAMPI) $(ARGS)
	mpiCC -o dna_mpi $(DNAMPI) $(ARGS)

points_mpi: $(POINTSMPI) $(ARGS)
	mpiCC -o points_mpi $(POINTSMPI) $(ARGS)

clean:
	rm -rf dna_mpi points_mpi