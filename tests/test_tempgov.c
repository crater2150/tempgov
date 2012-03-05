#include <CUnit/CUnit.h>
#include <CUnit/CUError.h>
#include <CUnit/TestDB.h>
#include <CUnit/TestRun.h>
#include <CUnit/Basic.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "test_tempgov.h"

#include "../src/constants.h"
#undef SYS_GLOB_CPUS
#undef SYS_THERMAL
#define SYS_GLOB_CPUS      "/tmp/test_cpu"
#define SYS_THERMAL        "/tmp/test_temp"

#include "../src/tempgov.c"

#define TEST_TEMP_LOW (TEMP_COOLDOWN_THRESHOLD - 5)
#define TEST_TEMP_HIGH (TEMP_HIGH_THRESHOLD + 5)
#define TEST_TEMP_MEDIUM ((TEMP_HIGH_THRESHOLD + TEMP_COOLDOWN_THRESHOLD) /2)

#define CU_ASSERT_FILE_CONTAINS(fp,expected) CU_ASSERT_TRUE( \
		cu_assert_file_contains(fp, expected))

static FILE* govhandle = NULL;
static FILE* temphandle = NULL;

// tests, in order of execution
static void test_setup_files();
static void test_glob_and_open();
static void test_temperature_reading();
static void test_setting_of_governor();
static void test_tempgov();

// helpers
static char* itoa(int i);
static _Bool cu_assert_file_contains(FILE* fp, const char const * expected);
static void file_replace(FILE* fp, char* contents);

static void test_setup_files()
{

	//create pseudo governor control file
	govhandle = fopen(SYS_GLOB_CPUS, "w+");
	CU_ASSERT_PTR_NOT_NULL(govhandle);
	fputs(GOV_DEFAULT, govhandle);
	fflush(govhandle);

	char* temp = itoa(TEST_TEMP_MEDIUM);

	//create pseudo temperature file
	temphandle = fopen(SYS_THERMAL, "w+");
	CU_ASSERT_PTR_NOT_NULL(temphandle);
	fputs(temp, temphandle);
	fflush(govhandle);

	free(temp);
}

static void test_glob_and_open()
{
	CU_ASSERT_EQUAL_FATAL(init_tempgov(), 0);

	int matches = 0;
	for(FILE** i = govhandles;*i != NULL; i++) {
		//file stream is open
		CU_ASSERT_EQUAL(ferror(*i), 0);
		matches++;
	}
	//with test glob pattern, only 1 file should be found
	CU_ASSERT_EQUAL(matches, 1);
}

static void test_setting_of_governor()
{
	cu_assert_file_contains(govhandle, GOV_DEFAULT);

	// tempgov should set governor to default on first call, even if it is
	// set already.
	CU_ASSERT_EQUAL(GOV_CHANGE_HIGHTEMP, set_governor(GOV_CHANGE_HIGHTEMP));
	cu_assert_file_contains(govhandle, GOV_HIGHTEMP);

	// after initial setting, no change shall occur, when same governor is
	// requested
	CU_ASSERT_EQUAL(GOV_NOCHANGE, set_governor(GOV_CHANGE_HIGHTEMP));
	cu_assert_file_contains(govhandle, GOV_HIGHTEMP);

	// setting another governor should cause a write
	CU_ASSERT_EQUAL(GOV_CHANGE_DEFAULT, set_governor(GOV_CHANGE_DEFAULT));
	cu_assert_file_contains(govhandle, GOV_HIGHTEMP);
}

static void test_temperature_reading()
{
	file_replace(temphandle, "0");
	cu_assert_file_contains(temphandle, "0");
	CU_ASSERT_EQUAL(read_temp(), 0);

	file_replace(temphandle, "67000");
	cu_assert_file_contains(temphandle, "67000");
	CU_ASSERT_EQUAL(read_temp(), 67000);

	file_replace(temphandle, "42215");
	cu_assert_file_contains(temphandle, "42215");
	CU_ASSERT_EQUAL(read_temp(), 42215);

	// ignore decimal places
	file_replace(temphandle, "123.987");
	cu_assert_file_contains(temphandle, "123.987");
	CU_ASSERT_EQUAL(read_temp(), 123);
}

static void test_tempgov()
{
	// prepare state
	char* med_temp = itoa(TEST_TEMP_MEDIUM);
	char* high_temp = itoa(TEST_TEMP_HIGH);
	char* low_temp = itoa(TEST_TEMP_LOW);
	set_governor(GOV_CHANGE_DEFAULT);
	cu_assert_file_contains(govhandle, GOV_DEFAULT);

	// governor is correct for current temperature
	// tempgov should not do anything
	file_replace(temphandle, med_temp);
	cu_assert_file_contains(temphandle, med_temp);
	CU_ASSERT_EQUAL(tempgov(), GOV_NOCHANGE);
	cu_assert_file_contains(govhandle, GOV_DEFAULT);

	// governor should switch on high temparature
	file_replace(temphandle, high_temp);
	cu_assert_file_contains(temphandle, high_temp);
	CU_ASSERT_EQUAL(tempgov(), GOV_CHANGE_HIGHTEMP);
	cu_assert_file_contains(govhandle, GOV_HIGHTEMP);

	// governor should not switch until low threshold is reached
	file_replace(temphandle, med_temp);
	cu_assert_file_contains(temphandle, med_temp);
	CU_ASSERT_EQUAL(tempgov(), GOV_NOCHANGE);
	cu_assert_file_contains(govhandle, GOV_HIGHTEMP);

	// governor should switch back after cooldown
	file_replace(temphandle, low_temp);
	cu_assert_file_contains(temphandle, low_temp);
	CU_ASSERT_EQUAL(tempgov(), GOV_CHANGE_DEFAULT);
	cu_assert_file_contains(govhandle, GOV_DEFAULT);
}

void test_tempgov_init()
{
	CU_pSuite pSuite = NULL;

	pSuite = CU_add_suite(__FILE__, NULL, NULL);

	CU_add_test(pSuite, "test_setup_files", test_setup_files);
	CU_add_test(pSuite, "test_glob_and_open", test_glob_and_open);
	CU_add_test(pSuite, "test_temperature_reading", test_temperature_reading);
	CU_add_test(pSuite, "test_setting_of_governor", test_setting_of_governor);
	CU_add_test(pSuite, "test_tempgov", test_tempgov);
}

static char* itoa(int i)
{
	char* str = malloc(sizeof(char) * 10);
	snprintf(str, 10, "%d", i);
	return str;
}

static _Bool cu_assert_file_contains(FILE* fp, const char const * expected)
{
	// do not leave changes in file pointer
	long cur_pos = ftell(fp);

	int length = strlen(expected);
	char* content = malloc(sizeof(char) * length);
	rewind(fp);

	// check file contents against expected string
	if(fgets(content, length, fp) == NULL) return false;
	//printf("\n --- \n %s \n %s \n", content, expected);
	if(strncmp(content, expected, length) != 0) return false;

	fseek(fp, cur_pos, SEEK_SET);
	return true;
}

static void file_replace(FILE* fp, char* contents)
{
	fflush(fp);
	rewind(fp);
	ftruncate(fileno(fp),0);
	fputs(contents, fp);
	fflush(fp);
}
