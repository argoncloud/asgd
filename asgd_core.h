#ifndef _ASGD_CORE_H_
#define _ASGD_CORE_H_

#include <stdlib.h>

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
		float *y);

#endif

