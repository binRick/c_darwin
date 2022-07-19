#pragma once
#include "dbg.h"
//#include "debug-memory/debug_memory.h"
#include "audit/audit.h"
#include <libproc.h>
#include <sys/sysctl.h>

#include <bsm/libbsm.h>
#include <libgen.h>
#include <sys/ioctl.h>
#include <sys/syslimits.h>

#include <errno.h>
#include <libproc.h>
#include <pwd.h>
#include <security/audit/audit_ioctl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

u_char     *buffer;
int        recordLength;
int        recordBalance;
int        processedLength;
int        tokenCount;
int        fetchToken;
tokenstr_t token;
char       pathbuf[PATH_MAX];
char       hourbuff[26];
char       timebuff[26];


bool isRoot() {
  uid_t euid = geteuid();

  if (euid != 0) {
    return(false);
  }
  return(true);
}


void shutDown(){
  fclose(auditFile);

  fprintf(stdout, "\nExiting...\n");
  exit(0);
}


FILE * initPipe() {
  char  *auditPipe = "/dev/auditpipe";
  u_int dataFlow   = 0x00000080 | 0x40000000;
  FILE  *auditFile;
  int   auditFileDescriptor;

  auditFile = fopen(auditPipe, "r");
  if (auditFile == NULL) {
    fprintf(stderr, "Unable to open audit pipe: %s\n", auditPipe);
    perror("Error ");
    exit(1);
  }
  auditFileDescriptor = fileno(auditFile);
  int ioctlReturn;

  int mode = AUDITPIPE_PRESELECT_MODE_LOCAL;

  ioctlReturn = ioctl(
    auditFileDescriptor,
    AUDITPIPE_SET_PRESELECT_MODE,
    &mode);
  if (ioctlReturn == -1) {
    fprintf(stderr, "Unable to set the audit pipe mode to local.\n");
    perror("Error ");
  }

  int queueLength;

  ioctlReturn = ioctl(
    auditFileDescriptor,
    AUDITPIPE_GET_QLIMIT_MAX,
    &queueLength);
  if (ioctlReturn == -1) {
    fprintf(stderr,
            "Unable to get the maximum queue length of the audit pipe.\n");
    perror("Error ");
  }


  ioctlReturn = ioctl(
    auditFileDescriptor,
    AUDITPIPE_SET_QLIMIT,
    &queueLength);
  if (ioctlReturn == -1) {
    fprintf(stderr,
            "Unable to set the queue length of the audit pipe.\n");
    perror("Error ");
  }

  u_int attributableEventsMask = dataFlow;

  ioctlReturn = ioctl(
    auditFileDescriptor,
    AUDITPIPE_SET_PRESELECT_FLAGS,
    &attributableEventsMask);
  if (ioctlReturn == -1) {
    fprintf(stderr,
            "Unable to set the attributable events preselection mask.\n");
    perror("Error ");
  }

  u_int nonAttributableEventsMask = dataFlow;

  ioctlReturn = ioctl(
    auditFileDescriptor,
    AUDITPIPE_SET_PRESELECT_NAFLAGS,
    &nonAttributableEventsMask);
  if (ioctlReturn == -1) {
    fprintf(stderr,
            "Unable to set the non-attributable events preselection mask.\n");
    perror("Error ");
  }
  return(auditFile);
} /* initPipe */


struct auditEvent getEvent(FILE *auditFile) {
  struct auditEvent curr;

  memset(&curr, 0, sizeof(curr));

  recordLength = au_read_rec(auditFile, &buffer);
  if (recordLength == -1) {
    return(curr);
  }

  recordBalance   = recordLength;
  processedLength = 0;
  tokenCount      = 0;
  int i = 1;


