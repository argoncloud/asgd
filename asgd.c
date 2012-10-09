#include "asgd.h"

#include "asgd_core.h"
#include "asgd_data.h"
#include "asgd_errors.h"

bool asgd_init(
	asgd_t *asgd,
	size_t n_features,
	size_t n_classes,
	float sgd_step_size,
	float l2_regularization)
{
	asgd_assert(asgd != NULL, ASGD_ERROR_ASGD_NULL);

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

	return true;
}

bool asgd_destr(
		asgd_t *asgd)
{
	free(asgd->sgd_weights);
	free(asgd->sgd_bias);
	free(asgd->asgd_weights);
	free(asgd->asgd_bias);

	return true;
}

bool asgd_fit(
	asgd_t *asgd,
	asgd_data_X_t *X,
	asgd_data_y_t *y)
{
	float *X_data, *margin_data;
	uint32_t *y_data;
	size_t X_rows, y_rows;

	asgd_data_buffer_t margin;
	asgd_assert(
			asgd_data_buffer_init(&margin),
			ASGD_ERROR_BUFFER_INIT);

	while (true)
	{
		asgd_assert(X->next_block(X, &X_data, &X_rows), ASGD_ERROR_NEXT_X_BLOCK);
		asgd_assert(y->next_block(y, &y_data, &y_rows), ASGD_ERROR_NEXT_Y_BLOCK);
		asgd_assert(X_rows == y_rows, ASGD_ERROR_DATA_XY_MISMATCHED_ROWS);
		if (X_rows == 0) break;

		asgd_assert(asgd_data_buffer_get(
				&margin,
				X_rows * asgd->n_classes * sizeof(*margin_data)),
				ASGD_ERROR_BUFFER_GET);

		asgd_assert(asgd_core_partial_fit(
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
			margin.data), ASGD_ERROR_CORE_PARTIAL_FIT);
	}

	asgd_assert(asgd_data_buffer_destr(&margin), ASGD_ERROR_BUFFER_DESTR);

	return true;
}

bool asgd_predict(
	asgd_t *asgd,
	asgd_data_X_t *X,
	asgd_data_y_t *y)
{
	float *X_data, *decision_data;
	uint32_t *y_data;
	size_t X_rows, y_rows;

	asgd_data_buffer_t decision;
	asgd_assert(asgd_data_buffer_init(&decision), ASGD_ERROR_BUFFER_INIT);

	while (true)
	{
		asgd_assert(X->next_block(X, &X_data, &X_rows), ASGD_ERROR_NEXT_X_BLOCK);
		asgd_assert(y->next_block(y, &y_data, &y_rows), ASGD_ERROR_NEXT_Y_BLOCK);
		asgd_assert(X_rows == y_rows, ASGD_ERROR_DATA_XY_MISMATCHED_ROWS);
		if (X_rows == 0) break;

		asgd_assert(asgd_data_buffer_get(
				&decision,
				X_rows * asgd->n_classes * sizeof(*decision_data)),
				ASGD_ERROR_BUFFER_GET);

		asgd_assert(asgd_core_decision_function(
				X_rows,
				asgd->n_feats,
				asgd->n_classes,

				asgd->asgd_weights,
				asgd->asgd_bias,

				X_data,
				decision.data), ASGD_ERROR_CORE_DECISION_FUNCTION);

		asgd_assert(asgd_core_predict(
				X_rows,
				asgd->n_classes,

				decision.data,
				y_data), ASGD_ERROR_CORE_PREDICT);
	}

	asgd_assert(asgd_data_buffer_destr(&decision), ASGD_ERROR_BUFFER_DESTR);

	return true;
}

