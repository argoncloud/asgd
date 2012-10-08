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

// internal data structures for the X and y getters
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
	long page_size;
	char *map_data;
	size_t map_len;

	size_t n_points;
	size_t n_feats;
	size_t points_done;
	size_t points_left;
	size_t batch_size;
};

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

typedef struct _asgd_data_y_file asgd_data_y_file_t;
struct _asgd_data_y_file
{
	asgd_data_y_t data;

	const char *file_name;
	int fd;
	long page_size;
	char *map_data;
	size_t map_len;
	bool writable;

	size_t n_points;
	size_t n_feats;
	size_t points_done;
	size_t points_left;
	size_t batch_size;
};

/*!
 * \brief Create a reader for an X matrix stored in memory.
 * The reader shall be inited each time the matrix has to be read from the start.
 *
 * \param[out] data The struct remembering how to read the matrix. Call `data.next_block` to read a batch.
 * \param[in] items A pointer to the first entry in the matrix.
 * The matrix should have consecutive (non-padded) entries and be row-major.
 * Each row should represent a list of features for a point.
 * \param[in] n_points The number of points (rows) in the matrix as stored in memory.
 * \param[in] n_feats The number of features for each point (cols) in the matrix as stored in memory.
 * \param[in] batch_size The desired batch size, i.e. how many points should be made available at each iteration.
 * `batch_size` must be the same for X and y if the two are read together.
 * The length of all batches will be always at least `batch_size`, possibly less in the last iteration
 * (if `batch_size` does not divide `n_points`).
 */
void asgd_data_X_memory_init(
		asgd_data_X_memory_t *data,
		float *items,
		size_t n_points,
		size_t n_feats,
		size_t batch_size);

/*!
 * \brief Create a reader for an X matrix stored in a file.
 *
 * \param[out] data The struct remembering how to read the matrix. Call `data.next_block` to read a batch.
 * The memory returned for a batch is always read-only.
 * \param file_name The name of the file containing the matrix.
 * The file should contain consecutive (non-padded) entries and be row-major.
 * Endianness should be same as the machine's.
 * Each row should represent a list of features for a point.
 * \param[in] n_points The number of points (rows) in the matrix.
 * \param[in] n_feats The number of features for each point (cols) in the matrix.
 * \param[in] batch_size The desired batch size, i.e. how many points should be made available at each iteration.
 * `batch_size` must be the same for X and y if the two are read together.
 * The length of all batches will be always at least `batch_size`, possibly less in the last iteration
 * (if `batch_size` does not divide `n_points`).
 */
void asgd_data_X_file_init(
		asgd_data_X_file_t *data,
		const char *file_name,
		size_t n_points,
		size_t n_feats,
		size_t batch_size);

/*!
 * \brief Create a reader for a y vector stored in memory.
 *
 * \param[out] data The struct remembering how to read the vector. Call `data.next_block` to read a batch.
 * \param items A pointer to the first entry in the vector.
 * The vector should have consecutive (non-padded) entries, each representing the class for one point.
 * \param[in] n_points The number of points (entries) in the vector as stored in memory.
 * \param[in] batch_size The desired batch size, i.e. how many points should be made available at each iteration.
 * `batch_size` must be the same for X and y if the two are read together.
 * The length of all batches will be always at least `batch_size`, possibly less in the last iteration
 * (if `batch_size` does not divide `n_points`).
 */
void asgd_data_y_memory_init(
		asgd_data_y_memory_t *data,
		uint32_t *items,
		size_t n_points,
		size_t batch_size);

/*!
 * \brief Create a reader for a y vector stored in a file.
 *
 * \param[out] data The struct remembering how to read the vector. Call `data.next_block` to read a batch.
 * \param[in] file_name The name of the file containing the data.
 * The file should contain consecutive (non-padded) entries, each representing the class for one point.
 * Endianness should be same as the machine's. If the y vector is to be written, the file *must* be long enough
 * to fit the entire vector. The POSIX `truncate` function may be used for this purpose.
 * \param[in] n_points The number of points (entries) in the vector.
 * \param[in] batch_size The desired batch size, i.e. how many points should be made available at each iteration.
 * `batch_size` must be the same for X and y if the two are read together.
 * The length of all batches will be always at least `batch_size`, possibly less in the last iteration
 * (if `batch_size` does not divide `n_points`).
 * \param[in] writable Whether the memory returned for a batch should be writable. Useful for `predict`.
 */
void asgd_data_y_file_init(
		asgd_data_y_file_t *data,
		const char *file_name,
		size_t n_points,
		size_t batch_size,
		bool writable);

#endif

