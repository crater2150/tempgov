#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define SYS_GLOB_CPUS      "/sys/devices/system/cpu/cpu?/cpufreq/scaling_governor"
#define SYS_THERMAL        "/sys/devices/virtual/thermal/thermal_zone0/temp"

#define GOV_DEFAULT        "ondemand"
#define GOV_HIGHTEMP       "powersave"

#define SYSLOG_IDENT       "tempgov"
#define SYSLOG_FORMAT      "Switching to governor \"%s\""

#define PIDFILE_PATH       "/var/run/tempgov.pid"

#define MSG_RUNNING        "Another tempgov instance is running already\n"
#define MSG_STARTING       "Starting temperature supervising"

#define MSG_ERR_PID_CREAT  "PID file creation failed"
#define MSG_ERR_PID_LOCK   "PID file locking failed"

#define FMT_MSG_ADD_CPU    "Adding CPU: %s"

#define CHECK_FREQUENCY             8
#define TEMP_HIGH_THRESHOLD         92000
#define TEMP_COOLDOWN_THRESHOLD     80000

#endif
