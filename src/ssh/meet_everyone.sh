#!/bin/bash
for i in {00..79}; do
	ssh-keyscan "ghc$i.ghc.andrew.cmu.edu" >> ~/.ssh/known_hosts
done