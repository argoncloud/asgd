#include "asgd_core.h"

#include <math.h>

#include "asgd_blas.h"
#include "asgd_errors.h"

void asgd_core_partial_fit(
		size_t batch_size,
		unsigned long *n_observs,
		float *sgd_step_size,
		float *asgd_step_size,

		float l2_reg,
		float sgd_step_size0,
		float sgd_step_size_sched_exp,
		float sgd_step_size_sched_mul,

		size_t n_feats,
		size_t n_points,
		size_t n_classes,

		float *sgd_weights,
		float *sgd_bias,
		float *asgd_weights,
		float *asgd_bias,

		float *X,
		float *y)
{
	// M x M
	// sgd_weights = n_feats x n_classes
	// sgd_bias = n_classes x 1
	// X = n_points x n_feats
	// y = n_points x 1
	
	asgd_assert(batch_size <= n_points, ASGD_ERROR_BATCHSIZE_INVALID);

	float *margin = malloc(batch_size * n_classes * sizeof(*margin));
	float *obs;
	asgd_assert(margin != NULL, ASGD_ERROR_MARGIN_NOMEM);

	for (size_t i=0; i < n_points; i += batch_size)
	{
		// the last iteration might require a smaller batch
		// in case X_rows % batch_size != 0
		if (i + batch_size > n_points)
		{
			batch_size = n_points - i;
		}

		obs = X + i * n_feats;
		
		// compute margin //
		// margin = label * (obs * sgd_weights + sgd_bias)
		for (size_t j = 0; j < batch_size; ++j)
		{
			float *margin_row = margin + j * n_classes;
			cblas_scopy(n_classes, sgd_bias, 1, margin_row, 1);
		}
		cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
				batch_size, n_classes, n_feats,
				1.f,
				obs, n_feats,
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

		for (size_t k = 0; k < batch_size; ++k)
		{
			for (size_t j = 0; j < n_classes; ++j)
			{
				size_t index = k * n_classes + j;
				float label = y[i+k] == j ? 1.f : -1.f;
				label = label * margin[index] < 1.f ? label : 0.f;
	
				if(fabs(label) > 0.f)
				{
					// sgd_weights += sgd_step_size * label * obs
					// sgd_bias += sgd_step_size * label
					cblas_saxpy(
							n_feats,
							*sgd_step_size * label / batch_size,
							obs + k * n_feats, 1,
							sgd_weights + j, n_classes);
	
					sgd_bias[j] += *sgd_step_size * label / batch_size;
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

		// update step_sizes //
		*n_observs += 1;

		float sgd_step_size_sched =
			1 + sgd_step_size0 * *n_observs * sgd_step_size_sched_mul;

		*sgd_step_size = sgd_step_size0 /
			pow(sgd_step_size_sched, sgd_step_size_sched_exp);

		*asgd_step_size = 1.0f / *n_observs;
	}

	free(margin);
}

