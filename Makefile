# ******* BLAS SETTINGS ******* #
# ASGD can be compiled either against an external cBLAS library (provided by the installer),
# or against a naive cBLAS implementation (part of the ASGD distribution).
#
# make will default to the naive implementation
# To use an external BLAS library, please set:
# BLAS =			(1 for external library - leaving empty uses naive cBLAS)
# BLAS_INCDIRS =	(dirs with cBLAS headers)
# BLAS_LIBDIRS =	(dirs with cBLAS libs)
# BLAS_LIBS =		(cBLAS libs to link against)
# BLAS_HEADER =		(name of the cBLAS header file in brackets, e.g. <cblas.h>)

# cBLAS specific variables (in this example, for MKL)
BLAS =
BLAS_INCDIRS = -I/opt/intel/mkl/include
BLAS_LIBDIRS = -L/opt/intel/mkl/lib/intel64
BLAS_LIBS = -lmkl_intel_lp64 -lmkl_intel_thread -lmkl_core -liomp5 -lpthread -fopenmp
BLAS_HEADER = <mkl_cblas.h>

# ******* COMPILATION SETTINGS ******* #

# list of directories with headers
INCDIRS = -Iinclude -Isimple_blas -Itests

# list of directories with libraries
LIBDIRS =

# list of libraries to link against
LIBS = -lm

# list of header files
HEADERS = include/asgd.h \
		  include/asgd_data.h \
		  include/asgd_core.h \
		  include/asgd_errors.h \
		  include/asgd_blas.h \

# list of object files to compile with
OBJS = obj/asgd.o \
	   obj/asgd_data.o \
	   obj/asgd_core.o \
	   obj/asgd_errors.o

# list of macros for the compiler
DEFS =

# compiler
CC = gcc

# debug flags
DEBUG = -g

# compiler flags
CFLAGS = -Wall -Werror -fmax-errors=5 -std=c99 -fPIC -O3 -march=native

# if BLAS=1, link against external BLAS library
ifeq ($(BLAS),1)
INCDIRS += $(BLAS_INCDIRS)
LIBDIRS += $(BLAS_LIBDIRS)
LIBS += $(BLAS_LIBS)
DEFS += -DASGD_BLAS -DBLAS_HEADER='$(BLAS_HEADER)'
else
OBJS += obj/simple_blas.o
endif

COMPILE_PREFIX = $(CC) $(CFLAGS) $(DEBUG) $(INCDIRS) $(LIBDIRS) $(LIBS) $(DEFS)

.PHONY: asgd
asgd: lib/libasgd.so lib/libasgd.a

lib/libasgd.so: lib $(OBJS)
	$(COMPILE_PREFIX) -shared -o lib/libasgd.so $(OBJS)

lib/libasgd.a: lib $(OBJS)
	ar -cvq lib/libasgd.a $(OBJS)

.PHONY: asgd_unit_test
asgd_unit_tests: bin/asgd_unit bin/asgd_core_unit bin/asgd_data_unit bin/simple_blas_unit

bin/asgd_unit: bin $(OBJS) obj/test_utils.o tests/asgd_unit.c
	$(COMPILE_PREFIX) -o bin/asgd_unit tests/asgd_unit.c $(OBJS) obj/test_utils.o

bin/asgd_core_unit: bin $(OBJS) obj/test_utils.o tests/asgd_core_unit.c
	$(COMPILE_PREFIX) -o bin/asgd_core_unit tests/asgd_core_unit.c $(OBJS) obj/test_utils.o

bin/asgd_data_unit: bin $(OBJS) obj/test_utils.o tests/asgd_data_unit.c
	$(COMPILE_PREFIX) -o bin/asgd_data_unit tests/asgd_data_unit.c $(OBJS) obj/test_utils.o

bin/simple_blas_unit: bin $(OBJS) obj/test_utils.o tests/simple_blas_unit.c
	$(COMPILE_PREFIX) -o bin/simple_blas_unit tests/simple_blas_unit.c $(OBJS) obj/test_utils.o

obj/asgd.o: obj $(HEADERS) asgd.c
	$(COMPILE_PREFIX) -c -o obj/asgd.o asgd.c

obj/asgd_core.o: obj $(HEADERS) asgd_core.c
	$(COMPILE_PREFIX) -c -o obj/asgd_core.o asgd_core.c

obj/asgd_data.o: obj $(HEADERS) asgd_data.c
	$(COMPILE_PREFIX) -c -o obj/asgd_data.o asgd_data.c

obj/asgd_errors.o: obj $(HEADERS) asgd_errors.c
	$(COMPILE_PREFIX) -c -o obj/asgd_errors.o asgd_errors.c

obj/simple_blas.o: obj simple_blas/simple_blas.c simple_blas/simple_blas.h
	$(COMPILE_PREFIX) -c -o obj/simple_blas.o simple_blas/simple_blas.c

obj/test_utils.o: obj tests/test_utils.c tests/test_utils.h
	$(COMPILE_PREFIX) -c -o obj/test_utils.o tests/test_utils.c

lib:
	mkdir -p lib

obj:
	mkdir -p obj

bin:
	mkdir -p bin

# clean up compilation byproducts
.PHONY: clean
clean:
	rm -fR bin/
	rm -fR lib/
	rm -fR obj/