  while (recordBalance) {
    fetchToken = au_fetch_tok(&token, buffer + processedLength, recordBalance);
    if (fetchToken == -1) {
      fprintf(stderr, "Error fetching token.\n");
      break;
    }
    switch (token.id) {
    case AUT_HEADER32:
      curr.eventType = token.tt.hdr32.e_type;
      break;
    case AUT_HEADER32_EX:
      fprintf(stderr, "AUT_HEADER32_EX\n");
      break;
    case AUT_HEADER64:
      fprintf(stderr, "AUT_HEADER64\n");
      break;
    case AUT_HEADER64_EX:
      fprintf(stderr, "AUT_HEADER64_EX\n");
      break;
    case AUT_SUBJECT32:
      curr.processPid  = (long)token.tt.subj32.pid;
      curr.processPath = getProcFromPid(token.tt.subj32.pid);
      struct passwd *user = getpwuid(token.tt.subj32.ruid);
      curr.username = user->pw_name;
      break;
    case AUT_SUBJECT64:
      fprintf(stderr, "AUT_SUBJECT64\n");
      break;
    case AUT_SUBJECT32_EX:
      fprintf(stderr, "AUT_SUBJECT32_EX\n");
      break;
    case AUT_RETURN32:
      //fprintf(stderr, "AUT_RETURN32\n");
      break;
    case AUT_RETURN64:
      fprintf(stderr, "AUT_RETURN64\n");
      break;
    case AUT_ATTR:
    case AUT_ATTR32:
      //fprintf(stderr, "AUT_ATTR\n");
      break;
    case AUT_PATH:
      curr.filePath = token.tt.path.path;
      break;
    }
    tokenCount++;
    processedLength += token.len;
    recordBalance   -= token.len;
    i++;
  }
  free(buffer);
  return(curr);
} /* getEvent */


char * getProcFromPid(pid_t pid) {
  int ret = proc_pidpath(pid, pathbuf, sizeof(pathbuf));

  if (ret <= 0) {
//    fprintf(stderr, "[!] PID %d: proc_pidpath ();\n", pid);
//    fprintf(stderr, "    %s\n", strerror(errno));
  } else {
    return(pathbuf);
  }
  return("");
}


void printEvent(struct auditEvent currentEvent) {
  if (
    raiseAlertForFile(currentEvent.filePath)
    && raiseAlertForProcess(currentEvent.processPath)) {
    char *event = getEventType(currentEvent.eventType);
    if (
      (strcmp(event, "wait4") == 0)
      || (strcmp(event, "pthread_sigmask") == 0)
      || (strcmp(event, "chdir") == 0)
      || (strcmp(event, "setpriority") == 0)
      || (strcmp(event, "pid_for_task") == 0)
      || (strcmp(event, "setgid") == 0)
      || (strcmp(event, "setuid") == 0)
      || (strcmp(event, "setpgrp") == 0)
      || (strcmp(event, "setrlimit") == 0)
      || (strcmp(event, "setreuid") == 0)
      || (strcmp(event, "setregid") == 0)
      || (strcmp(basename(strdup(currentEvent.processPath)), ".") == 0)
      || (strcmp(basename(strdup(currentEvent.processPath)), "suggestd") == 0)
      || (strcmp(basename(strdup(currentEvent.processPath)), "launchd") == 0)
      ) {
      return;
    }

    if (strcmp(event, "") == 0) {
      /*
       * fprintf(
       * stderr,
       * "%s - tokens.c - Event = \"\"\n",
       * getCurrentTimestamp()
       * );*/
      char ev[100];
      sprintf(ev, C_RED "%d"C_RESET, currentEvent.eventType);
      event = ev;
    }
    if (strcmp(event, "hide") == 0) {
      return;
    }

    //TODO: find a better way to use these colors :/
    if (currentEvent.filePath != NULL) {
      fprintf(
        stdout,
        "%s ["C_CYAN "%s"C_RESET "] Detected "C_BLUE "%8d"C_RESET " "C_YELLOW "%s"C_RESET " event from "C_GREEN "%s"C_RESET " -> %s\n",
        getCurrentTime(),
        currentEvent.username,
        currentEvent.processPid,
        event,
        basename(strdup(currentEvent.processPath)),
        currentEvent.filePath
        );
    } else {
      fprintf(
        stdout,
        "%s ["C_CYAN "%s"C_RESET "] Detected "C_BLUE "%8d"C_RESET " "C_MAGENTA "%s"C_RESET " event from "C_RED "%s"C_RESET "\n",
        getCurrentTime(),
        currentEvent.username,
        currentEvent.processPid,
        event,
        basename(strdup(currentEvent.processPath))
        );
    }
/*
 *  fprintf(
 *    stderr,
 *    "%s - tokens.c - [%s] Detected %s event from %s -> %s - ",
 *    getCurrentTimestamp(),
 *    currentEvent.username,
 *    event,
 *    basename(strdup(currentEvent.processPath)),
 *    currentEvent.filePath
 *    );
 *  fprintf(
 *    stderr,
 *    "EventType: %i - Pid: %i - ProcessPath: %s\n",
 *    currentEvent.eventType,
 *    currentEvent.processPid,
 *    currentEvent.processPath
 *    );
 */
  }
} /* printEvent */


