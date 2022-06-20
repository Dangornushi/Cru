#include <stdio.h>

int __CRU_Strput(char *__s1, int __size) {
	for(size_t __i=0;__i<__size;__i++)
		printf("%c", __s1[__i]);
	return 0;
}
int main () {
	char x [] = "dangomushi";
	__CRU_Strput(x, sizeof(x));	return 0;
}
int start(void) {
	return main();
}