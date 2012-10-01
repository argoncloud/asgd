#ifndef _ASGD_BLAS_H_
#define _ASGD_BLAS_H_

// determine which BLAS implementation to use

#if defined ASGD_BLAS
#include BLAS_HEADER
#else
#include "simple_blas.h"
#endif

#endif

