#!/bin/bash

for i in 10 20 30 40 50 100 200 500
do
	echo "Tamanho $i"
	./main 2324234 < ../../INSTANCIAS/tspPQDU/$i/simetrico/INST-C-$i-3.txt  > Teste/NOVAS_C/simetrica/INST-C-$i-3.out 
	./main 9824931 < ../../INSTANCIAS/tspPQDU/$i/simetrico/INST-C-$i-4.txt  > Teste/NOVAS_C/simetrica/INST-C-$i-4.out 
	./main 5304310 < ../../INSTANCIAS/tspPQDU/$i/simetrico/INST-C-$i-5.txt  > Teste/NOVAS_C/simetrica/INST-C-$i-5.out 
done