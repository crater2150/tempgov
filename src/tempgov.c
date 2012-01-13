#include <string.h>
#include <stdio.h>
#include <glob.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

#define SYS_GLOB_CPUS  "/sys/devices/system/cpu/cpu?/cpufreq/scaling_governor"
#define SYS_THERMAL    "/sys/devices/virtual/thermal/thermal_zone0/temp"

#define GOV_DEFAULT    "ondemand"
#define GOV_HIGHTEMP   "powersave"

#define SYSLOG_IDENT   "tempgov"
#define SYSLOG_FORMAT  "Switching to governor \"%s\""

#define CHECK_FREQUENCY  8
#define TEMP_THRESHOLD   92000
static void setgov(const char* governor);
static void cleanup(int signal);
static char* tempgov();
FILE** govhandles;

int main(int argc, char* argv[])
{

	pid_t pid, sid;

	pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}
	umask(0);

	sid = setsid();
	if (sid < 0) {
		exit(EXIT_FAILURE);
	}

	if ((chdir("/")) < 0) {
		exit(EXIT_FAILURE);
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	openlog(SYSLOG_IDENT, 0, LOG_DAEMON);

	glob_t pglob;
	int globerr;
	if ((globerr = glob(SYS_GLOB_CPUS, 0, NULL, &pglob)) != 0) {
		syslog(LOG_ERR, "Glob error.");
		return 1;
	}

	govhandles = malloc(sizeof(FILE*) * pglob.gl_pathc + 1);
	for(int i = 0; i < pglob.gl_pathc; i++) {
		govhandles[i] = fopen(pglob.gl_pathv[i],"w");
		syslog(LOG_INFO, "Adding CPU: %s", pglob.gl_pathv[i]);
	}
	govhandles[pglob.gl_pathc] = NULL;
	globfree(&pglob);

	signal(15, &cleanup);

	syslog(LOG_INFO, "Starting temperature supervising");
	while(1) {
		sleep(8);
		char* gov = tempgov();
		if(gov != NULL) {
			syslog(LOG_NOTICE, SYSLOG_FORMAT, gov);
			free(gov);
		}
	}

	return 0;
}

void cleanup(int signal)
{
	for(FILE** i = govhandles;*i != NULL; i++) {
		fclose(*i);
	}

}
static void setgov(const char* governor)
{
	for(FILE** i = govhandles;*i != NULL; i++) {
		rewind(*i);
		fwrite(governor, sizeof(char), strlen(governor), *i);
	}
}

static char* tempgov()
{
	static _Bool on_hightemp = 0;
	FILE* systemp = fopen(SYS_THERMAL, "r");
	char tempc[7];
	tempc[6] = '\0';
	fread(tempc, sizeof(char), 6, systemp);
	fclose(systemp);

	char* changed = NULL;

	int temp = atoi(tempc);
	if(temp > TEMP_THRESHOLD && ! on_hightemp) {
		setgov(GOV_HIGHTEMP);
		changed = strdup(GOV_HIGHTEMP);
		on_hightemp = 1;
	} else if(on_hightemp) {
		setgov(GOV_DEFAULT);
		changed = strdup(GOV_DEFAULT);
		on_hightemp = 0;
	}
	return changed;
}
