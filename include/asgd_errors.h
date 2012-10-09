#ifndef _ASGD_ERRORS_H_
#define _ASGD_ERRORS_H_

#include <stdbool.h>
#include <stdio.h>

#define	ASGD_ERROR_ASGD_INIT_NOMEM \
	"Not enough memory to allocate the necessary structures for the ASGD."
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
	"The number of classes should be at most 2^32."
#define ASGD_ERROR_CANNOT_OPEN_X_FILE \
	"The file containing the data for X could not be opened."
#define ASGD_ERROR_CANNOT_CLOSE_X_FILE \
	"The file containing the data for X could not be closed."
#define ASGD_ERROR_CANNOT_OPEN_Y_FILE \
	"The file containing the data for y could not be opened."
#define ASGD_ERROR_CANNOT_CLOSE_Y_FILE \
	"The file containing the data for y could not be closed."
#define ASGD_ERROR_CANNOT_RETRIEVE_PAGESIZE \
	"A call to sysconf(_SC_PAGESIZE) failed. Could not retrieve the system page size."
#define ASGD_ERROR_CANNOT_MMAP_X_FILE \
	"Could not memory map file for X."
#define ASGD_ERROR_CANNOT_MUNMAP_X_FILE \
	"Could not unmap memory mapped file for X."
#define ASGD_ERROR_CANNOT_MMAP_Y_FILE \
	"Could not memory map file for y."
#define ASGD_ERROR_CANNOT_MUNMAP_Y_FILE \
	"Could not unmap memory mapped file for y."
#define ASGD_ERROR_CORE_PARTIAL_FIT \
	"core partial fit function failed."
#define ASGD_ERROR_CORE_DECISION_FUNCTION \
	"core decision function failed."
#define ASGD_ERROR_CORE_PREDICT \
	"core predict failed."
#define ASGD_ERROR_ASGD_FIT \
	"fit failed."
#define ASGD_ERROR_ASGD_PREDICT \
	"predict failed."
#define ASGD_ERROR_BUFFER_INIT \
	"buffer init failed."
#define ASGD_ERROR_BUFFER_GET \
	"buffer get failed."
#define ASGD_ERROR_BUFFER_DESTR \
	"buffer destr failed."
#define ASGD_ERROR_DATA_X_INIT \
	"Could not initialize X data."
#define ASGD_ERROR_DATA_Y_INIT \
	"Could not initialize y data."
#define ASGD_ERROR_ASGD_INIT \
	"Could not initialize ASGD."
#define ASGD_ERROR_ASGD_DESTR \
	"Could not destry ASGD."
#define ASGD_ERROR_ASGD_NULL \
	"Found a NULL pointer for ASGD."
#define ASGD_ERROR_DATA_NULL \
	"Found a NULL pointer for data getter struct."
#define ASGD_ERROR_STATE_NULL \
	"Found a NULL pointer for data getter state."
#define ASGD_ERROR_BUFFER_NULL \
	"Found a NULL pointer for an internal buffer."
#define ASGD_ERROR_NEXT_X_BLOCK \
	"Failed retrieving the next block of the X matrix."
#define ASGD_ERROR_NEXT_Y_BLOCK \
	"Failed retrieving the next block of the y vector."


/*!
 * \brief Assert whether a condition is true. If the assertion fails,
 * print an error message and exit with failure.
 *
 * \param condition An expression that must evaluate to true.
 * \param message The message to print if the expression is false.
 */
#ifdef DEBUG
#define asgd_assert(condition, message) \
	if (!(condition)) \
	{ \
		fprintf(stderr, "ERROR: %s\n", (message)); \
		return false; \
	}
#else
#define asgd_assert(condition, message) \
	if (!(condition)) \
	{ \
		return false; \
	}
#endif

#endif

