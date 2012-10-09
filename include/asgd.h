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
 * \brief Constructor for the ASGD structure.
 *
 * \param[in] n_features The number of features.
 * \param[in] n_classes The number of classes.
 * \param[in] sgd_step_size The SGD step size parameter.
 * \param[in] l2_regularization The L2 regularization parameter.
 *
 * \return An ASGD instance ready for fitting. Call asgd_destr to deallocate.
 */
bool asgd_init(
	asgd_t *asgd,
	size_t n_features,
	size_t n_classes,
	float sgd_step_size,
	float l2_regularization);

/*!
 * \brief Destructor for the ASGD structure
 *
 * \param[in] asgd The ASGD instance to destroy. Must have been `asdg_init`ed before.
 */
bool asgd_destr(
		asgd_t *asgd);

/*!
 * \brief Train the ASGD to match the features found in a row of X with the
 * class found in the corresponding y entry.
 *
 * \param[in,out] asgd The asgd to train.
 * \param[in] X A structure retrieving features for each point.
 * \param[in] y A structure retrieving the class for each point.
 *
 */
bool asgd_fit(
	asgd_t *asgd,
	asgd_data_X_t *X,
	asgd_data_y_t *y);

/*!
 * \brief Using a trained ASGD, find which class corresponds to each point.
 *
 * \param[in] asgd A trained asgd
 * \param[in] X A structure retrieving features for each point to classify.
 * \param[out] y A structure retrieving storage for the class predicted for each point.
 *
 */
bool asgd_predict(
	asgd_t *asgd,
	asgd_data_X_t *X,
	asgd_data_y_t *y);

#endif

