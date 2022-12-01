#pragma once
#ifndef DLS_H
#define DLS_H
//////////////////////////////////////
#include "dls/defines.h"
//////////////////////////////////////
#include "core/includes.h"
#include "dls/commands.h"
//////////////////////////////////////
#include "dls/types.h"
//////////////////////////////////////
#include "c_eventemitter/include/eventemitter.h"
#include "core/core.h"
#include "dls/command-capture.h"
#include "dls/command-keylogger.h"
#include "dls/command-test.h"
#include "dls/commands.h"
#include "dls/dls-bestline.h"
#include "dls/vars.h"
#include "murmurhash.c/murmurhash.h"
#include "optparse/optparse.h"
#include "tp/tp.h"
//////////////////////////////////////
int handle_main(int argc, char *argv[]);
#endif
