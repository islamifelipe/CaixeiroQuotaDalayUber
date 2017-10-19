#!/bin/bash

for i in 10 20 30 40 50 
do
	./naive < ../../INSTANCIAS/tspPQDU/$i/simetrico/INST-C-$i-3.txt > Testes/simetrico/INST-C-$i-3.out
	./naive < ../../INSTANCIAS/tspPQDU/$i/simetrico/INST-C-$i-4.txt > Testes/simetrico/INST-C-$i-4.out
	./naive < ../../INSTANCIAS/tspPQDU/$i/simetrico/INST-C-$i-5.txt > Testes/simetrico/INST-C-$i-5.out
done