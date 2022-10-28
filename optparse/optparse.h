#pragma once
#ifndef OPTPARSE_H
#define OPTPARSE_H
#define MAX_SUBCOMMANDS    1024
//////////////////////////////////////
#include "core/core.h"
#include "hash/hash.h"
#include "optparse/optparse.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
size_t get_subcommands_qty();
hash_t *get_subcommands_hash(struct optparse_cmd *cmd);
size_t get_subcommands_qty(struct optparse_cmd *cmd);
hash_t *get_command_strings(struct optparse_cmd *cmd);
void print_subcommands(struct optparse_cmd *cmd, int depth);
#endif
