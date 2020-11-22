/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-2016 by Solar Designer
 *
 * ...with changes in the jumbo patch, by JimF and magnum (and various others?)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There's ABSOLUTELY NO WARRANTY, express or implied.
 */

#define NEED_OS_FORK
#undef _GNU_SOURCE
#define _GNU_SOURCE 1 /* for strcasestr in legacy builds */

//#include "os.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "debug.h"    /* note: #ifdef DEBUG at top of debug.h causes
                       * nothing to be included for non-debug compiles*/
#include "debug01.h"
#include "getoptlib.h"
#define OPTIONSDEFS
#include "options.h"


extern DEBUGOPTIONSTYPE myoptions;

static struct opt_entry opt_list[] = {
//		{(char*)"", FLG_PASSWD, 0, 0, 0, OPT_FMT_ADD_LIST, &myoptions.passwd},
	{(char*)"", FLG_PASSWD, 0, 0, 0, (char*)OPT_FMT_ADD_LIST, &myoptions.passwd},
	{(char*)"list", FLG_ZERO, 0, 0, OPT_REQ_PARAM,
			(char*)OPT_FMT_STR_ALLOC, &options.listconf},

#if defined DEBUG
	{(char*)"debug-level", FLG_NONE, 0, 0, OPT_REQ_PARAM,
							(char*)"%d", &myoptions.debug_level},
	{(char*)"debug-flags", FLG_NONE, 0, 0, OPT_REQ_PARAM,
			(char*)OPT_FMT_ADD_LIST_MULTI, &myoptions.debug_flags},
	{(char*)"debug-server", FLG_NONE, 0, 0, OPT_REQ_PARAM,
			(char*)OPT_FMT_STR_ALLOC, &myoptions.debug_server_name},
	{(char*)"debug-port", FLG_NONE, 0, 0, OPT_REQ_PARAM,
							(char*)"%d", &myoptions.degug_port_number},
	{(char*)"debug-filename", FLG_NONE, 0, 0, OPT_REQ_PARAM,
			(char*)OPT_FMT_STR_ALLOC, &myoptions.debug_filename},
	{(char*)"debug-device", FLG_NONE, 0, 0, OPT_REQ_PARAM,
			(char*)OPT_FMT_STR_ALLOC, &myoptions.debug_device},
	{(char*)"debug-color", FLG_NONE, 0, 0, OPT_REQ_PARAM,
							(char*)"%d", &myoptions.debug_color_flag},
	{(char*)"debug-clrscr", FLG_NONE, 0, 0, OPT_REQ_PARAM,
							(char*)"%d", &myoptions.debug_clear_screen},
	{(char*)"debug-mask", FLG_NONE, 0, 0, OPT_REQ_PARAM,
			(char*)OPT_FMT_ADD_LIST_MULTI, &myoptions.debug_mask_messages},
	{(char*)"debug-force", FLG_NONE, 0, 0, OPT_REQ_PARAM,
			(char*)OPT_FMT_ADD_LIST_MULTI, &myoptions.debug_display_messages},
#endif

	{NULL}
};

#define JOHN_COPYRIGHT \
	"John Kuras"

// the 2 DJ_DOS builds currently set this (and do not build the header). If other environs
// can not build the header, then they will also have this value set.
#ifdef NO_JOHN_BLD
#define JOHN_BLD "unk-build-type"
#else
//#include "john_build_rule.h"
#endif



#define JOHN_USAGE	  \
"test program for debuglib and getoptlib\n" \
"Copyright (c) 2018-2020 " JOHN_COPYRIGHT "\n" \
"Usage: %s [OPTIONS]\n" \
"--help                     print usage summary\n" \
"--list=WHAT                list capabilities, see --list=help\n" \
"--data-folder=PATH         use PATH instead of ~\\Application Data for the\n" \
"                           database folder\n" \
"--gui-mode                 use gui mode (default)\n" \
"--input-file-name=NAME     input file name (used by text mode)\n" \
"--master-password=NAME     database password (used in brief mode)\n" \
"--recover                  use recover mode\n" \
"--system-name=NAME         system name (used in brief mode)\n" \
"--text-mode                use text mode\n"

#ifdef DEBUG
#define JOHN_USAGE_DEBUG \
"--debug-level=N            set debug output verbosity (if =0, then no debug output,\n" \
"                           if =1, then minimum debug output, if =2, then verbose\n" \
"                           debug output, if =3, then very verbose debug output)\n" \
"--debug-flags=N[,..]       set debug flag(s), see --list=debug-flags\n" \
"--debug-server=SERVER      set debug ouput tcp server name or server ip address (used\n" \
"                           only for output device types UDPPORT and TCPPORT), default\n" \
"                           is localhost\n" \
"--debug-port=N             set debug ouput udp/tcp port number (used for output\n" \
"                           device types UDPPORT and TCPPORT), default is 5001\n" \
"--debug-filename=FILE      set debug output file name (used only for output device\n" \
"                           type FILEOUTPUT)\n" \
"--debug-device=DEVICE      set debug output device, see --list=debug-device\n" \
"--debug-color={0,1}        set color output debug option (on=1, off=0)\n" \
"--debug-clrscr={0,1}       set clear debug screen at start of run option (on=1, off=0)\n" \
"--debug-mask=N[,..]        set debug mask(s) to ignore certain messages, see\n" \
"                           --list=debug-mask\n" \
"--debug-force=N[,..]       set debug force(s) to display messages that would otherwise\n" \
"                           be ignored (see --list=debug-mask)\n"
#endif

static void print_usage(char *name)
{
//	if (!john_main_process)
//		exit(0);

	printf(JOHN_USAGE, name);
#ifdef DEBUG
	printf("%s", JOHN_USAGE_DEBUG);
#endif
	exit(0);
}

void opt_print_hidden_usage(void)
{
	puts("");
	puts("note: use of both --master-password and --system-name invokes a special \"brief\"");
	puts("      mode unless overriden by --text-mode or --gui-mode options\n");
}

#ifdef DEBUG
void debug_flag_list(void)
{
	int i;
	i=0;
	while (strlen(debug_flag[i]) != 0) {
		if(i != 0) printf(" %s\n",debug_flag[i]);
		i++;
	}
	printf("\n");
	return;
}

void debug_device_list(void)
{
	int i;
	for (i = 0 ; i < DIMDEBUGDEVICES ; i++) {
		printf(" %s\n",debug_devices[i]);
	}
	printf("\n");
	return;
}

void debug_mask_list(void)
{
	printf("(none - option not used in this version of the program)\n");
	return;
}

#endif   //   fi def DEBUG

void opt_init(char *name, int argc, char **argv, int show_usage)
{
	if (show_usage)
		print_usage(name);


	memset(&myoptions, 0, sizeof(myoptions));

//	options.loader.field_sep_char = ':';
//	options.max_wordfile_memory = WORDLIST_BUFFER_DEFAULT;
//	options.req_minlength = -1;

//	if (!options.verbosity)
//		options.verbosity = VERB_DEFAULT;


#if defined DEBUG
	list_init(&myoptions.debug_flags);
	list_init(&myoptions.debug_mask_messages);
	list_init(&myoptions.debug_display_messages);

#endif


#ifdef DEBUG
	if ( !myoptions.debug_level &&
	    ( myoptions.debug_flags->count == 0) ) {
		list_add(myoptions.debug_flags, (char*)"none");
	}
#endif


	opt_process(opt_list, &myoptions.flags, argv);

	opt_check(opt_list, myoptions.flags, argv);


//	rec_argc = argc; rec_argv = argv;
//	rec_check = 0;
}
