#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define SYSLOG_IDENT       "tempgov"
#define SYSLOG_FORMAT      "Switching to governor \"%s\""


#define MSG_RUNNING        "Another tempgov instance is running already\n"
#define MSG_STARTING       "Starting temperature supervising"

#define MSG_ERR_PID_CREAT  "PID file creation failed"
#define MSG_ERR_PID_LOCK   "PID file locking failed"

#define FMT_MSG_ADD_CPU    "Adding CPU: %s"

#endif
