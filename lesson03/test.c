#include <stdio.h>

int main() {
	int a = 10;
	#ifdef DEBUG
	printf("爬树\n");
	#endif
	for (int i = 0; i < 3; ++i) {
		printf("hello \n");
	}
	
	return 0;
}
