#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
////////////////////////////////////////////////////////
#include "c_ansi/ansi-codes/ansi-codes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_stringfn/include/stringfn.h"
#include "c_timer/include/c_timer.h"
#include "c_vector/include/vector.h"
#include "cargs.h"
#include "ls-proc-args.h"
#include "ls-proc-args.h"
#include "ls-proc.h"
#include "process/process.h"
#include "socket99/socket99.h"
#include "str-flatten.c/src/str-flatten.h"
#include "timestamp/timestamp.h"
#include "tiny-regex-c/re.h"
#include "wildcardcmp/wildcardcmp.h"
////////////////////////////////////////////////////////

bool match_item(char *MATCH_WILDCARD, char *ITEM);
bool match_item_v(struct Vector *MATCH_WILDCARD_V, char *ITEM);
int ls_procs();
