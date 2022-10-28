#pragma once
#ifndef OPTPARSE_C
#define OPTPARSE_C
#define LOCAL_DEBUG_MODE    OPTPARSE_DEBUG_MODE
////////////////////////////////////////////
#include "optparse/optparse.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"

////////////////////////////////////////////
static bool OPTPARSE_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__optparse(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_optparse") != NULL) {
    log_debug("Enabling optparse Debug Mode");
    OPTPARSE_DEBUG_MODE = true;
  }
}

////////////////////////////////////////////
hash_t *get_subcommands_hash(struct optparse_cmd *cmd){
  hash_t *h  = hash_new();
  size_t qty = get_subcommands_qty(cmd);

  for (size_t i = 0; i < qty; i++)
    hash_set(h, cmd->subcommands[i].name, (void *)&(cmd->subcommands[i]));
  return(h);
}

size_t get_subcommands_qty(struct optparse_cmd *cmd){
  size_t q = 0;

  if (cmd->subcommands)
    for (int i = 0; i < MAX_SUBCOMMANDS && cmd->subcommands[i].name; i++)
      q++;
  return(q);
}

hash_t *get_command_strings(struct optparse_cmd *cmd){
  hash_t              *h = hash_new();
  struct optparse_cmd *c, *sc;
  char                *s;

  hash_each(get_subcommands_hash(cmd), {
    c = (struct optparse_cmd *)val;
    hash_each(get_subcommands_hash(c), {
      sc = (struct optparse_cmd *)val;
      asprintf(&s, "%s %s", c->name, sc->name);
      hash_set(h, strdup(s), (void *)val);
    });
    hash_each(get_command_strings(c), {
      asprintf(&s, "%s %s", c->name, key);
      hash_set(h, strdup(s), (void *)val);
    })
  });
  return(h);
}

void print_subcommands(struct optparse_cmd *cmd, int depth){
  struct optparse_cmd *c;
  char                *depth_chars = "";

  while (strlen(depth_chars) < depth)
    asprintf(&depth_chars, "%s ", depth_chars);
  hash_each(get_subcommands_hash(cmd), {
    c = (struct optparse_cmd *)val;
    fprintf(stdout, "%s- %s\n",
            depth_chars,
            c->name
            );
    print_subcommands(c, depth + 1);
  });
}

#undef LOCAL_DEBUG_MODE
#endif
