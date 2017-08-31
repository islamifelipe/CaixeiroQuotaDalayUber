#!/bin/bash

for i in 20 30 40 50 100 200 500
do
	./algoritm1 < INSTANCIAS/tspPQDU/$i/assimetrico/INST-B-$i-5.txt > testes/exato/$i/simetrico/INST-B-$i-5.out
	./tspb < INSTANCIAS/tspPQDU/$i/simetrico/INST-B-$i-5.txt > testes/PCVB/$i/simetrico/INST-B-$i-5.out
done