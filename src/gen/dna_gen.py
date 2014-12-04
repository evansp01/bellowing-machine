 #!/usr/bin/env python3

import random
import argparse

bases = ["T", "C", "A", "G"]

def random_base():
	return bases[random.randint(0,3)]


def random_string(k):
	l = []
	for i in range(k):
		l.append(random_base())
	return ''.join(l)

def relative_string(parent, similarity):
	newstr = []
	for i in parent:
		if random.random() > similarity:
			newstr.append(random_base())
		else:
			newstr.append(i)
	return ''.join(newstr)

def gen_n_relatives(parent, similarity, n, f):
	l = []
	for _ in range(n):
		f.write(relative_string(parent,similarity)+"\n")
	return l

def distribution(lengths, clusters, ppc, similarity,f):
	means = []
	for _ in range(clusters):
		parent = random_string(lengths)
		gen_n_relatives(parent, similarity, ppc,f)
		means.append(parent)
	return means

def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("-c", "--clusters", help="the number of clusters you would like the dataset to have", type=int, required=True)
	parser.add_argument("-l", "--length", help="the length of the individual dna strands", type=int, required=True)
	parser.add_argument("-n", "--number", help="the number of additional points in each cluster", type=int, required=True)
	parser.add_argument("-s", "--similarity", help="a real between 0 and 1 describing how similar points within a cluster are. 1 is the most similar", type=float, required=True)
	parser.add_argument("-f", "--file", help="the file to write the dataset to", required=True)
	args = parser.parse_args()
	f = open(args.file, "w")
	f.write('%s %s\n' % (args.length, args.clusters*args.number))
	points = distribution(args.length, args.clusters, args.number, args.similarity,f)
	f.close()

if __name__ == "__main__":
	main()
