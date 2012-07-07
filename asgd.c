#include "asgd.h"

#include "asgd_core.h"
#include "asgd_errors.h"

/**
 * @param r As many random integers (any integer value) as the # of rows - 1
 */
/*void matrix_row_shuffle(matrix_t *m, int *r)
{
	// do a Durstenfeld shuffle
	for (size_t i = m->rows-1; i > 0; --i) {
		size_t j = r[i-1] % (i+1);
		// flip current row with a random row among remaining ones
		for (size_t k = 0; k < m->cols; ++k) {
			matrix_swap(m, i, k, j, k);
		}
	}
}*/

/**
 * Constructor for the ASGD structure
 *
 * @param n_features The number of features.
 * @param n_points The number of points.
 * @param n_classes The number of classes.
 * @param sgd_step_size The SGD step size parameter.
 * @param l2_regularization The L2 regularization parameter.
 *
 * @return An ASGD instance ready for fitting. Call asgd_destr to deallocate. 
 */
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
	asgd->asgd_step_size = 1;
	asgd->asgd_step_size0 = 1;

	asgd->sgd_step_size_sched_exp = 2. / 3.;
	asgd->sgd_step_size_sched_mul = asgd->l2_reg;
	
	asgd->n_observs = 0;
	return asgd;
}

/**
 * Destructor for the ASGD structure
 *
 * @param asgd The ASGD instance to destroy
 */
void asgd_destr(
		asgd_t *asgd)
{
	free(asgd->sgd_weights);
	free(asgd->sgd_bias);
	free(asgd->asgd_weights);
	free(asgd->asgd_bias);
	free(asgd);
}

/*void partial_fit(
		nb_asgd_t *data,
		matrix_t *X,
		matrix_t *y,
		size_t *perm,
		size_t batch_size)
{
	core_partial_fit(
			batch_size,
			&data->n_observs,
			&data->sgd_step_size,
			&data->asgd_step_size,
			
			data->l2_reg,
			data->sgd_step_size0,
			data->sgd_step_size_scheduling_exp,
			data->sgd_step_size_scheduling_mul,
			
			data->sgd_weights->data,
			data->sgd_weights->rows,
			data->sgd_weights->cols,

			data->sgd_bias->data,
			data->sgd_bias->rows,
			data->sgd_bias->cols,

			data->asgd_weights->data,
			data->asgd_weights->rows,
			data->asgd_weights->cols,

			data->asgd_bias->data,
			data->asgd_bias->rows,
			data->asgd_bias->cols,

			X->data,
			X->rows,
			X->cols,

			y->data,
			y->rows,
			y->cols,
			perm);
}*/

void asgd_fit(
	asgd_t *asgd,
	bool (*retrieve_data)(void *state, float **X, float **y),
	void *state,
	size_t batch_size)
{
	float *X, *y;
	while (retrieve_data(state, &X, &y))
	{
		asgd_core_partial_fit(
			batch_size,
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
			y);
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

