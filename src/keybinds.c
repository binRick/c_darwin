/**********************************/
#include "../include/includes.h"

/**********************************/
list_t *keystates;

/**********************************/


int iterate_profiles(){
  list_node_t     *node;
  list_iterator_t *it = list_iterator_new(keystates, LIST_HEAD);
  int             i   = 0;

  while ((node = list_iterator_next(it))) {
    struct KeyState *KS = (struct KeyState *)node->val;
    i++;
  }
  list_iterator_destroy(it);
  return(i);
}

struct KeyState new_default_ks(){
  struct  KeyState *P0 = malloc(sizeof(struct KeyState));

  P0->name = "unknown";
  P0->code = 0;
  P0->ts   = 0;
  return(*P0);
}


struct KeyState new_ks(const char *name, int code){
  struct   KeyState P0 = new_default_ks();

  P0.ts   = (unsigned long)timestamp();
  P0.name = (char *)name;
  P0.code = (int)code;
  return(P0);
}


int handle_key(const int keycode, const char *key){
  char            *f    = "[";
  char            *r    = "";
  char            *nkey = strdup(key);

  struct KeyState K = new_ks(key, keycode);

  list_rpush(keystates, list_node_new(&K));
  // int qty = iterate_profiles();
//  log_debug("qty:%d", qty);
  //strcpy(&nkey, strdup(key));
//  stringfn_mut_replace(nkey, f, r);
//  log_trace("handling %d|%s => %s", keycode, key, nkey);
  return(0);
}
