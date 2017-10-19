#!/bin/bash


for i in 10 20 30 40 50 100 200 500
do
	echo "Tamanho $i"
	./main 34324 < ../../INSTANCIAS/tspPQDU/$i/simetrico/INST-C-$i-3.txt > Teste/simetrico/INST-C-$i-3.out 
	./main 234343 < ../../INSTANCIAS/tspPQDU/$i/simetrico/INST-C-$i-4.txt > Teste/simetrico/INST-C-$i-4.out 
	./main 934834 < ../../INSTANCIAS/tspPQDU/$i/simetrico/INST-C-$i-5.txt > Teste/simetrico/INST-C-$i-5.out

done