#ifndef OPTS_H
#define OPTS_H 

char* opt_sysfs_thermal;
char* opt_sysfs_cpus;
char* opt_default_governor;
char* opt_hightemp_governor;
char* opt_pidfile;
int opt_threshold_cooldown;
int opt_threshold_hightemp;
int opt_interval;


short parse_args(int argc, char* argv[]);

#endif /* OPTS_H */
