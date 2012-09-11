# ******* INSTRUCTIONS ******* #
# ASGD can be compiled either against an external cBLAS library (provided by the installer),
# or against a naive BLAS implementation (part of the ASGD distribution).
#
# make will default to the naive implementation.
# To use an external BLAS library, please set:
# TYPE = blas		(for instance, call `make TYPE=blas`)
# BLAS_INCDIRS =	(dirs with cBLAS headers)
# BLAS_LIBDIRS =	(dirs with cBLAS libs)
# BLAS_LIBS =		(cBLAS lib files to link against)
# BLAS_HEADER =		(name of the cBLAS header file in brackets, e.g. <cblas.h>)
#
# Available make targets are:
#
# simple_blas_unit
# 		Compiles a unit test for the naive BLAS implementation
#		The unit test will exit with status 0 if the test passes


# ******* COMPILATION SETTINGS ******* #

# list of directories with headers
INCDIRS = 

# list of directories with libraries
LIBDIRS = 

# list of libraries to link against
LIBS = -lm -lrt

# list of object files to compile with
OBJS =

# list of macros for the compiler
DEFS = -D_POSIX_C_SOURCE=199309 

# compiler
CC = gcc

# debug flags
DEBUG = -g

# compiler flags
CFLAGS = -Wall -Werror -fmax-errors=5 -std=c99 -fPIC -O3 -march=native

# MKL
BLAS_INCDIRS = -I/opt/intel/mkl/include
BLAS_LIBDIRS = -L/opt/intel/mkl/lib/intel64
BLAS_LIBS = -lmkl_intel_lp64 -lmkl_intel_thread -lmkl_core -liomp5 -lpthread -fopenmp
BLAS_HEADER = <mkl_cblas.h>

# if TYPE=blas, link against external BLAS library
ifeq ($(TYPE),blas)
INCDIRS += $(BLAS_INCDIRS)
LIBDIRS += $(BLAS_LIBDIRS)
LIBS += $(BLAS_LIBS)
DEFS += -DASGD_BLAS -DBLAS_HEADER='$(BLAS_HEADER)'
else
OBJS += obj/simple_blas.o
endif

COMPILE_PREFIX = $(CC) $(CFLAGS) $(DEBUG) $(INCDIRS) $(LIBDIRS) $(LIBS) $(DEFS)

asgd_unit: bin obj/simple_blas.o obj/test_utils.o obj/asgd_errors.o obj/asgd.o obj/asgd_core.o
	$(COMPILE_PREFIX) -o bin/asgd_unit tests/asgd_unit.c obj/simple_blas.o obj/test_utils.o obj/asgd_errors.o obj/asgd.o obj/asgd_core.o

asgd_core_unit: bin obj/simple_blas.o obj/test_utils.o obj/asgd_errors.o obj/asgd_core.o tests/asgd_core_unit.c
	$(COMPILE_PREFIX) -o bin/asgd_core_unit tests/asgd_core_unit.c obj/simple_blas.o obj/test_utils.o obj/asgd_errors.o obj/asgd_core.o

asgd_data_unit: bin obj/test_utils.o obj/asgd_errors.o obj/asgd_data.o tests/asgd_data_unit.c
	$(COMPILE_PREFIX) -o bin/asgd_data_unit tests/asgd_data_unit.c obj/test_utils.o obj/asgd_errors.o obj/asgd_data.o

simple_blas_unit: bin obj/test_utils.o obj/simple_blas.o tests/simple_blas_unit.c
	$(COMPILE_PREFIX) -o bin/simple_blas_unit tests/simple_blas_unit.c obj/simple_blas.o obj/test_utils.o

obj/asgd.o: obj asgd_errors.h asgd.c asgd.h
	$(COMPILE_PREFIX) -c -o obj/asgd.o asgd.c

obj/asgd_core.o: obj asgd_errors.h asgd_blas.h asgd_core.c asgd_core.h
	$(COMPILE_PREFIX) -c -o obj/asgd_core.o asgd_core.c

obj/asgd_data.o: obj asgd_errors.h asgd_data.c asgd_data.h
	$(COMPILE_PREFIX) -c -o obj/asgd_data.o asgd_data.c

obj/asgd_errors.o: obj asgd_errors.c asgd_errors.h
	$(COMPILE_PREFIX) -c -o obj/asgd_errors.o asgd_errors.c

obj/simple_blas.o: obj simple_blas/simple_blas.c simple_blas/simple_blas.h
	$(COMPILE_PREFIX) -c -o obj/simple_blas.o simple_blas/simple_blas.c

obj/test_utils.o: obj tests/test_utils.c tests/test_utils.h
	$(COMPILE_PREFIX) -c -o obj/test_utils.o tests/test_utils.c

obj:
	mkdir -p obj

bin:
	mkdir -p bin

# clean up compilation byproducts
.PHONY: clean
clean:
	rm -fR bin/
	rm -fR obj/

