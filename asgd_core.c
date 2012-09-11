#include "asgd_core.h"

#include <math.h>

#include "asgd_blas.h"
#include "asgd_errors.h"

void asgd_core_partial_fit(
		unsigned long *n_observs,
		float *sgd_step_size,
		float *asgd_step_size,

		float l2_reg,
		float sgd_step_size0,
		float sgd_step_size_sched_exp,
		float sgd_step_size_sched_mul,

		size_t n_points,
		size_t n_feats,
		size_t n_classes,

		float *sgd_weights,
		float *sgd_bias,
		float *asgd_weights,
		float *asgd_bias,

		const float *X,
		const uint32_t *y,
		
		float *margin)
{
	// M x M
	// sgd_weights = n_feats x n_classes
	// sgd_bias = n_classes x 1
	// X = n_points x n_feats
	// y = n_points x 1
	// margin = n_points x n_classes
		
	// compute margin //
	// margin = X * sgd_weights + sgd_bias^T
	for (size_t j = 0; j < n_points; ++j)
	{
		float *margin_row = margin + j * n_classes;
		cblas_scopy(n_classes, sgd_bias, 1, margin_row, 1);
	}
	cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
			n_points, n_classes, n_feats,
			1.f,
			X, n_feats,
			sgd_weights, n_classes,
			1.f,
			margin, n_classes);

	// update sgd //
	if (l2_reg != 0.f)
	{
		// sgd_weights *= (1 - l2_reg * sgd_step_size)
		cblas_sscal(n_feats * n_classes,
				1 - l2_reg * *sgd_step_size,
				sgd_weights, 1);
	}

	for (size_t k = 0; k < n_points; ++k)
	{
		for (size_t j = 0; j < n_classes; ++j)
		{
			size_t index = k * n_classes + j;
			float label = y[k] == j ? 1.f : -1.f;
			label = label * margin[index] < 1.f ? label : 0.f;

			// if the margin was violated, correct it
			if(fabs(label) > 0.f)
			{
				// sgd_weights += sgd_step_size * label / n_points * X^T
				// sgd_bias += sgd_step_size * label / n_points
				cblas_saxpy(
						n_feats,
						*sgd_step_size * label / n_points,
						X + k * n_feats, 1,
						sgd_weights + j, n_classes);

				sgd_bias[j] += *sgd_step_size * label / n_points;
			}
		}
	}

	// update asgd //
	// asgd_weights = (1 - asgd_step_size) * asgd_weights + asgd_step_size * sgd_weights
	cblas_sscal(n_feats * n_classes,
			1 - *asgd_step_size,
			asgd_weights, 1);

	cblas_saxpy(n_feats * n_classes,
			*asgd_step_size,
			sgd_weights, 1,
			asgd_weights, 1);

	// asgd_bias = (1 - asgd_step_size) * asgd_bias + asgd_step_size * sgd_bias
	cblas_sscal(n_classes,
			1.f - *asgd_step_size,
			asgd_bias, 1);

	cblas_saxpy(n_classes,
			*asgd_step_size,
			sgd_bias, 1,
			asgd_bias, 1);

	// update step sizes //
	*n_observs += 1;

	float sgd_step_size_sched =
		1 + sgd_step_size0 * *n_observs * sgd_step_size_sched_mul;

	*sgd_step_size = sgd_step_size0 /
		pow(sgd_step_size_sched, sgd_step_size_sched_exp);

	*asgd_step_size = 1.0f / *n_observs;
}

void core_decision_function(
		size_t n_points,
		size_t n_feats,
		size_t n_classes,
		
		float *sgd_weights,
		float *sgd_bias,
		float *X,

		float *dec)
{
	for (size_t i = 0; i < n_points; ++i)
	{
		cblas_scopy(n_classes, sgd_bias, 1, dec + i * n_classes, 1);
	}

	cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
			n_points, n_classes, n_feats,
			1.f,
			X, n_feats,
			sgd_weights, n_classes,
			1.f,
			dec, n_classes);
}

/*void core_predict(
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

