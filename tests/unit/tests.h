/*****************************************************************************
 *
 *  tests.h
 *
 *  Edinburgh Soft Matter and Statistical Physics Group and
 *  Edinburgh Parallel Computing Centre
 *
 *  Kevin Stratford (kevin@epcc.ed.ac.uk)
 *  (c) 2010-2014 The University of Edinburgh
 *
 *****************************************************************************/

#ifndef TESTS_H
#define TESTS_H

void test_assert_info(const int lvalue, int line, const char * file);

#define test_assert(x) test_assert_info((x), __LINE__, __FILE__)

#ifdef _FLOAT_
  #define TEST_PRECISION_TOLERANCE  1.0e-07
#else
  #define TEST_PRECISION_TOLERANCE 1.0e-14
#endif

#define TEST_FLOAT_TOLERANCE  1.0e-07
#define TEST_DOUBLE_TOLERANCE 1.0e-14

/* List of test drivers (see relevant file.c) */

int test_pe_suite(void);
int test_random_suite(void);
int test_rt_suite(void);
int test_memory_suite(void);
int test_data_input_suite(void);
int test_lr_suite(void);
int test_prior_suite(void);
int test_mvn_suite(void);
int test_chain_suite(void);
int test_sample_suite(void);
// int test_timer_suite(void);

#endif
