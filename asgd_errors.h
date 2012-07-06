#ifndef _ASGD_ERRORS_H_
#define _ASGD_ERRORS_H_

#include <stdbool.h>

#define	ASGD_ERROR_ASGD_INIT_NOMEM \
	"Not enough memory to allocate the ASGD structure."
#define	ASGD_ERROR_L2REG_INVALID \
	"The L2 regularization parameter must be positive."

/*
 * Assert whether a condition is true. If the assertion fails,
 * print an error message and exit with failure
 *
 * @param condition An expression that must evaluate to true
 * @param message The message to print when the expression is false
 */
void asgd_assert(
		bool condition,
		const char *message);

#endif

