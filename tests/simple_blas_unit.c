#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../simple_blas/simple_blas.h"
#include "test_utils.h"

bool test_sscal(float slack, size_t *depth)
{
	float x[1][5] = {{3,5,7,11,13}};
	float alpha = 37;

	float exp_x[1][5] = {{111, 185, 259, 407, 481}};

	cblas_sscal(5, alpha, (float *)x, 1);

	return asgd_test_matrix_diff(
			"cblas_sscal",
			(float *)exp_x,
			(float *)x,
			1, 5,
			slack,
			depth);
}

bool test_scopy(float slack, size_t *depth)
{
	float x[8] = {2,3,5,7,11,13,17,19};
	float y[10] = {1,0,0,0,0,0,0,0,0,23};
	float exp_y[10] = {1,2,3,5,7,11,13,17,19,23};

	cblas_scopy(8, x, 1, y+1, 1);

	return asgd_test_matrix_diff(
			"cblas_scopy",
			(float *)exp_y,
			(float *)y,
			1, 10,
			slack,
			depth);
}

bool test_saxpy(float slack, size_t *depth)
{
	float x[1][5] = {{3.f,5.f,7.f,11.f,13.f}};
	float y[1][5] = {{17.f,19.f,23.f,29.f,31.f}};
	float alpha = 37.f;

	float exp_y[1][5] = {{128.f, 204.f, 282.f, 436.f, 512.f}};

	cblas_saxpy(5, alpha, (float *)x, 1, (float *)y, 1);

	return asgd_test_matrix_diff(
			"cblas_saxpy",
			(float *)exp_y,
			(float *)y,
			1, 5,
			slack,
			depth);
}

bool test_sdsdot(float slack, size_t *depth)
{
	float x[1][5] = {{3.f,5.f,7.f,11.f,13.f}};
	float y[5][1] = {{17.f},{19.f},{23.f},{29.f},{31.f}};
	float alpha = 37.f;

	float res = cblas_sdsdot(5, alpha, (float *)x, 1, (float *)y, 1);
	float exp = 1066;

	return asgd_test_matrix_diff(
			"cblas_sdsdot",
			&exp, &res,
			1, 1,
			slack,
			depth);
}

bool test_sgemv(float slack, size_t *depth)
{
	float alpha = 3.f;
	float beta = 5.f;

	float a_n[4][3] = {
		{3.f,5.f,7.f},
		{11.f,13.f,17.f},
		{19.f,23.f,29.f},
		{31.f,37.f,41.f}};
	float x_n[3][1] = {{2.f},{4.f},{6.f}};
	float y_n[4][1] = {{1.f},{2.f},{3.f},{4.f}};

	float a_t[4][3] = {
		{3.f,5.f,7.f},
		{11.f,13.f,17.f},
		{19.f,23.f,29.f},
		{31.f,37.f,41.f}};
	float x_t[4][1] = {{2.f},{4.f},{6.f},{8.f}};
	float y_t[3][1] = {{1.f},{2.f},{3.f}};

	float exp_y_n[4][1] = {{209.f}, {538.f}, {927.f}, {1388.f}};
	float exp_y_t[3][1] = {{1241.f}, {1498.f}, {1767.f}};

	cblas_sgemv(CblasRowMajor, CblasNoTrans,
			4, 3,
			alpha,
			(float *)a_n, 3,
			(float *)x_n, 1,
			beta,
			(float *)y_n, 1);
	cblas_sgemv(CblasRowMajor, CblasTrans,
			4, 3,
			alpha,
			(float *)a_t, 3,
			(float *)x_t, 1,
			beta,
			(float *)y_t, 1);

	return
		asgd_test_matrix_diff(
				"cblas_sgemv (N)",
				(float *)exp_y_n,
				(float *)y_n,
				1, 4,
				slack,
				depth) &
		asgd_test_matrix_diff(
				"cblas_sgemv (T)",
				(float *)exp_y_t,
				(float *)y_t,
				1, 3,
				slack,
				depth);
}

bool test_sger(float slack, size_t *depth)
{
	float a[4][3] = {
		{3.f,5.f,7.f},
		{11.f,13.f,17.f},
		{19.f,23.f,29.f},
		{31.f,37.f,41.f}};
	float alpha = 3.f;
	float x[4][1] = {{1.f},{2.f},{3.f},{4.f}};
	float y[1][3] = {{2.f,4.f,6.f}};

	float exp_a[4][3] = {
		{9.f,17.f,25.f},
		{23.f,37.f,53.f},
		{37.f,59.f,83.f},
		{55.f,85.f,113.f}};

	cblas_sger(CblasRowMajor,
			4, 3,
			alpha,
			(float *)x, 1,
			(float *)y, 1,
			(float *)a, 3);

	return asgd_test_matrix_diff(
			"cblas_sger",
			(float *)exp_a,
			(float *)a,
			4, 3,
			slack,
			depth);
}

bool test_sgemm(float slack, size_t *depth)
{
	float alpha = 2.f;
	float beta = 53.f;

	float a_n_n[2][3] = {{3.f,5.f,7.f},{11.f,13.f,17.f}};
	float b_n_n[3][2] = {{19.f,23.f},{29.f,31.f},{37.f,41.f}};
	float c_n_n[2][2] = {{59.f,61.f},{67.f,71.f}};
	float exp_c_n_n[2][2] = {{4049.f, 4255.f}, {5981.f, 6469.f}};

	float a_t_t[3][2] = {{19.f,23.f},{29.f,31.f},{37.f,41.f}};
	float b_t_t[2][3] = {{3.f,5.f,7.f},{11.f,13.f,17.f}};
	float c_t_t[2][2] = {{59.f,61.f},{67.f,71.f}};
	float exp_c_t_t[2][2] = {{4049.f, 5663.f}, {4573.f, 6469.f}};

	cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
			2, 2, 3,
			alpha,
			(float *)a_n_n, 3,
			(float *)b_n_n, 2,
			beta,
			(float *)c_n_n, 2);
	cblas_sgemm(CblasRowMajor, CblasTrans, CblasTrans,
			2, 2, 3,
			alpha,
			(float *)a_t_t, 2,
			(float *)b_t_t, 3,
			beta,
			(float *)c_t_t, 2);

	return
		asgd_test_matrix_diff(
				"cblas_sgemm (N)",
				(float *)exp_c_n_n,
				(float *)c_n_n,
				2, 2,
				slack,
				depth) &
		asgd_test_matrix_diff(
				"cblas_sgemm (T)",
				(float *)exp_c_t_t,
				(float *)c_t_t,
				2, 2,
				slack,
				depth);
}

int main(void)
{
	size_t depth = 0;
	float slack = 1e-7;
	bool res = true;

	asgd_test_print_header("simple_blas_unit", &depth);

	res &= test_sscal(slack, &depth);
	res &= test_scopy(slack, &depth);
	res &= test_saxpy(slack, &depth);
	res &= test_sdsdot(slack, &depth);
	res &= test_sgemv(slack, &depth);
	res &= test_sger(slack, &depth);
	res &= test_sgemm(slack, &depth);

	asgd_test_print_footer("simple_blas_unit", res, &depth);
	return res ? EXIT_SUCCESS : EXIT_FAILURE;
}

