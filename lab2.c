#include "types.h"
#include "user.h"

int main(int argc, char *argv[])
{
	
	int PScheduler(void);
	int PInheritance(void);
	int PAging(void);

	printf(1, "\n This program tests the correctness of your lab#2\n");
			  
	PScheduler();
	PAging();
	PInheritance();

	return 0;
}

int PScheduler(void) {
	int pid;
	int i,j,k;
			  
	printf(1, "\n  Step 2: testing the priority scheduler and setpriority(int priority)) systema call:\n");
	printf(1, "\n  Step 2: Assuming that the priorities range between range between 0 to 31\n");
	printf(1, "\n  Step 2: 0 is the highest priority. All processes have a default priority of 10\n");
	printf(1, "\n  Step 2: The parent processes will switch to priority 0\n");
	setpriority(0);
	for (i = 0; i <  3; i++) {
		pid = fork();
		if (pid > 0 )
			continue;
		else if ( pid == 0) {
			setpriority(30-10*i);	
			for (j=0;j<50000;j++)
				for(k=0;k<1000;k++)
					asm("nop");
			printf(1, "\n child# %d with priority %d has finished! \n",getpid(),30-10*i);		
			exit(0);
		}
		else
			printf(2," \n Error \n");
	}
	if(pid > 0) {
		for (i = 0; i <  3; i++)
			wait(0);
    printf(1,"\n if processes with highest priority finished first then its correct \n");
	}
	return 0;
}

int PAging(void) {
	printf(1, "\n-- PAging\n\n");
	setpriority(13);
	printf(1, "parent's initial priority : %d\n", getpriority());

	int pid = fork();
	
	if(pid) {
		wait(0);
		printf(1, "parent priority after child 1: %d\n", getpriority());
		
		int pid2 = fork();

		if(pid2){
			wait(0);
			printf(1, "parent priority after child 2: %d\n", getpriority());
		} else {
			exit(0);
		}
	} else {
		exit(0);
	}
	return 0;
}

int PInheritance(void) {
	printf(1, "\n-- PInheritance\n\n");

	setpriority(18);
	int pid = fork();

	if(pid > 0) {
		printf(1, "parent priority: %d\n", getpriority());
		wait(0);
	} else {
		printf(1, "child priority: %d\n", getpriority());
		exit(0);
	}
	exit(0);
	return 0;
}
