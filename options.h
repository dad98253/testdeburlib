/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98,2003,2006,2013 by Solar Designer
 *
 * ...with changes in the jumbo patch, by JimF and magnum (and various others?)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There's ABSOLUTELY NO WARRANTY, express or implied.
 */

/*
 * John's command line options definition.
 */

#ifndef _JOHN_OPTIONS_H
#define _JOHN_OPTIONS_H

#ifndef OPTIONSDEFS
#define EXTERN		extern
#define INITIZERO
#define INITSZERO
#define INITBOOLFALSE
#define INITBOOLTRUE
#define INITNULL
#define INITBUFFERSIZE
#define INITDIRSIZE
#else  // MAINDEFS
#define EXTERN
#define INITIZERO	=0
#define INITSZERO	={0}
#define INITBOOLFALSE	=false
#define INITBOOLTRUE	=true
#define INITNULL	=NULL
#define INITBUFFERSIZE	=BUFFERSIZE
#define INITDIRSIZE		=DIRSIZE
#endif  // MAINDEFS

#if AC_BUILT
#include "autoconfig.h"
#endif

#include "list.h"
//#include "loader.h"
//#include "getopt.h"

/*
 * Core Option flags bitmasks (low 32 bits):
 */
/* An action requested */
#define FLG_ACTION			0x00000001
/* Password files specified */
#define FLG_PASSWD			0x00000002
/* An option supports password files */
#define FLG_PWD_SUP			0x00000004
/* An option requires password files */
#define FLG_PWD_REQ			(0x00000008 | FLG_PWD_SUP)
/* Some option that doesn't have its own flag is specified */
#define FLG_NONE			0x00000010

#define FLG_ZERO			0x0

/* debug flags, options.debug_flags[...] is set to the flag value */
#define DBG_TRACE			0x1000000000000000ULL
/* debug levels, options.debug_level is set to the level value, or 0
 * (off) */
#define DBG_UNINIT			-1
#define DBG_NONE			0
#define DBG_MINIMUM			1
#define DBG_VERBOSE			2
#define DBG_VERBOSEVERBOSE		3
#define DBG_VERBOSEVERBOSEVERBOSE	4

/* Verbosity level. Higher is more chatty. */
#define VERB_MAX                        5
#define VERB_LEGACY                     4
#define VERB_DEFAULT                    3



/*
 * Structure with option flags and all the parameters.
 */

struct options_main {
// Option flags
//	opt_flags flags;

	// Password files
	struct list_main *passwd;

#ifdef DEBUG
// debug options
	unsigned int debug_level;
	struct list_main *debug_flags;
	char * debug_server_name;
	unsigned int degug_port_number;
	char * debug_filename;
	char * debug_device;
	unsigned int debug_color_flag;
	unsigned int debug_clear_screen;
	struct list_main *debug_mask_messages;
	struct list_main *debug_display_messages;
#endif

/* -list=WHAT Get a config list (eg. a list of incremental modes available) */
	char *listconf;
};

EXTERN struct options_main options;


/*
 * Initializes the options structure.
 */
EXTERN void opt_init(char *name, int argc, char **argv, int show_usage);

/*
 * Prints the "hidden" options usage
 */
EXTERN void opt_print_hidden_usage(void);


#undef EXTERN
#undef INITIZERO
#undef INITSZERO
#undef INITBOOLFALSE
#undef INITBOOLTRUE
#undef INITNULL
#undef INITBUFFERSIZE
#undef INITDIRSIZE


#endif
