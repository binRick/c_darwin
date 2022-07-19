#pragma once
#include <stdio.h>
#define DEBUG_PID_ENV    false
#include <sys/sysctl.h>
////////////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_vector/include/vector.h"
#include "djbhash/src/djbhash.h"
#include "fsio.h"
#include "kitty/kitty.h"
#include "parson.h"
#include "socket99/socket99.h"
#include "str-replace.h"
#include "stringbuffer.h"
#include "stringfn.h"
#include "tiny-regex-c/re.h"
//////////////////////////////////////////
#define C_RED        "\x1b[31m"
#define C_GREEN      "\x1b[32m"
#define C_YELLOW     "\x1b[33m"
#define C_BLUE       "\x1b[34m"
#define C_MAGENTA    "\x1b[35m"
#define C_CYAN       "\x1b[36m"
#define C_RESET      "\x1b[0m"
FILE *auditFile;
struct auditEvent {
  char  *filePath;
  char  *processPath;
  pid_t processPid;
  char  *username;
  int   eventType;
};

void shutDown();
bool isRoot();
FILE * initPipe();
struct auditEvent getEvent(FILE *auditFile);
char * getProcFromPid(pid_t pid);
void printEvent(struct auditEvent currentEvent);
char * getEventType(int event);
bool raiseAlertForFile(char *filePath);
bool raiseAlertForProcess(char *processPath);
bool isWhiteList(char *path);
bool isAllowedRule(char *path);
char * getEventString(int event);
bool startsWith(const char *pre, const char *str);
char * getCurrentTime();
char * getCurrentTimestamp();
int audit_main();
