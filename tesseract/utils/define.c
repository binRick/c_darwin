#pragma once
#ifndef TESSERACT_UTILS_DEFINE_C
#define TESSERACT_UTILS_DEFINE_C

#define MINIMIZE_PREFERENCES    false

#define DEBUG_MOUSE_LOCATION()    { do {                           \
                                      p = get_mouse_location();    \
                                      log_info("Mouse is at %dx%d" \
                                               "%s",               \
                                               (int)p->x,          \
                                               (int)p->y,          \
                                               ""                  \
                                               );                  \
                                    } while (0); }
#endif
