#ifndef _TEMPGOV_H_
#define _TEMPGOV_H_

int init_tempgov();

void cleanup_governor();

void tempgov_main_loop(int interval);

#endif
