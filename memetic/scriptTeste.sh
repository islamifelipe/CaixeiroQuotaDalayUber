#!/bin/bash

for i in 10 20 30 40 50 100 200 500
do
	echo "Tamanho $i"
	./main 4958498 < ../../INSTANCIAS/tspPQDU/$i/simetrico/INST-A-$i-3.txt > Teste/simetrico/INST-A-$i-3.out 
	./main 4958498 < ../../INSTANCIAS/tspPQDU/$i/simetrico/INST-A-$i-4.txt > Teste/simetrico/INST-A-$i-4.out 
	./main 4958498 < ../../INSTANCIAS/tspPQDU/$i/simetrico/INST-A-$i-5.txt > Teste/simetrico/INST-A-$i-5.out

	./main 4958498 < ../../INSTANCIAS/tspPQDU/$i/simetrico/INST-B-$i-3.txt > Teste/simetrico/INST-B-$i-3.out
	./main 4958498 < ../../INSTANCIAS/tspPQDU/$i/simetrico/INST-B-$i-4.txt > Teste/simetrico/INST-B-$i-4.out 
	./main 4958498 < ../../INSTANCIAS/tspPQDU/$i/simetrico/INST-B-$i-5.txt > Teste/simetrico/INST-B-$i-5.out  
done