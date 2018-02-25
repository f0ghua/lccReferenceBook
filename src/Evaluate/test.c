#include <stdio.h>
#include <string.h>

#include "Evaluate.h"

int main(void)
{
	double Answer = 0;
	char Buffer[1024];
	do{
		printf("Enter an expression to be evaluated.  For example \"15/3\"\n");
		fgets(Buffer, 1024, stdin);
		if(strcmp(Buffer, "\n")==0)
			return 1;
		Answer = Evaluate(Buffer);
		if(Answer==P_ERR)
			return 1;
		printf("The answer is: %g\n", Answer);
	} while(Answer!=P_ERR);
	return 0;
}
