#!/bin/bash

for i in 20 30 40 50 100 200 500
do
	./algoritm1 < INSTANCIAS/tspPQDU/$i/assimetrico/INST-B-$i-4.txt > testes/exato/$i/assimetrico/INST-B-$i-4.out
		./tspb < INSTANCIAS/tspPQDU/$i/assimetrico/INST-B-$i-4.txt > testes/PCVB/$i/assimetrico/INST-B-$i-4.out
done