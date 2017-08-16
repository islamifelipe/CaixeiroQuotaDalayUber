#!/bin/bash

mkdir simetricas
mkdir assimetricas

for i in 10 20 30 40 50 100 200 500 600 1000
do
	./geradorSimetricas $i  0.2 -0.3 > simetricas/sim"$i"_1.in
	sleep 1
	./geradorSimetricas $i  0.6 -0.7 > simetricas/sim"$i"_2.in
	sleep 1
	./geradorSimetricas $i  0.7 -0.5 > simetricas/sim"$i"_3.in
	sleep 1
	./geradorSimetricas $i  0.8 -0.8 > simetricas/sim"$i"_4.in
	sleep 1
	./geradorSimetricas $i  0.4 -0.5 > simetricas/sim"$i"_5.in
	sleep 1
	./geradorSimetricas $i  0.9 -0.1 > simetricas/sim"$i"_6.in
	sleep 1
	./geradorSimetricas $i  0.1 -0.8 > simetricas/sim"$i"_7.in
	sleep 1
	./geradorSimetricas $i  0.3 -0.4 > simetricas/sim"$i"_8.in
	sleep 1
	./geradorSimetricas $i  0.4 -0.7 > simetricas/sim"$i"_9.in
	sleep 1
	./geradorSimetricas $i  0.1 -0.2 > simetricas/sim"$i"_10.in


	./geradorAssimetricas $i  0.2 -0.3 > assimetricas/assim"$i"_1.in
	sleep 1
	./geradorAssimetricas $i  0.6 -0.7 > assimetricas/assim"$i"_2.in
	sleep 1
	./geradorAssimetricas $i  0.7 -0.5 > assimetricas/assim"$i"_3.in
	sleep 1
	./geradorAssimetricas $i  0.8 -0.8 > assimetricas/assim"$i"_4.in
	sleep 1
	./geradorAssimetricas $i  0.4 -0.5 > assimetricas/assim"$i"_5.in
	sleep 1
	./geradorAssimetricas $i  0.9 -0.1 > assimetricas/assim"$i"_6.in
	sleep 1
	./geradorAssimetricas $i  0.1 -0.8 > assimetricas/assim"$i"_7.in
	sleep 1
	./geradorAssimetricas $i  0.3 -0.4 > assimetricas/assim"$i"_8.in
	sleep 1
	./geradorAssimetricas $i  0.4 -0.7 > assimetricas/assim"$i"_9.in
	sleep 1
	./geradorAssimetricas $i  0.1 -0.2 > assimetricas/assim"$i"_10.in

done