#include <cstdlib>
#include <cstdio>
#include "randomc.h"

int main(int argc, char **argv) {
	        TRandomMersenne rg(std::atoi(argv[1]));
		        for (int i=0;i<20;i++) printf("%d\n",rg.IRandom(0,200));
			        return 0;
}

