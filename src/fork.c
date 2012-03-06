#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "constants.h"
#include "fork.h"
#include "opts.h"

static void pidfile_check_lock();
static void do_fork();
static void pidfile_write_lock();

static int pidfile_fd;
static FILE* pidfile;

void daemonize()
{
	pidfile_fd = creat(opt_pidfile, 0);
	if(pidfile_fd == -1) {
		perror(MSG_ERR_PID_CREAT);
		exit(EXIT_FAILURE);
	}

	pidfile_check_lock();

	do_fork();

	pidfile_write_lock();
}

void cleanup_fork()
{
	flock(pidfile_fd, LOCK_UN);
	fclose(pidfile);
}

static void pidfile_check_lock()
{
	int lock_success = flock(pidfile_fd, LOCK_EX | LOCK_NB);

	if(lock_success == -1) {
		if(errno == EWOULDBLOCK) {
			printf(MSG_RUNNING);
		} else {
			perror(MSG_ERR_PID_LOCK);
		}
		exit(EXIT_FAILURE);
	}

	ftruncate(pidfile_fd, 0);
}

static void pidfile_write_lock()
{
	pidfile = fdopen(pidfile_fd, "w");

	pid_t ownpid = getpid();
	fprintf(pidfile, "%d\n", (int) ownpid);
	fflush(pidfile);
}

static void do_fork()
{
	pid_t pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}
	umask(0);

	pid_t sid = setsid();
	if (sid < 0) {
		exit(EXIT_FAILURE);
	}

	if ((chdir("/")) < 0) {
		exit(EXIT_FAILURE);
	}
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}
