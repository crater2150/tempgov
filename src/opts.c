#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "defaults.h"
#include "opts.h"

// set defaults
char* opt_sysfs_thermal = NULL;
char* opt_sysfs_cpus = NULL;
char* opt_default_governor = NULL;
char* opt_hightemp_governor = NULL;
char* opt_pidfile = NULL;
int opt_threshold_cooldown = TEMP_COOLDOWN_THRESHOLD;
int opt_threshold_hightemp = TEMP_HIGH_THRESHOLD;
int opt_interval = CHECK_FREQUENCY;

short parse_args(int argc, char* argv[])
{
	opt_sysfs_thermal = strdup(SYS_THERMAL);
	opt_sysfs_cpus = strdup(SYS_GLOB_CPUS);
	opt_default_governor = strdup(GOV_DEFAULT);
	opt_hightemp_governor = strdup(GOV_HIGHTEMP);
	opt_pidfile = strdup(PIDFILE_PATH);

	struct option long_options[] = {
		{"thermal",        required_argument, NULL, 't'},
		{"cpus",           required_argument, NULL, 'c'},
		{"gov-default",    required_argument, NULL, 'd'},
		{"gov-high",       required_argument, NULL, 'g'},
		{"pidfile",        required_argument, NULL, 'p'},
		{"low-threshold",  required_argument, NULL, 'l'},
		{"high-threshold", required_argument, NULL, 'h'},
		{"interval",       required_argument, NULL, 'i'},
		{0, 0, 0, 0}
	};
	char options[] = "t:c:d:g:p:l:h:i:";

	int opt;

	while ((opt = getopt_long(argc, argv, options, long_options, NULL)) != -1) {
		switch (opt) {
			case 't':
				free(opt_sysfs_thermal);
				opt_sysfs_thermal = strdup(optarg);
				break;
			case 'c':
				free(opt_sysfs_cpus);
				opt_sysfs_cpus = strdup(optarg);
				break;
			case 'd':
				free(opt_default_governor);
				opt_default_governor = strdup(optarg);
				break;
			case 'g':
				free(opt_hightemp_governor);
				opt_hightemp_governor = strdup(optarg);
				break;
			case 'p':
				free(opt_pidfile);
				opt_pidfile = strdup(optarg);
				break;
			case 'l':
				opt_threshold_cooldown = atoi(optarg);
				break;
			case 'h':
				opt_threshold_hightemp = atoi(optarg);
			case 'i':
				opt_interval = atoi(optarg);
				break;
			default:
				return 1;
				break;;
		}
	}

	return -1;
}
