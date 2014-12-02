#include <stdio.h>
#include <string.h>
#include "networkTransClass.h"

int main(int argc, char **argv)
{
	NetworkTransClass tc;

	if(!tc.establishConnection())
		printf("establishConnection error\n");
	else
		tc.run();

	tc.closeConnection();

	

	return 0;
}
