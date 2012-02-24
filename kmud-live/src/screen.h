/* ************************************************************************
*   File: screen.h                                      Part of CircleMUD *
*  Usage: header file with ANSI color codes for online color              *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define NORMAL		"\x1B[0m"
#define BOLD		"\x1B[1m"
#define RED			"\x1B[31m"
#define GREEN		"\x1B[32m"
#define YELLOW		"\x1B[33m"
#define BLUE		"\x1B[34m"
#define MAGENTA		"\x1B[35m"
#define CYAN		"\x1B[36m"
#define WHITE		"\x1B[37m"
#define NULL_COLOR	""



/* conditional color.  pass it a pointer to a Character and a color level. */
const int CL_OFF = 0; 
const int CL_SPARSE = 1; 
const int CL_NORMAL = 2; 
const int CL_COMPLETE = 3; 
#define _clrlevel(ch) ((PRF_FLAGGED((ch), PRF_COLOR_1) ? 1 : 0) + \
		       (PRF_FLAGGED((ch), PRF_COLOR_2) ? 2 : 0))
#define clr(ch,lvl) (_clrlevel(ch) >= (lvl))

#define COLOR_NORMAL(ch,lvl)	(clr((ch),(lvl))?NORMAL:NULL_COLOR)
#define COLOR_BOLD(ch,lvl)		(clr((ch),(lvl))?BOLD:NULL_COLOR)
#define COLOR_RED(ch,lvl)		(clr((ch),(lvl))?RED:NULL_COLOR)
#define COLOR_GREEN(ch,lvl)		(clr((ch),(lvl))?GREEN:NULL_COLOR)
#define COLOR_YELLOW(ch,lvl)	(clr((ch),(lvl))?YELLOW:NULL_COLOR)
#define COLOR_BLUE(ch,lvl)		(clr((ch),(lvl))?BLUE:NULL_COLOR)
#define COLOR_MAGENTA(ch,lvl)	(clr((ch),(lvl))?MAGENTA:NULL_COLOR)
#define COLOR_CYAN(ch,lvl)		(clr((ch),(lvl))?CYAN:NULL_COLOR)
#define COLOR_WHITE(ch,lvl)		(clr((ch),(lvl))?WHITE:NULL_COLOR)

#define COLOR_LEV(ch) (_clrlevel(ch))

#define QNRM CCNRM(ch,C_SPR)
#define QRED CCRED(ch,C_SPR)
#define QGRN CCGRN(ch,C_SPR)
#define QYEL CCYEL(ch,C_SPR)
#define QBLU CCBLU(ch,C_SPR)
#define QMAG CCMAG(ch,C_SPR)
#define QCYN CCCYN(ch,C_SPR)
#define QWHT CCWHT(ch,C_SPR)
















