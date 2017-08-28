
all: naive.o LKHParser.o  exato  
	
LKHParser.o : LKHParser.cpp
	g++ -lpthread -g -c  LKHParser.cpp

FileHelper.o : FileHelper.cpp
	g++ -lpthread -g -c  FileHelper.cpp

naive.o : naive.cpp
	g++ -lpthread -g -c  naive.cpp

		
exato : naive.o  FileHelper.o LKHParser.o 
	g++ -lpthread -m64 -g  -lm -o naive naive.o FileHelper.o LKHParser.o -Llib/ -lgurobi_c++ -lgurobi65 -lm
