#include "asgd.h"

#include "asgd_core.h"
#include "asgd_errors.h"

asgd_t *asgd_init(
	size_t n_features,
	size_t n_points,
	size_t n_classes,
	float sgd_step_size,
	float l2_regularization)
{
	asgd_t *asgd = malloc(sizeof(*asgd));
	asgd_assert(asgd != NULL, ASGD_ERROR_ASGD_INIT_NOMEM);
	
	asgd_assert(n_features > 0, ASGD_ERROR_FEATURES_INVALID);
	asgd_assert(n_points > 0, ASGD_ERROR_POINTS_INVALID);
	asgd_assert(n_classes > 0, ASGD_ERROR_CLASSES_INVALID);
	asgd->n_feats = n_features;
	asgd->n_points = n_points;
	asgd->n_classes = n_classes;

	asgd_assert(l2_regularization > 0, ASGD_ERROR_L2REG_INVALID);
	asgd->l2_reg = l2_regularization;

	size_t weights_size =
		asgd->n_feats * asgd->n_classes * sizeof(asgd->asgd_weights);
	size_t bias_size =
		asgd->n_classes * 1 * sizeof(asgd->asgd_bias);
	
	asgd->sgd_weights = malloc(weights_size);
	asgd->sgd_bias = malloc(bias_size);
	asgd->asgd_weights = malloc(weights_size);
	asgd->asgd_bias = malloc(bias_size);
	asgd_assert(asgd->sgd_weights != NULL, ASGD_ERROR_ASGD_INIT_NOMEM);
	asgd_assert(asgd->sgd_bias != NULL, ASGD_ERROR_ASGD_INIT_NOMEM);
	asgd_assert(asgd->asgd_weights != NULL, ASGD_ERROR_ASGD_INIT_NOMEM);
	asgd_assert(asgd->asgd_bias != NULL, ASGD_ERROR_ASGD_INIT_NOMEM);
	
	asgd->sgd_step_size = sgd_step_size;
	asgd->sgd_step_size0 = sgd_step_size;
	asgd->asgd_step_size = 1.f;
	asgd->asgd_step_size0 = 1.f;

	asgd->sgd_step_size_sched_exp = 2.f / 3.f;
	asgd->sgd_step_size_sched_mul = asgd->l2_reg;
	
	asgd->n_observs = 0;
	return asgd;
}

void asgd_destr(
		asgd_t *asgd)
{
	free(asgd->sgd_weights);
	free(asgd->sgd_bias);
	free(asgd->asgd_weights);
	free(asgd->asgd_bias);
	free(asgd);
}

void asgd_fit(
	asgd_t *asgd,
	bool (*retrieve_data)(
		void *state,
		size_t n_feats,
		size_t n_classes,
		float **X,
		uint32_t **y,
		float **margin,
		size_t *batch_size),
	void *state)
{
	float *X, *margin;
	uint32_t *y;
	size_t batch_size;
	while (retrieve_data(
				state,
				asgd->n_feats,
				asgd->n_classes,
				&X,
				&y,
				&margin,
				&batch_size))
	{
		asgd_core_partial_fit(
			&asgd->n_observs,
			&asgd->sgd_step_size,
			&asgd->asgd_step_size,

			asgd->l2_reg,
			asgd->sgd_step_size0,
			asgd->sgd_step_size_sched_exp,
			asgd->sgd_step_size_sched_mul,

			asgd->n_feats,
			asgd->n_points,
			asgd->n_classes,

			asgd->sgd_weights,
			asgd->sgd_bias,
			asgd->asgd_weights,
			asgd->asgd_bias,

			X,
			y,
			margin);
	}
}

/*void decision_function(
	nb_asgd_t *data,
	matrix_t *X,
	matrix_t *r)
{
	// set the result vector to one, so that
	// when we multiply it becomes the bias vector
	for (size_t i = 0; i < r->rows; ++i)
	{
		for (size_t j = 0; j < r->cols; ++j)
		{
			matrix_set(r, i, j, 1.0f);
		}
	}

	cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
		r->rows, r->cols, data->asgd_weights->rows,
		1.0f,
		X->data, X->cols,
		data->asgd_weights->data, data->asgd_weights->cols,
		matrix_get(data->asgd_bias, 0, 0),
		r->data, r->cols);
}


void predict(
	nb_asgd_t *data,
	matrix_t *X,
	matrix_t *r)
{
	decision_function(data, X, r);

	for (size_t i = 0; i < r->rows; ++i)
	{
		for (size_t j = 0; j < r->cols; ++j)
		{
			// take positive as +1
			// and nonpositive as -1
			if (matrix_get(r, i, j) > 0.0f)
			{
				matrix_set(r, i, j, 1.0f);
			}
			else
			{
				matrix_set(r, i, j, -1.0f);
			}
		}
	}
}*/

