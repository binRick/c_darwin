#pragma once
#include "core/core.h"
#include "dls/dls-bestline.h"
static char       *BESTLINE_PROMPTS[] = {
  "▶",
  " → ",
  "↪",
  "",
  "",
  "⬢",
  "🌞",
  "⭐",
  "👉",
  "🔅",
  "👉",
  "🐧",
  NULL,
};
static const char *BESTLINE_PREFIXES[] = {
  ">",
  "~",
  "➤",
  "$",
  "○",
  "<",
  NULL,
};
static const char *BESTLINE_CURSORS[] = {
  AC_CURSOR_BLOCK,
  AC_CURSOR_BAR,
  AC_CURSOR_UNDER,
  NULL,
};
static const char *BESTLINE_PREFIX_COLORS[] = { AC_RED, AC_GREEN, AC_YELLOW, AC_BLUE, };
static const char *BESTLINE_PROMPT_COLORS[] = { AC_RED, AC_GREEN, AC_YELLOW, AC_BLUE, };
static const char *BESTLINE_CURSOR_COLORS[] = {
  "green",
  "yellow",
  "black",
  "red",
  NULL,
};
