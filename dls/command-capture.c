#pragma once
#ifndef DLS_CAPTURE_COMMANDS_C
#define DLS_CAPTURE_COMMANDS_C
#include "core/core.h"
#include "dls/command-capture.h"

#define NewCaptureSpace(ID) \
  ((CommandCapture)(DYN_LIT(CaptureSpace, CommandCapture, { .req.index = ID })))
#define NewCaptureDisplay(ID) \
  ((CommandCapture)(DYN_LIT(CaptureDisplay, CommandCapture, { .req.index = ID })))
#define NewCaptureWindow(ID) \
  ((CommandCapture)(DYN_LIT(CaptureWindow, CommandCapture, { .req.id = ID })))

void _command_capture_dev(){
  log_info("Capturing dev");
  CommandCapture w = NewCaptureWindow(666);
  CommandCapture d = NewCaptureDisplay(1);
  CommandCapture s = NewCaptureSpace(1);
  log_info("capturing id #%d", VCALL_OBJ(w, id));
  log_info("capturing id #%d", VCALL_OBJ(s, id));
  log_info("capturing id #%d", VCALL_OBJ(d, id));
  exit(EXIT_SUCCESS);
}

bool CaptureDisplay_capture(const VSelf){
  VSELF(CaptureDisplay);
  log_info("Capturing display %d", self->req.id);
  return(true);
}

int CommandCapture_index(CommandCapture cc) {
  int id = 0;

  printf("cc> Turning on ...%d\n", id);
  return(id);
}

int CommandCapture_id(CommandCapture cc) {
  int id = 0;

  printf("cc> Turning on ...%d\n", id);
  return(id);
}

bool CaptureSpace_capture(const VSelf){
  VSELF(CaptureSpace);
  log_info("Capturing Space %d", self->req.id);
  return(true);
}

bool CaptureWindow_capture(const VSelf){
  VSELF(CaptureWindow);
  log_info("Capturing window %d", self->req.id);
  return(true);
}

int CaptureWindow_id(const VSelf){
  VSELF(CaptureWindow);
  return(self->req.id);
}

int CaptureSpace_id(CommandCapture cc){
  int id = CommandCapture_id(cc);

  id++;
  log_info("special case space: %d", id);
  return(id);
}
#define CaptureSpace_id_CUSTOM    ()
implExtern(CommandCapture, CaptureSpace);

int CaptureDisplay_id(CommandCapture cc){
  int id = 123;

  log_info("special case display: %d", id);
  return(id);
}
#define CaptureDisplay_id_CUSTOM    ()
implExtern(CommandCapture, CaptureDisplay);

implExtern(CommandCapture, CaptureWindow);
#endif
