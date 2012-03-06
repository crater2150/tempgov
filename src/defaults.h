#ifndef DEFAULTS_H
#define DEFAULTS_H 

#define SYS_GLOB_CPUS      "/sys/devices/system/cpu/cpu?/cpufreq/scaling_governor"
#define SYS_THERMAL        "/sys/devices/virtual/thermal/thermal_zone0/temp"

#define GOV_DEFAULT        "ondemand"
#define GOV_HIGHTEMP       "powersave"

#define CHECK_FREQUENCY             8
#define TEMP_HIGH_THRESHOLD         92000
#define TEMP_COOLDOWN_THRESHOLD     80000

#define PIDFILE_PATH       "/var/run/tempgov.pid"

#endif /* DEFAULTS_H */
