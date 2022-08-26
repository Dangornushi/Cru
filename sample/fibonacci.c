
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===--- str ...1 bit, unsigned ---===*/

typedef struct {
    unsigned char S;
} __Cru_str;

typedef struct {
    char *S;
    int len;
} __Cru_string;

typedef struct {
    unsigned int S;
} __Cru_int;

/* ===--- Vector (String) ---===*/

typedef struct {
    char **vD;
} __Cru_Vec_string;

/* ===--- Vector (int) ---===*/

typedef struct {
    int *V;
} __Cru_Vec_int;

int __CRU_Charput(char __s1) {
    printf("%c", __s1);
    return 0;
}

int __CRU_Strput(const char *__s1, int __size) {
    for (size_t __i = 0; __i < __size; __i++) __CRU_Charput(__s1[__i]);
    return 0;
}

int __CRU_Stringput(__Cru_string *__s1) {
    printf("%s\n", (char*)__s1->S);
    return 0;
}

int __Cru_Vec_string_put(__Cru_Vec_string *__data) {
    size_t i = 0;

    printf("[ ");
    for (; i < strlen(*__data->vD) - 1; i++)
        printf("%s, ", *__data->vD++);
    printf("%s ]", *__data->vD);
    return 0;
}

char *__CRU_Add(char *s1, const char *s2) {
    char buf[512];
    sprintf(buf, "%s%s", s1, s2);
    strcpy(s1, buf);
    return s1;
}

__Cru_string _String(char *s1) {
    __Cru_string Cs = {s1, sizeof(s1)};
    return Cs;
}

__Cru_Vec_string _Vec(char **s1) {
    __Cru_Vec_string Cv = {s1};
    return Cv;
}

int fibonacci (int num) {
    if (num==1) {
        return 0;
    }
    if (num==2) {
        return 1;
    }
    return fibonacci(num-1)+fibonacci(num-2);

}int main () {
    int x = 9;
    printf("%d\n", fibonacci(x));    return 0;

}

int start(void) {
	return main();
}