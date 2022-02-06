/* ************************************************************************
*   File: config.c                                      Part of CircleMUD *
*  Usage: Configuration of various aspects of CircleMUD operation         *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define __CONFIG_C__

#include "conf.h"

#if !defined (CIRCLE_WINDOWS)
#ifndef FALSE
#ifndef TRUE
const int TRUE = 1;
const int FALSE = 0;
#endif
#endif
#endif
const int YES = 1;
const int NO = 0; 

float SHOUT_HEALTH_PERCENT = 0.10f;
int max_exp_gain = 500000;	/* max gainable per kill */
int max_exp_loss = 1000000;	/* max losable per death */

const char *OK = "Okay.\r\n";
const char *NOPERSON = "No-one by that name here.\r\n";
const char *NOEFFECT = "Nothing seems to happen.\r\n";

int MAX_PLAYERS = 300;
int max_bad_pws = 3;
