#ifndef _ASGD_TEST_UTILS_H_
#define _ASGD_TEST_UTILS_H_

#include <stdbool.h>
#include <stddef.h>

//! Test statuses
#define ASGD_TEST_FAIL "[FAIL]"
#define ASGD_TEST_PASS "[PASS]"
#define ASGD_TEST_INFO "[INFO]"
#define ASGD_TEST_TEST "[TEST]"

/*!
 * \brief Print the header for a starting test
 * \param[in] status The status of the test
 * \param[in] name The name to print for the test
 */
void asgd_test_print_header(
		const char *name);

/*!
 * \brief Print the footer for a finished test
 * \param[in] status The status of the test
 * \param[in] name The name to print for the test
 */
void asgd_test_print_footer(
		const char *name,
		bool status);

/*!
 * \brief Show the different entries between two matrices
 * \param[in] name The header printed before the diff
 * \param[in] exp Matrix of expected values
 * \param[in] got Matrix of computed values
 * \param[in] rows Number of rows in exp and got		
 * \param[in] cols Number of cols in exp and got
 * \param[in] slack The maximum allowed difference between two values
 * \return Whether the two arrays are entry-wise equal up to the slack
 */
bool asgd_test_matrix_diff(
		const char *name,
		const float *exp,
		const float *got,
		size_t rows,
		size_t cols,
		float slack);

#endif


