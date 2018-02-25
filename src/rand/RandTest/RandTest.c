/* Test for rand library */

#include <stdio.h>
#include <rand.h>

/* Link with rand.lib

	Put rand.lib in \lcc\lib\
	and rand.h in \lcc\include\

/* See the rand library's comments for details. */

int main(void)
{
    char buf[20];
    int i;

    sgenrand(4357);

	 /* Some unsigened longs */
    for (i=0; i<1000; i++) {
        printf("%12u", genrand_l());
        if (i%5==4) printf("\n");
    }

	 /* Some doubles */
    for (i=0; i<1000; i++) {
        printf("%10f", genrand_d());
        if (i%5==4) printf("\n");
    }

	printf("\nPress <Enter>\n");
	fgets(buf, 20, stdin);

	return 0;
}

