
all: main.o  exato tspp TSP.o

	
main.o : main.cpp
	g++ -lpthread -g -c  main.cpp

TSP.o : TSP.cpp
	g++ -lpthread -g -c  TSP.cpp

		
exato : main.o  
	g++ -lpthread -m64 -g  -lm -o algoritm1 main.o -Llib/ -lgurobi_c++ -lgurobi65 -lm


tspp : TSP.o  
	g++ -lpthread -m64 -g  -lm -o tsp TSP.o -Llib/ -lgurobi_c++ -lgurobi65 -lm
