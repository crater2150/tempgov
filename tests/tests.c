#include <CUnit/CUnit.h>
#include <CUnit/CUError.h>
#include <CUnit/TestDB.h>
#include <CUnit/TestRun.h>
#include <CUnit/Basic.h>

#include "test_tempgov.h"

int main(int argc, char **argv)
{
	if (CUE_SUCCESS != CU_initialize_registry())
		      return CU_get_error();

	test_tempgov_init();
	/* Run all tests using the basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_basic_show_failures(CU_get_failure_list());
	return CU_get_error();
}
