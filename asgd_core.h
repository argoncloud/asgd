#ifndef _ASGD_CORE_H_
#define _ASGD_CORE_H_

#include <stdlib.h>
#include <stdint.h>

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
		
		float *margin);

void core_decision_function(
		size_t n_points,
		size_t n_feats,
		size_t n_classes,
		
		float *sgd_weights,
		float *sgd_bias,
		float *X,

		float *dec);

void core_predict(
	size_t n_points,
	size_t n_classes,

	float *dec,
	uint32_t *res);

#endif

