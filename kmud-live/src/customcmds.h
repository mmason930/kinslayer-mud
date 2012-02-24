/* ************************************************************************
*   File: customcmds.h                                  Part of CircleMUD *
*  Usage: header file for custom commands                                 *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/* Used for Command: gcomm
 * Level: Immortal+
 *
 * Notes: Modeled after COLOR code in screen.h
 */
const int GCOMM_OFF = 0; 
const int GCOMM_COLORS = 1; 
const int GCOMM_FLAGS = 2; 
const int GCOMM_ALL = 3; 
#define _gcommlevel(ch) ((PRF_FLAGGED((ch), PRF_GCOMM_1) ? 1 : 0) + \
		       (PRF_FLAGGED((ch), PRF_GCOMM_2) ? 2 : 0))

#define GCOMM_LEV(ch) (_gcommlevel(ch))
