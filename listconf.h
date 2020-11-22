/*
 * Copyright (c) 2012, 2013 Frank Dittrich and magnum
 *
 * This software is hereby released to the general public under the following
 * terms:  Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 */

#ifndef _JOHN_LISTCONF_H
#define _JOHN_LISTCONF_H

#ifndef LISTCONFDEFS
#define EXTERN		extern
#define INITIZERO
#define INITSZERO
#define INITBOOLFALSE
#define INITBOOLTRUE
#define INITNULL
#define INITBUFFERSIZE
#define INITDIRSIZE
#else  // LISTCONFDEFS
#define EXTERN
#define INITIZERO	=0
#define INITSZERO	={0}
#define INITBOOLFALSE	=false
#define INITBOOLTRUE	=true
#define INITNULL	=NULL
#define INITBUFFERSIZE	=BUFFERSIZE
#define INITDIRSIZE		=DIRSIZE
#endif  // LISTCONFDEFS


#ifdef DEBUG
#define DEBUG_STRING " debug"
#else
#define DEBUG_STRING ""
#endif

#if defined(MEMDBG_ON) && defined(MEMDBG_EXTRA_CHECKS)
#define MEMDBG_STRING " memdbg-ex"
#elif defined(MEMDBG_ON)
#define MEMDBG_STRING " memdbg"
#else
#define MEMDBG_STRING ""
#endif

#define _STR_VALUE(arg)			#arg
#define STR_MACRO(n)			_STR_VALUE(n)

#define JOHN_VERSION			"1.0"

 // Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT	"64 bit"
#else   // _WIN64
#define ENVIRONMENT	"32 bit"
#endif  // _WIN64
// Check GCC
#elif __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT	"64 bit"
#else   // __x86_64__
#define ENVIRONMENT	"32 bit"
#endif  // __x86_64__
#else   // not windows or gnuc
#define ENVIRONMENT	""
#endif

#ifdef WINDOZE
#define JOHN_BLD "Windows " ENVIRONMENT
#else   // WINDOZE
#define JOHN_BLD "linux-gnu " ENVIRONMENT
#endif  // WINDOZE
#define JTR_GIT_VERSION ""
#if JTR_RELEASE_BUILD
#undef JTR_GIT_VERSION
#define JTR_GIT_VERSION JOHN_VERSION
#endif

#ifdef WINDOZE 
 //not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

/* Suboptions that can be used before full initialization, like --list=help */
EXTERN void listconf_parse_early(void);

/* Suboptions that depend on full initialization, like --list=externals */
EXTERN void listconf_parse_late(void);

/*
 * Prints the contents of a section's list, returns the number
 * of lines printed, or -1 if the section doesn't exist.
 */
int cfg_print_section_list_lines(char *section, char *subsection);

/*
 * Searches for sections with the supplied name, and prints a list of
 * valid subsections. If function is non-null, only prints subsections
 * (ie. external modes) that has function (ie. generate or filter).
 * If notfunction is non-null, that function must NOT be present (ie.
 * for listing external modes that has filter() but not generate() )
 */
int cfg_print_subsections(char *section, char *function, char *notfunction, int print_heading);


#undef EXTERN
#undef INITIZERO
#undef INITSZERO
#undef INITBOOLFALSE
#undef INITBOOLTRUE
#undef INITNULL
#undef INITBUFFERSIZE
#undef INITDIRSIZE

#endif
