#pragma once
#ifndef LS_WIN_HTTPSERVER_INCBIN_C
#define LS_WIN_HTTPSERVER_INCBIN_C
#include "darwin-ls/darwin-ls-httpserver.h"
#define INCBIN_SILENCE_BITCODE_WARNING
#define INCBIN_STYLE     INCBIN_STYLE_SNAKE
#define INCBIN_PREFIX    inc_
#include "incbin/incbin.h"
INCBIN(favicon, INCLUDE_FAVICON_FILE);
INCBIN(errorpng, INCLUDE_ERROR_IMAGE);
INCBIN(main_css, INCLUDE_MAIN_CSS);
INCBIN(main_js, INCLUDE_MAIN_JS);
INCBIN(jquery_min_js, INCLUDE_JQUERY_MIN_JS);
INCBIN(reveal_js, INCLUDE_REVEAL_DIST_REVEAL_JS);
INCBIN(reveal_js_map, INCLUDE_REVEAL_DIST_REVEAL_JS_MAP);
INCBIN(reset_css, INCLUDE_REVEAL_DIST_RESET_CSS);
INCBIN(reveal_css, INCLUDE_REVEAL_DIST_REVEAL_CSS);
INCBIN(reveal_theme_white_css, INCLUDE_REVEAL_DIST_THEME_WHITE_CSS);
INCBIN(reveal_monokai_css, INCLUDE_REVEAL_DIST_MONOKAI_CSS);
INCBIN(reveal_highlight_js, INCLUDE_REVEAL_DIST_HIGHLIGHT_JS);
INCBIN(reveal_markdown_js, INCLUDE_REVEAL_DIST_MARKDOWN_JS);
INCBIN(reveal_math_js, INCLUDE_REVEAL_DIST_MATH_JS);
INCBIN(reveal_multiple_presentations_html, INCLUDE_REVEAL_MULTIPLE_PRESENTATIONS_HTML);
INCBIN(reveal_reveal_source_space_pro_css, INCLUDE_REVEAL_SOURCE_SANS_PRO_CSS);
INCBIN(reveal_reveal_source_space_pro_regular_ttf, INCLUDE_REVEAL_SOURCE_SANS_PRO_REGULAR_TTF);
INCBIN(reveal_reveal_source_space_pro_regular_woff, INCLUDE_REVEAL_SOURCE_SANS_PRO_REGULAR_WOFF);
INCBIN(reveal_reveal_source_space_pro_italic_ttf, INCLUDE_REVEAL_SOURCE_SANS_PRO_ITALIC_TTF);
INCBIN(reveal_reveal_source_space_pro_italic_woff, INCLUDE_REVEAL_SOURCE_SANS_PRO_ITALIC_WOFF);

#endif
