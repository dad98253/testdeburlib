//
// Copyright (c) 2012, 2013 Frank Dittrich, JimF and magnum
//
// This software is hereby released to the general public under the following
// terms:  Redistribution and use in source and binary forms, with or without
// modification, are permitted.
//

#if AC_BUILT
/* need to know if HAVE_LIBGMP is set, for autoconfig build */
#include "autoconfig.h"
#endif

#if !AC_BUILT
# include <string.h>
# ifndef _MSC_VER
#  include <strings.h>
# endif
#else
# if STRING_WITH_STRINGS
#  include <string.h>
#  include <strings.h>
# elif HAVE_STRING_H
#  include <string.h>
# elif HAVE_STRINGS_H
#  include <strings.h>
# endif
#endif

#if __GLIBC__
#include <gnu/libc-version.h>
#endif

#include "options.h"


#ifdef DEBUG
extern void debug_flag_list();
extern void debug_device_list();
extern void debug_mask_list();
#endif

//#include "version.h"
#define LISTCONFDEFS
#include "listconf.h" /* must be included after version.h */
#include "memdbg.h"

//
// FIXME: Should all the listconf_list_*() functions get an additional stream
// parameter, so that they can write to stderr instead of stdout in case fo an
// error?
//
static void listconf_list_options()
{
	puts("help[:WHAT], build-info, hidden-options");

	// NOTE: The following must end the list. Anything listed after
	//   <conf section name> will be ignored by current
	//   bash completion scripts.

	// FIXME: Should all the section names get printed instead?
	//        But that would require a valid config.
	//
//	puts("<conf section name>");
	return;
}

static void listconf_list_help_options()
{
	puts("help, format-methods, parameters, list-data");
	return;
}

static void listconf_list_method_names()
{
	return;
}

static void listconf_list_build_info(void)
{
	char DebuggingOptions[512], *cpdbg=DebuggingOptions;

	puts("Version: " JTR_GIT_VERSION);
	puts("Build: " JOHN_BLD DEBUG_STRING MEMDBG_STRING  );

// output some compiler version stuff
#if __ICC
	printf("icc version: %d.%d.%d (gcc %d.%d.%d compatibility)\n",
	       __ICC / 100, (__ICC % 100) / 10, __ICC % 10,
	       __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(__clang_version__)
	printf("clang version: %s (gcc %d.%d.%d compatibility)\n",
	       __clang_version__,
	       __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif __GNUC__
	printf("gcc version: %d.%d.%d\n", __GNUC__,
	       __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif _MSC_VER
//
// See https://msdn.microsoft.com/en-us/library/b0084kay.aspx
// Currently, _MSC_BUILD is not reported, but we could convert
// _MSC_FULL_VER 150020706 and _MSC_BUILD 1 into a string
// "15.00.20706.01".
//
#ifdef _MSC_FULL_VER
	printf("Microsoft Visual C version: %d, build %d\n", _MSC_VER, _MSC_FULL_VER);
#else
	printf("Microsoft compiler version: %d\n", _MSC_VER);
#endif
#ifdef __CLR_VER
	puts("Common Language Runtime version: " __CLR_VER);
#endif
#elif defined(__VERSION__)
	printf("Compiler version: %s\n", __VERSION__);
#endif

#ifdef __GLIBC_MINOR__
#ifdef __GLIBC__
	printf("GNU libc version: %d.%d (loaded: %s)\n",
	       __GLIBC__, __GLIBC_MINOR__, gnu_get_libc_version());
#endif
#endif
	printf("Compile date and time: %s %s\n", __DATE__, __TIME__);
	// OK, now append debugging options, BUT only output  something if
	// one or more of them is set. IF none set, be silent.

	#if defined (DEBUG)
		cpdbg += sprintf(cpdbg, "\t'#define DEBUG' set\n");
	#endif
		if (DebuggingOptions != cpdbg) {
			printf("Built with these debugging options\n%s\n", DebuggingOptions);
		}
	return;
}

void listconf_parse_early(void)
{
//
// --list=? needs to be supported, because it has been supported in the released
// john-1.7.9-jumbo-6 version, and it is used by the bash completion script.
// --list=? is, however, not longer mentioned in doc/OPTIONS and in the usage
// output. Instead, --list=help is.
//
	if ((!strcasecmp(options.listconf, "help") ||
	                         !strcmp(options.listconf, "?"))) {
		listconf_list_options();
		exit(EXIT_SUCCESS);
	}

	if ((!strcasecmp(options.listconf, "help:help") ||
	                         !strcasecmp(options.listconf, "help:"))) {
		listconf_list_help_options();
		exit(EXIT_SUCCESS);
	}

	if (!strcasecmp(options.listconf, "help:format-methods"))
	{
		listconf_list_method_names();
		exit(EXIT_SUCCESS);
	}

	if (!strncasecmp(options.listconf, "help:", 5))
	{
		if (strcasecmp(options.listconf, "help:parameters") &&
		    strcasecmp(options.listconf, "help:list-data"))
		{
			fprintf(stderr,
			        "%s is not a --list option that supports additional values.\nSupported options:\n",
			        options.listconf+5);
			listconf_list_help_options();
			exit(EXIT_FAILURE);
		}
	}

	if (!strcasecmp(options.listconf, "hidden-options"))
	{
		opt_print_hidden_usage();
		exit(EXIT_SUCCESS);
	}

	if (!strcasecmp(options.listconf, "build-info"))
	{
		listconf_list_build_info();
		exit(EXIT_SUCCESS);
	}

#ifdef DEBUG
	if (!strcasecmp(options.listconf, "debug-flags"))
	{
		debug_flag_list();
		exit(EXIT_SUCCESS);
	}
	if (!strcasecmp(options.listconf, "debug-device"))
	{
		debug_device_list();
		exit(EXIT_SUCCESS);
	}
	if (!strcasecmp(options.listconf, "debug-mask"))
	{
		debug_mask_list();
		exit(EXIT_SUCCESS);
	}
#endif
}

void listconf_parse_late(void)
{
/*	if (!strncasecmp(options.listconf, "list-data", 9) &&
	    (options.listconf[9] == '=' || options.listconf[9] == ':') &&
	    options.listconf[10] != '\0')
	{
		cfg_print_section_list_lines(&options.listconf[10], NULL);
		exit(EXIT_SUCCESS);
	}  */
	//
	// Other --list=help:WHAT are processed in listconf_parse_early(), but
	// these require a valid config:
	//
	if (!strcasecmp(options.listconf, "help:parameters"))
	{
//		cfg_print_section_names(1);
		exit(EXIT_SUCCESS);
	}
	if (!strcasecmp(options.listconf, "help:list-data"))
	{
//		cfg_print_section_names(2);
		exit(EXIT_SUCCESS);
	}

	// --list last resort: list subsections of any john.conf section name

	//printf("Subsections of [%s]:\n", options.listconf);
/*	if (cfg_print_subsections(options.listconf, NULL, NULL, 1))
		exit(EXIT_SUCCESS);
	else {
		fprintf(stderr, "Section [%s] not found.\n", options.listconf);
		// Just in case the user specified an invalid value
		// like help or list...
		// print the same list as with --list=?, but exit(EXIT_FAILURE)
		//
		listconf_list_options();
		exit(EXIT_FAILURE);
	} */
}
