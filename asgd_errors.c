#include "asgd_errors.h"

#include <stdio.h>
#include <stdlib.h>

void asgd_assert(
		bool condition,
		const char *message)
{
	if (!condition)
	{
		fprintf(stderr, "*** FATAL ***\n%s\n", message);
		exit(EXIT_FAILURE);
	}
}

bool asgd_verify(
		bool condition,
		const char *message)
{
	if (!condition)
	{
		fprintf(stdout, "*** ERROR ***\n%s\n", message);
	}
	return condition;
}

