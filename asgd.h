#ifndef _ASGD_H_
#define _ASGD_H_

#include <stdbool.h>
#include <stdlib.h>

typedef struct _asgd asgd_t;
struct _asgd
{
	size_t n_feats;
	size_t n_points;
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

asgd_t *asgd_init(
	size_t n_features,
	size_t n_points,
	size_t n_classes,
	float sgd_step_size,
	float l2_regularization);

void asgd_destr(
		asgd_t *asgd);

void asgd_fit(
		asgd_t *asgd,
		bool (*retrieve_data)(void *state, float **X, float **y),
		void *state,
		size_t batch_size);

/*void predict(
		asgd_t *asgd,
		float *X,
		float *y);*/

#endif

