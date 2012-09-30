#include "asgd.h"

#include "asgd_core.h"
#include "asgd_data.h"
#include "asgd_errors.h"

asgd_t *asgd_init(
	size_t n_features,
	size_t n_classes,
	float sgd_step_size,
	float l2_regularization)
{
	asgd_t *asgd = malloc(sizeof(*asgd));
	asgd_assert(asgd != NULL, ASGD_ERROR_ASGD_INIT_NOMEM);

	asgd_assert(n_features > 0, ASGD_ERROR_FEATURES_INVALID);
	asgd_assert(n_classes > 0, ASGD_ERROR_CLASSES_INVALID);
	asgd->n_feats = n_features;
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
	asgd_data_X_t *X,
	asgd_data_y_t *y)
{
	float *X_data, *margin_data;
	uint32_t *y_data;
	size_t X_rows, y_rows;

	asgd_data_buffer_t margin;
	asgd_data_buffer_init(&margin);

	bool loop = true;
	while (loop)
	{
		loop &= X->next_block(X, &X_data, &X_rows);
		loop &= y->next_block(y, &y_data, &y_rows);
		asgd_assert(X_rows == y_rows, ASGD_ERROR_DATA_XY_MISMATCHED_ROWS);

		margin_data = asgd_data_buffer_get(
				&margin,
				X_rows * asgd->n_classes * sizeof(*margin_data));

		asgd_core_partial_fit(
			&asgd->n_observs,
			&asgd->sgd_step_size,
			&asgd->asgd_step_size,

			asgd->l2_reg,
			asgd->sgd_step_size0,
			asgd->sgd_step_size_sched_exp,
			asgd->sgd_step_size_sched_mul,

			X_rows,
			asgd->n_feats,
			asgd->n_classes,

			asgd->sgd_weights,
			asgd->sgd_bias,
			asgd->asgd_weights,
			asgd->asgd_bias,

			X_data,
			y_data,
			margin_data);
	}

	asgd_data_buffer_destr(&margin);
}

void asgd_predict(
	asgd_t *asgd,
	asgd_data_X_t *X,
	asgd_data_y_t *y)
{
	float *X_data, *decision_data;
	uint32_t *y_data;
	size_t X_rows, y_rows;

	asgd_data_buffer_t decision;
	asgd_data_buffer_init(&decision);

	bool loop = true;
	while (loop)
	{
		loop &= X->next_block(X, &X_data, &X_rows);
		loop &= y->next_block(y, &y_data, &y_rows);
		asgd_assert(X_rows == y_rows, ASGD_ERROR_DATA_XY_MISMATCHED_ROWS);

		decision_data = asgd_data_buffer_get(
				&decision,
				X_rows * asgd->n_classes * sizeof(*decision_data));

		asgd_core_decision_function(
				X_rows,
				asgd->n_feats,
				asgd->n_classes,

				asgd->asgd_weights,
				asgd->asgd_bias,

				X_data,
				decision_data);

		asgd_core_predict(
				X_rows,
				asgd->n_classes,

				decision_data,
				y_data);
	}

	asgd_data_buffer_destr(&decision);
}

