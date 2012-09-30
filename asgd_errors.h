#ifndef _ASGD_ERRORS_H_
#define _ASGD_ERRORS_H_

#include <stdbool.h>

#define	ASGD_ERROR_ASGD_INIT_NOMEM \
	"Not enough memory to allocate the ASGD structure."
#define	ASGD_ERROR_L2REG_INVALID \
	"The L2 regularization parameter must be positive."
#define	ASGD_ERROR_FEATURES_INVALID \
	"The number of features must be positive."
#define	ASGD_ERROR_POINTS_INVALID \
	"The number of points must be positive."
#define	ASGD_ERROR_CLASSES_INVALID \
	"The number of classes must be positive."
#define	ASGD_ERROR_BATCHSIZE_INVALID \
	"The batch size must be smaller or equal to than the number of points."
#define	ASGD_ERROR_MARGIN_NOMEM \
	"Not enough memory to allocate the margin matrix."
#define ASGD_ERROR_DATA_NOMEM \
	"Not enough memory to create the data source."
#define ASGD_ERROR_CORE_NONPOSITIVE_NCLASSES \
	"The number of classes must be strictly positive."
#define ASGD_ERROR_DATA_XY_MISMATCHED_ROWS \
	"The X matrix must have the same number of rows as the y vector for the same batch."
#define ASGD_ERROR_TOO_MANY_CLASSES \
	"The number of classes should be at most 2^32"

/*!
 * \brief Assert whether a condition is true. If the assertion fails,
 * print an error message and exit with failure.
 *
 * \param condition An expression that must evaluate to true
 * \param message The message to print if the expression is false
 */
void asgd_assert(
		bool condition,
		const char *message);

/*!
 * \brief Verify that a condition is true. If the verification fails,
 * print an error message and return false.
 *
 * \param condition An expression that should evaluate to true
 * \param message The message to print if the expression is false
 */
bool asgd_verify(
		bool condition,
		const char *message);

#endif

