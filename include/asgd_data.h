#ifndef _ASGD_DATA_H_
#define _ASGD_DATA_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// variable size buffer - used internally by the library
typedef struct _asgd_data_buffer asgd_data_buffer_t;
struct _asgd_data_buffer
{
	float *data;
};

void asgd_data_buffer_init(asgd_data_buffer_t *buffer);

float *asgd_data_buffer_get(asgd_data_buffer_t *buffer, size_t size);

void asgd_data_buffer_destr(asgd_data_buffer_t *buffer);

// X
typedef struct _asgd_data_X asgd_data_X_t;
struct _asgd_data_X
{
	bool (*next_block)(asgd_data_X_t *state, float **data, size_t *rows);
};

typedef struct _asgd_data_X_memory asgd_data_X_memory_t;
struct _asgd_data_X_memory
{
	asgd_data_X_t data;

	float *items;
	size_t n_points;
	size_t n_feats;
	size_t points_left;
	size_t batch_size;
};

typedef struct _asgd_data_X_file asgd_data_X_file_t;
struct _asgd_data_X_file
{
	asgd_data_X_t data;

	const char *file_name;
	int fd;
	size_t n_points;
	size_t n_feats;
	size_t points_done;
	size_t points_left;
	size_t batch_size;
};

/*!
 * \brief Init a retriever for an X matrix that is fully stored in memory
 *
 * \param data The structure to init
 * \param n_points The number of points (rows) in the matrix
 * \param n_feats The number of features for each point (cols) in the matrix
 * \param batch_size The number of points to return at each iteration (must match y's)
 */
void asgd_data_X_memory_init(
		asgd_data_X_memory_t *data,
		float *items,
		size_t n_points,
		size_t n_feats,
		size_t batch_size);

/*!
 * \brief Init a retriever for an X matrix that is fully stored in a file
 *
 * \param data The structure to init
 * \param file_name The name of the file where to find the data
 * \param n_points The number of points (rows) in the matrix
 * \param n_feats The number of features for each point (cols) in the matrix
 * \param batch_size The number of points to return at each iteration (must match y's)
 */
void asgd_data_X_data_init(
		asgd_data_X_memory_t *data,
		const char *file_name,
		size_t n_points,
		size_t n_feats,
		size_t batch_size);

// y
typedef struct _asgd_data_y asgd_data_y_t;
struct _asgd_data_y
{
	bool (*next_block)(asgd_data_y_t *state, uint32_t **data, size_t *rows);
};

typedef struct _asgd_data_y_memory asgd_data_y_memory_t;
struct _asgd_data_y_memory
{
	asgd_data_y_t data;

	uint32_t *items;
	size_t n_points;
	size_t points_left;
	size_t batch_size;
};

/*!
 * \brief Init a retriever for a y vector that is fully stored in memory
 *
 * \param data The structure to init
 * \param n_points The number of points (entries) in the vector
 * \param batch_size The number of points to return at each iteration (must match X's)
 */
void asgd_data_y_memory_init(
		asgd_data_y_memory_t *data,
		uint32_t *items,
		size_t n_points,
		size_t batch_size);

#endif

