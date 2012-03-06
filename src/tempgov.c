#define _GNU_SOURCE

#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include "constants.h"
#include "opts.h"
#include "tempgov.h"

#define GOV_UNKNOWN         -1
#define GOV_NOCHANGE        0
#define GOV_CHANGE_HIGHTEMP 1
#define GOV_CHANGE_DEFAULT  2

static FILE** govhandles;

static int set_governor(const int target_governor);
static int check_temp_limits(const int temp);
static int read_temp();
static int tempgov();

int init_tempgov()
{
	glob_t pglob;
	int globerr;
	if ((globerr = glob(opt_sysfs_cpus, 0, NULL, &pglob)) != 0) {
		return globerr;
	}

	govhandles = malloc(sizeof(FILE*) * pglob.gl_pathc + 1);
	for(int i = 0; i < pglob.gl_pathc; i++) {
		govhandles[i] = fopen(pglob.gl_pathv[i],"w");
		syslog(LOG_INFO, FMT_MSG_ADD_CPU, pglob.gl_pathv[i]);
	}
	govhandles[pglob.gl_pathc] = NULL;
	globfree(&pglob);
	return 0;
}

void cleanup_governor()
{
	for(FILE** i = govhandles;*i != NULL; i++) {
		fclose(*i);
	}
}

void tempgov_main_loop(const int interval)
{
	while(1) {
		sleep(interval);
		switch(tempgov()) {
			case GOV_CHANGE_HIGHTEMP:
				syslog(LOG_NOTICE, SYSLOG_FORMAT,
						opt_hightemp_governor);
				break;
			case GOV_CHANGE_DEFAULT:
				syslog(LOG_NOTICE, SYSLOG_FORMAT,
						opt_default_governor);
				break;
		}
	}
}

static int set_governor(const int target_governor)
{
	//ensure the governor is set at first reading
	static int current_governor = GOV_UNKNOWN;

	if(target_governor == current_governor) {
		return GOV_NOCHANGE;
	}

	char* governor_name;
	if(target_governor == GOV_CHANGE_HIGHTEMP) {
		governor_name = strdup(opt_hightemp_governor);
	} else if(target_governor == GOV_CHANGE_DEFAULT) {
		governor_name = strdup(opt_default_governor);
	} else {
		return GOV_NOCHANGE;
	}

	for(FILE** i = govhandles;*i != NULL; i++) {
		rewind(*i);
		fwrite(governor_name, sizeof(char), strlen(governor_name), *i);
	}

	return current_governor = target_governor;
}

static int check_temp_limits(const int temp)
{
	if(temp > opt_threshold_hightemp) {
		return GOV_CHANGE_HIGHTEMP;
	} else if(temp < opt_threshold_cooldown) {
		return GOV_CHANGE_DEFAULT;
	} else {
		return GOV_NOCHANGE;
	}
}

static int read_temp()
{
	FILE* systemp = fopen(opt_sysfs_thermal, "r");
	char tempc[10];
	fgets(tempc, 10, systemp);
	fclose(systemp);
	return atoi(tempc);
}

static int tempgov()
{

	int target_state = check_temp_limits(read_temp());

	if(target_state != GOV_NOCHANGE) {
		set_governor(target_state);
		return(target_state);
	}

	return GOV_NOCHANGE;
}
