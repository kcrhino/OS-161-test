#include <types.h>
#include <lib.h>
#include <test.h>

int printCmd(int nargs, char **args)
	{
		for (int i = 1;i < nargs; i++ )
		{
			kprintf("%s", args[i] );
			kprintf("\n");
		}
		return 0;
	} 
