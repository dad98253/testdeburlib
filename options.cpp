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
/*
#include "arch.h"
#include "misc.h"
#include "params.h"
#include "memory.h"
#include "list.h"
#include "loader.h"
#include "logger.h"
#include "status.h"
#include "recovery.h"
#include "options.h"
#include "bench.h"
#include "external.h"
#include "john.h"
#include "dynamic.h"
#include "unicode.h"
#include "fake_salts.h"
#include "path.h"
#include "regex.h"
#ifdef HAVE_MPI
#include "john-mpi.h"
#define _PER_NODE "per node "
#else
#define _PER_NODE ""
#endif
#ifdef HAVE_OPENCL
#include "common-opencl.h"
#endif
*/
#include "debug.h"    /* note: #ifdef DEBUG at top of debug.h causes
                       * nothing to be included for non-debug compiles*/
#include "debug01.h"
#include "getoptlib.h"
#include "options.h"

/*
#if HAVE_LIBGMP || HAVE_INT128 || HAVE___INT128 || HAVE___INT128_T
#include "prince.h"
#endif
#include "version.h"
#include "listconf.h" // must be included after version.h
#include "jumbo.h"
#include "memdbg.h"
*/
struct options_main options;

static struct opt_entry opt_list[] = {
//		{(char*)"", FLG_PASSWD, 0, 0, 0, OPT_FMT_ADD_LIST, &options.passwd},
	{(char*)"", FLG_PASSWD, 0, 0, 0, (char*)OPT_FMT_ADD_LIST, &options.passwd},

#if defined DEBUG
	{(char*)"debug-level", FLG_NONE, 0, 0, OPT_REQ_PARAM,
							(char*)"%d", &options.debug_level},
	{(char*)"debug-flags", FLG_NONE, 0, 0, OPT_REQ_PARAM,
			(char*)OPT_FMT_ADD_LIST_MULTI, &options.debug_flags},
	{(char*)"debug-server", FLG_NONE, 0, 0, OPT_REQ_PARAM,
			(char*)OPT_FMT_STR_ALLOC, &options.debug_server_name},
	{(char*)"debug-port", FLG_NONE, 0, 0, OPT_REQ_PARAM,
							(char*)"%d", &options.degug_port_number},
	{(char*)"debug-filename", FLG_NONE, 0, 0, OPT_REQ_PARAM,
			(char*)OPT_FMT_STR_ALLOC, &options.debug_filename},
	{(char*)"debug-device", FLG_NONE, 0, 0, OPT_REQ_PARAM,
			(char*)OPT_FMT_STR_ALLOC, &options.debug_device},
	{(char*)"debug-color", FLG_NONE, 0, 0, OPT_REQ_PARAM,
							(char*)"%d", &options.debug_color_flag},
	{(char*)"debug-clrscr", FLG_NONE, 0, 0, OPT_REQ_PARAM,
							(char*)"%d", &options.debug_clear_screen},
	{(char*)"debug-mask", FLG_NONE, 0, 0, OPT_REQ_PARAM,
			(char*)OPT_FMT_ADD_LIST_MULTI, &options.debug_mask_messages},
	{(char*)"debug-force", FLG_NONE, 0, 0, OPT_REQ_PARAM,
			(char*)OPT_FMT_ADD_LIST_MULTI, &options.debug_display_messages},
#endif

	{NULL}
};

#define JOHN_COPYRIGHT \
	"bla-bla-bla"

// the 2 DJ_DOS builds currently set this (and do not build the header). If other environs
// can not build the header, then they will also have this value set.
#ifdef NO_JOHN_BLD
#define JOHN_BLD "unk-build-type"
#else
//#include "john_build_rule.h"
#endif



#define JOHN_USAGE	  \
"test program for debuglib and getoptlib\n" \
"Copyright (c) 2018 " JOHN_COPYRIGHT "\n" \
"Usage: %s [OPTIONS] [FILES] bla-bla-bla\n" \
"--list=WHAT                list capabilities, see --list=help or doc/OPTIONS\n"

