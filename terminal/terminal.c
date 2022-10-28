#pragma once
#ifndef TERMINAL_C
#define TERMINAL_C
////////////////////////////////////////////
/*
 * static struct terminal_type_names_t { const char *name, *env_key, *env_val; } terminal_type_names[] = {
 *  [TERMINAL_TYPE_KITTY]     = { .name = "kitty",     .env_key = "KITTY_PID",        .env_val = NULL,    },
 *  [TERMINAL_TYPE_ALACRITTY] = { .name = "alacritty", .env_key = "ALACRITTY_SOCKET", .env_val = NULL,    },
 *  [TERMINAL_TYPE_ITERM2]    = { .name = "iterm2",    .env_key = "TERM_PROGRAM",     .env_val = "iTerm.app",    },
 *  [TERMINAL_TYPE_TERMINAL]  = { .name = "terminal",  .env_key = "TERM_PROGRAM",     .env_val = "Apple_Terminal"  , },
 *                              { 0 },
 * };
 * static struct kitty_cmd_items_t                                               kitty_cmd_items[] = {
 *  { .desc = "Set Window Title",       .key = "window-title",       .cmd = "@set-window-title",       .valid_val  ue_regex = ".*",           .types = KITTY_CMD_TYPE_AT, },
 *  { .desc = "Set Tab Title",          .key = "tab-title",          .cmd = "@set-tab-title",          .valid_val  ue_regex = ".*",           .types = KITTY_CMD_TYPE_AT & KITTY_CMD_TYPE_SOCKET },
 *  { .desc = "Set Font Size",          .key = "font-size",          .cmd = "@set-font-size",          .valid_val  ue_regex = "*[0-9][0-9]$", .types = KITTY_CMD_TYPE_AT, },
 *  { .desc = "Set Tab Color",          .key = "tab-color",          .cmd = "@set-tab-color",          .valid_val  ue_regex = "*#",           .types = KITTY_CMD_TYPE_AT, },
 *  { .desc = "Set Background Opacity", .key = "background-opacity", .cmd = "@set-background-opacity", .valid_val  ue_regex = "*[0-9]",       .types = KITTY_CMD_TYPE_AT, },
 *  { .desc = "Send Text",              .key = "send-text",          .cmd = "@send-text",              .valid_val  ue_regex = ".*",           .types = KITTY_CMD_TYPE_AT, },
 *  { 0 },
 * };
 */
////////////////////////////////////////////
#include "terminal/terminal.h"
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
static bool TERMINAL_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__terminal(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_TERMINAL") != NULL) {
    log_debug("Enabling terminal Debug Mode");
    TERMINAL_DEBUG_MODE = true;
  }
}
static void terminal_internal_fxn1(void);

////////////////////////////////////////////
static void terminal_internal_fxn1(void){
  log_info("Called function terminal_internal_fxn1");
  if (TERMINAL_DEBUG_MODE == true)
    log_info("Debug Mode Enabled");
}

////////////////////////////////////////////
void terminal_fxn1(void){
  terminal_internal_fxn1();
  log_info("Called function terminal_fxn1");
  if (TERMINAL_DEBUG_MODE == true)
    log_info("Debug Mode Enabled");
}
////////////////////////////////////////////
#endif