char * getEventType(int event) {
  switch (event) {
  case 1:
    return("exit");

  case 14:
    return("access");

  case 22:
    return("readlink");

  case 72:
    return("open/read");

  case 112:
    return("close");
  }
//    if (showAll) {
  return(getEventString(event));

//    }
  return("hide");
}


bool raiseAlertForFile(char *filePath) {
  if (filePath == NULL) {
    filePath = "";
  }
/*    if (fileFilter != NULL) {
 *      if (!strcasestr(filePath, fileFilter)) {
 *          return false;
 *      }
 *  }*/
  return(true);

  // FIXME:
  // Raise alert only if a process touch files in current user HOMEDIR
//    if (strncmp(filePath, HOMEDIR, strlen(HOMEDIR)) == 0) {
//        fprintf(stderr, "[D] %s is in %s\n", filePath, HOMEDIR);
  return(true);

//    }
  return(false);
}


bool raiseAlertForProcess(char *processPath) {
//    if (processFilter != NULL) {
//        if (!strcasestr(processPath, processFilter)) {
//           return false;
//      }
//  }
  return(true);

  // FIXME:
  // We need to check some white list and rules
  if (isWhiteList(processPath)) {
    return(false);
  }else if (isAllowedRule(processPath)) {
    return(false);
  }
  return(true);
}


char * getEventString(int event){
  char ev[15];

  sprintf(ev, "%d", event);
  FILE    *fp;
  char    *line = NULL;
  size_t  len   = 0;
  ssize_t read;

  fp = fopen(AUDIT_EVENT_FILE, "r");
  if (fp == NULL) {
    return("N/A");
  }

  while ((read = getline(&line, &len, fp)) != -1) {
    //Getting the line of the event.
    if (startsWith(ev, line)) {
      //Parsing the line to return only the event human-readable.
      //TODO: Make this ugly code here better!
      char *segment = strtok(line, ":");
      segment = strtok(0, ":");
      segment = strtok(0, ":");
      //Eliminating the content after "(" to return a nicer output
      char *p = strchr(segment, '(');
      if (!p) {
        fclose(fp);
        return(segment);
      }
      *p = 0;
      fclose(fp);
      return(segment);
    }
  }
  return("N/A");
}


bool startsWith(const char *pre, const char *str) {
  size_t lenpre = strlen(pre),
         lenstr = strlen(str);

  return(lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0);
}


char * getCurrentTimestamp() {
  time_t    timer;
  struct tm *tm_info;

  time(&timer);
  tm_info = localtime(&timer);
  strftime(timebuff, 26, "[%Y-%m-%d %H:%M:%S]", tm_info);
  return(timebuff);
}


char * getCurrentTime() {
  time_t    timer;
  struct tm *tm_info;

  time(&timer);
  tm_info = localtime(&timer);
  strftime(hourbuff, 26, "[%H:%M:%S]", tm_info);
  return(hourbuff);
}


int audit_main(){
  signal(SIGINT, shutDown);

  if (!isRoot()) {
    fprintf(stdout, "This software must be run as root.\n");
    return(1);
  }

  auditFile = initPipe();


  int i = 0;
  while (true) {
    i++;
    printEvent(getEvent(auditFile));
  }

  fclose(auditFile);
  fprintf(stdout, "Exiting..\n");
  return(0);
}
