#ifndef _ASGD_H_
#define _ASGD_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "asgd_data.h"

typedef struct _asgd asgd_t;
struct _asgd
{
	size_t n_feats;
	size_t n_classes;
	float l2_reg;

	float *sgd_weights;
	float *sgd_bias;
	float sgd_step_size;
	float sgd_step_size0;

	float sgd_step_size_sched_exp;
	float sgd_step_size_sched_mul;

	float *asgd_weights;
	float *asgd_bias;
	float asgd_step_size;
	float asgd_step_size0;

	unsigned long n_observs;
};

/*!
 * Constructor for the ASGD structure
 *
 * \param[in] n_features The number of features.
 * \param[in] n_classes The number of classes.
 * \param[in] sgd_step_size The SGD step size parameter.
 * \param[in] l2_regularization The L2 regularization parameter.
 *
 * \return An ASGD instance ready for fitting. Call asgd_destr to deallocate. 
 */
asgd_t *asgd_init(
	size_t n_features,
	size_t n_classes,
	float sgd_step_size,
	float l2_regularization);

/*!
 * Destructor for the ASGD structure
 *
 * \param[in] asgd The ASGD instance to destroy
 */
void asgd_destr(
		asgd_t *asgd);

void asgd_fit(
	asgd_t *asgd,
	asgd_data_X_t *X,
	asgd_data_y_t *y);

void asgd_predict(
	asgd_t *asgd,
	asgd_data_X_t *X,
	asgd_data_y_t *y);

#endif