#ifdef DEBUG
#define JOHN_USAGE_DEBUG \
"--debug-level=N            set debug output verbosity (if =0, then no debug output,\n" \
"                           if =1, then minimum debug output, if =2, then verbose\n" \
"                           debug output, if =3, then very verbose debug output)\n" \
"--debug-flags=N[,..]       set debug flag(s), see --list=debug-flags\n" \
"--debug-server=SERVER      set debug ouput tcp server name or server ip address (used\n" \
"                           only for output device type TCPPORT)\n" \
"--debug-port=N             set debug ouput tcp port number (used only for output\n" \
"                           device type TCPPORT)\n" \
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
	puts("--help                     print usage summary, just like running the command");
	puts("                           without any parameters");
	puts("--config=FILE              use FILE instead of john.conf or john.ini");
	puts("--mem-file-size=SIZE       size threshold for wordlist preload (default 5 MB)");
	printf("--format=CLASS             valid classes: dynamic, cpu");
	printf("\n");
	printf("--verbosity=N              change verbosity (1-%u, default %u)\n",
	       VERB_MAX, VERB_DEFAULT);
	puts("--show=types               show some information about hashes in file (machine");
	puts("                           readable)");
	puts("--show=invalid             show any lines from input that are not valid for");
	puts("                           selected format(s)");
	puts("--skip-self-tests          skip self tests");
	puts("--test-full[=LEVEL]        run more thorough self-tests");
	puts("--stress-test[=TIME]       loop self tests forever");
#ifdef HAVE_FUZZ
	puts("--fuzz[=DICTFILE]          fuzz formats' prepare(), valid() and split()");
	puts("--fuzz-dump[=FROM,TO]      dump the fuzzed hashes between FROM and TO to file pwfile.format");
#endif
	puts("--input-encoding=NAME      input encoding (alias for --encoding)");
	puts("--internal-codepage=NAME   codepage used in rules/masks (see doc/ENCODING)");
	puts("--target-encoding=NAME     output encoding (used by format, see doc/ENCODING)");
#ifdef HAVE_OPENCL
	puts("--force-scalar             (OpenCL) force scalar mode");
	puts("--force-vector-width=N     (OpenCL) force vector width N");
#endif
#if HAVE_LIBGMP || HAVE_INT128 || HAVE___INT128 || HAVE___INT128_T
	puts("\nPRINCE mode options:");
	puts("--prince-loopback[=FILE]   fetch words from a .pot file");
	puts("--prince-elem-cnt-min=N    minimum number of elements per chain (1)");
	puts("--prince-elem-cnt-max=N    maximum number of elements per chain (8)");
	puts("--prince-skip=N            initial skip");
	puts("--prince-limit=N           limit number of candidates generated");
	puts("--prince-wl-dist-len       calculate length distribution from wordlist");
	puts("                           instead of using built-in table");
	puts("--prince-wl-max=N          load only N words from input wordlist");
	puts("--prince-case-permute      permute case of first letter");
	puts("--prince-mmap              memory-map infile (not available with case permute)");
	puts("--prince-keyspace          just show total keyspace that would be produced");
	puts("                           (disregarding skip and limit)");
#endif
	puts("");
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

#if defined DEBUG
	list_init(&options.debug_flags);
	list_init(&options.debug_mask_messages);
	list_init(&options.debug_display_messages);

#endif


#ifdef DEBUG
	if ( !options.debug_level &&
	    ( options.debug_flags->count == 0) ) {
		list_add(options.debug_flags, (char*)"none");
	}
#endif


	opt_process(opt_list, &options.flags, argv);

	opt_check(opt_list, options.flags, argv);


//	rec_argc = argc; rec_argv = argv;
//	rec_check = 0;
}
