
all: main.o  exato 

	
main.o : main.cpp
	g++ -lpthread -g -c  main.cpp

		
exato : main.o  
	g++ -lpthread -m64 -g  -lm -o algoritm1 main.o -Llib/ -lgurobi_c++ -lgurobi65 -lm

