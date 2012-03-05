#include <glob.h>
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>

#include "constants.h"
#include "fork.h"
#include "tempgov.h"

static void cleanup(int signal);

int main(int argc, char* argv[])
{
	daemonize();

	openlog(SYSLOG_IDENT, 0, LOG_DAEMON);


	signal(15, &cleanup);

	syslog(LOG_INFO,MSG_STARTING);

	int init_state = init_tempgov();
	if(init_state != 0) {
		switch(init_state) {
			case GLOB_NOSPACE:
				syslog(LOG_ERR, "Glob ran out of memory");
			case GLOB_ABORTED:
				syslog(LOG_ERR, "Read error while looking for CPUs");
			case GLOB_NOMATCH:
				syslog(LOG_ERR, "No CPUs found in sysfs");
		}
		return 1;
	}

	tempgov_main_loop(CHECK_FREQUENCY);

	return 0;
}

static void cleanup(int signal)
{
	cleanup_governor();
	cleanup_fork();
	exit(0);
}
