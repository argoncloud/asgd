#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../asgd_core.h"
#include "../asgd_errors.h"
#include "test_utils.h"

static bool test_core_partial_fit()
{
	asgd_test_print_header(ASGD_TEST_TEST, "core_partial_fit");

	unsigned long n_observs = 0;
	float sgd_step_size = 1e-1f;
	float asgd_step_size = 0.5f;

	const float l2_reg = 1e-3f;
	const float sgd_step_size0 = 1e-1f;
	const float sgd_step_size_sched_exp = 2.f / 3.f;
	const float sgd_step_size_sched_mul = l2_reg;

	#define N_POINTS (5)
	#define N_FEATS (5)
	#define N_CLASSES (5)

	float sgd_weights[N_FEATS][N_CLASSES] = {
		{1.f, 0.f, 0.f, 0.f, 0.f},
		{0.f, 1.f, 0.f, 0.f, 0.f},
		{0.f, 0.f, 1.f, 0.f, 0.f},
		{0.f, 0.f, 0.f, 1.f, 0.f},
		{0.f, 0.f, 0.f, 0.f, 1.f}
	};
	float sgd_bias[N_CLASSES] = {0.1f, 0.1f, 0.1f, 0.1f, 0.1f};
	float asgd_weights[N_FEATS][N_CLASSES] = {
		{2.f, 4.f, 6.f, 8.f, 10.f},
		{2.f, 4.f, 6.f, 8.f, 10.f},
		{2.f, 4.f, 6.f, 8.f, 10.f},
		{2.f, 4.f, 6.f, 8.f, 10.f},
		{2.f, 4.f, 6.f, 8.f, 10.f}
	};
	float asgd_bias[N_CLASSES] = {2.f, 4.f, 6.f, 8.f, 10.f};

	float X[N_POINTS][N_FEATS] = {
		{1.f, 0.1f, 0.1f, 0.1f, 0.1f},
		{0.1f, 1.f, 0.1f, 0.1f, 0.1f},
		{0.1f, 0.1f, 1.f, 0.1f, 0.1f},
		{0.1f, 0.1f, 0.1f, 1.f, 0.1f},
		{0.1f, 0.1f, 0.1f, 0.1f, 1.f}
	};
	float y[N_POINTS] = {0.f,1.f,2.f,3.f,4.f};
		
	float *margin = malloc(N_POINTS * N_CLASSES * sizeof(*margin));
	asgd_assert(margin != NULL, ASGD_ERROR_MARGIN_NOMEM);

	unsigned long exp_observs = 1;
	float exp_sgd_step_size = 0.099993334;
	float exp_asgd_step_size = 1.f;
	float exp_sgd_weights[N_FEATS][N_CLASSES] = {
		{0.9919f, -0.026f, -0.026f, -0.026f, -0.026f},
		{-0.026f, 0.9919f, -0.026f, -0.026f, -0.026f},
		{-0.026f, -0.026f, 0.9919f, -0.026f, -0.026f},
		{-0.026f, -0.026f, -0.026f, 0.9919f, -0.026f},
		{-0.026f, -0.026f, -0.026f, -0.026f, 0.9919f}
	};
	float exp_sgd_bias[N_CLASSES] = {0.02f, 0.02f, 0.02f, 0.02f, 0.02f};

	float exp_asgd_weights[N_FEATS][N_CLASSES] = {
		{1.49595f, 1.987f, 2.987f, 3.987f, 4.987f},
		{0.987f, 2.49595f, 2.987f, 3.987f, 4.987f},
		{0.987f, 1.987f, 3.49595f, 3.987f, 4.987f},
		{0.987f, 1.987f, 2.987f, 4.49595f, 4.987f},
		{0.987f, 1.987f, 2.987f, 3.987f, 5.49595f}
	};
	float exp_asgd_bias[N_CLASSES] = {1.01f, 2.01f, 3.01f, 4.01f, 5.01f};

	asgd_core_partial_fit(
			&n_observs,
			&sgd_step_size,
			&asgd_step_size,
			
			l2_reg,
			sgd_step_size0,
			sgd_step_size_sched_exp,
			sgd_step_size_sched_mul,
			
			N_POINTS,
			N_FEATS,
			N_CLASSES,
			
			(float *)sgd_weights,
			(float *)sgd_bias,
			(float *)asgd_weights,
			(float *)asgd_bias,

			(float *)X,
			(float *)y,

			margin);

	bool succ = true;

	succ &= asgd_test_matrix_diff(
			"sgd_weights",
			(float *)exp_sgd_weights,
			(float *)sgd_weights,
			N_FEATS,
			N_CLASSES,
			1e-5f);

	succ &= asgd_test_matrix_diff(
			"sgd_bias",
			(float *)exp_sgd_bias,
			(float *)sgd_bias,
			1,
			N_CLASSES,
			1e-5f);

	succ &= asgd_test_matrix_diff(
			"asgd_weights",
			(float *)exp_asgd_weights,
			(float *)asgd_weights,
			N_FEATS,
			N_CLASSES,
			1e-5f);

	succ &= asgd_test_matrix_diff(
			"asgd_bias",
			(float *)exp_asgd_bias,
			(float *)asgd_bias,
			1,
			N_CLASSES,
			1e-5f);

	printf("%s Testing parameters\n", ASGD_TEST_TEST);

	if (n_observs == exp_observs &&
			sgd_step_size == exp_sgd_step_size &&
			asgd_step_size == exp_asgd_step_size)
	{
		printf("%s Parameters as expected\n", ASGD_TEST_PASS);
	}
	else
	{
		succ = false;
		printf("%s Parameters not as expected\n", ASGD_TEST_FAIL);
		printf("%s Exp: n_observs=%10lu sgd_step_size=%10f asgd_step_size=%10f\n",
				ASGD_TEST_FAIL,
				exp_observs, exp_sgd_step_size, exp_asgd_step_size);
		printf("%s Got: n_observs=%10lu sgd_step_size=%10f asgd_step_size=%10f\n",
				ASGD_TEST_FAIL,
				n_observs, sgd_step_size, asgd_step_size);
	}

	free(margin);

	if (succ)
	{
		asgd_test_print_footer(ASGD_TEST_PASS, "core_partial_fit");
	}
	else
	{
		asgd_test_print_header(ASGD_TEST_FAIL, "core_partial_fit");
	}

	return succ;
}

int main(void)
{
	bool res = true;
	
	asgd_test_print_header(ASGD_TEST_TEST, "asgd_data_unit");

	res &= test_core_partial_fit();

	if (res)
	{
		asgd_test_print_footer(ASGD_TEST_PASS, "asgd_data_unit");
		return EXIT_SUCCESS;
	}
	else
	{
		asgd_test_print_footer(ASGD_TEST_FAIL, "asgd_data_unit");
		return EXIT_FAILURE;
	}
}

