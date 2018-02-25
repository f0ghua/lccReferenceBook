#include <windows.h>
#include <stdio.h>
#include "gclib.h"

MEMORYSTATUS MemStat;

int main(void)
{
	char * buf1;
	// first loop uses the same pointer - the memory is released
	// each time there is a new allocation.
	for(int i = 0; i<10; i++){
		buf1 = GC_malloc(2000000);
		if(!buf1){
			printf("%s\n", "Failed\n");
		}
		GlobalMemoryStatus( &MemStat );
		printf("%s %p %d %u\n", "First loop - ", buf1, i, MemStat.dwAvailPhys);
		sleep(500);
	}

	printf("\n");
	char * buf[10];
	// second loop uses a different pointer - the memory is not released
	// each time there is a new allocation.
	for(int i = 0; i<10; i++){
		buf[i] = GC_malloc(2000000);
		if(!buf[i]){
			printf("%s\n", "Failed\n");
		}
		GlobalMemoryStatus( &MemStat );
		printf("%s %p %d %u\n", "Second loop - ", buf[i], i, MemStat.dwAvailPhys);
		sleep(500);
	}

	return 0;
}
