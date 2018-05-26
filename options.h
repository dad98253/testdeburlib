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

#if AC_BUILT
#include "autoconfig.h"
#endif

#include "list.h"
//#include "loader.h"
#include "getopt.h"

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
/* A cracking mode enabled */
#define FLG_CRACKING_CHK		0x00000020
#define FLG_CRACKING_SUP		0x00000040
#define FLG_CRACKING_SET \
	(FLG_CRACKING_CHK | FLG_CRACKING_SUP | FLG_ACTION | FLG_PWD_REQ)
/* Wordlist mode enabled, options.wordlist is set to the file name, or
 * we get it from john.conf */
#define FLG_WORDLIST_CHK		0x00000080
#define FLG_WORDLIST_SET \
	(FLG_WORDLIST_CHK | FLG_CRACKING_SET | FLG_RULES_ALLOW)
/* Wordlist mode enabled, reading from stdin */
#define FLG_STDIN_CHK			0x00000100
#define FLG_STDIN_SET			(FLG_STDIN_CHK | FLG_WORDLIST_SET)
/* Wordlist rules enabled */
#define FLG_RULES			0x00000200
/* "Single crack" mode enabled */
#define FLG_SINGLE_CHK			0x00000400
#define FLG_SINGLE_SET			(FLG_SINGLE_CHK | FLG_CRACKING_SET)
/* Incremental mode enabled */
#define FLG_INC_CHK			0x00000800
#define FLG_INC_SET			(FLG_INC_CHK | FLG_CRACKING_SET)
/* Mask mode enabled (might be hybrid) */
#define FLG_MASK_CHK			0x00001000
#define FLG_MASK_SET \
	(FLG_MASK_CHK | FLG_ACTION | FLG_CRACKING_SUP | FLG_PWD_SUP)
/* External mode or word filter enabled */
#define FLG_EXTERNAL_CHK		0x00002000
#define FLG_EXTERNAL_SET \
	(FLG_EXTERNAL_CHK | FLG_ACTION | FLG_CRACKING_SUP | FLG_PWD_SUP)
/* Batch cracker */
#define FLG_BATCH_CHK			0x00004000
#define FLG_BATCH_SET			(FLG_BATCH_CHK | FLG_CRACKING_SET)
/* Stdout mode */
#define FLG_STDOUT			0x00008000
/* Restoring an interrupted session */
#define FLG_RESTORE_CHK			0x00010000
#define FLG_RESTORE_SET			(FLG_RESTORE_CHK | FLG_ACTION)
/* A session name is set */
#define FLG_SESSION			0x00020000
/* Print status of a session */
#define FLG_STATUS_CHK			0x00040000
#define FLG_STATUS_SET			(FLG_STATUS_CHK | FLG_ACTION)
/* Make a charset */
#define FLG_MAKECHR_CHK			0x00100000
#define FLG_MAKECHR_SET \
	(FLG_MAKECHR_CHK | FLG_ACTION | FLG_PWD_SUP)
/* Show cracked passwords */
#define FLG_SHOW_CHK			0x00200000
#define FLG_SHOW_SET \
	(FLG_SHOW_CHK | FLG_ACTION | FLG_PWD_REQ)
/* Perform a benchmark */
#define FLG_TEST_CHK			0x00400000
#define FLG_TEST_SET \
	(FLG_TEST_CHK | FLG_CRACKING_SUP | FLG_ACTION)
#ifdef HAVE_FUZZ
/* Perform a fuzzing */
#define FLG_FUZZ_CHK			0x08000000
#define FLG_FUZZ_SET \
	(FLG_FUZZ_CHK | FLG_CRACKING_SUP | FLG_ACTION)
/* Dump fuzzed hashes */
#define FLG_FUZZ_DUMP_CHK		0x40000000
#define FLG_FUZZ_DUMP_SET \
	(FLG_FUZZ_DUMP_CHK | FLG_CRACKING_SUP | FLG_ACTION)
#endif
/* Passwords per salt requested */
#define FLG_SALTS			0x01000000
/* Ciphertext format forced */
#define FLG_FORMAT			0x02000000
/* Memory saving enabled */
#define FLG_SAVEMEM			0x04000000
/* Node number(s) specified */
#define FLG_NODE			0x10000000
/* fork() requested, and process count specified */
#define FLG_FORK			0x20000000

/* Note that 0x80000000 is taken for OPT_REQ_PARAM, see getopt.h */

/*
 * Jumbo Options flags bitmasks (high 32 bits)
 *
 * Tip: For your private patches, pick first free from MSB. When
 * sharing your patch, pick first free from LSB of high 32 bits.
 *
 * In Jumbo, the combination flg_set == FLG_ZERO and req_clr == OPT_REQ_PARAM
 * gets dupe checking automatically, without a specific flag.
 */
#define FLG_ZERO			0x0

/* .pot file used as wordlist, options.wordlist is set to the file name, or
 * we use the active .pot file */
#define FLG_LOOPBACK_CHK		0x0000000100000000ULL
#define FLG_LOOPBACK_SET	  \
	(FLG_LOOPBACK_CHK | FLG_WORDLIST_SET | FLG_CRACKING_SET | FLG_DUPESUPP)
/* pipe mode enabled, reading from stdin with rules support */
#define FLG_PIPE_CHK			0x0000000200000000ULL
#define FLG_PIPE_SET			(FLG_PIPE_CHK | FLG_WORDLIST_SET)
/* Dynamic load of foreign format module */
#define FLG_DYNFMT			0x0000000400000000ULL
/* Turn off logging */
#define FLG_NOLOG			0x0000000800000000ULL
/* Log to stderr */
#define FLG_LOG_STDERR			0x0000001000000000ULL
/* Markov mode enabled */
#define FLG_MKV_CHK			0x0000002000000000ULL
#define FLG_MKV_SET			(FLG_MKV_CHK | FLG_CRACKING_SET)
/* Emit a status line for every password cracked */
#define FLG_CRKSTAT			0x0000004000000000ULL
/* Wordlist dupe suppression */
#define FLG_DUPESUPP			0x0000008000000000ULL
/* Force scalar mode */
#define FLG_SCALAR			0x0000010000000000ULL
#define FLG_VECTOR			0x0000020000000000ULL
/* Reject printable binaries */
#define FLG_REJECT_PRINTABLE		0x0000040000000000ULL
/* Skip self tests */
#define FLG_NOTESTS			0x0000080000000000ULL
/* Regex cracking mode */
#define FLG_REGEX_CHK			0x0000100000000000ULL
#define FLG_REGEX_SET	  \
	(FLG_REGEX_CHK | FLG_ACTION | FLG_CRACKING_SUP | FLG_PWD_SUP)
/* Encodings. You can only give one of --internal-enc or --target-enc */
#define FLG_INPUT_ENC			0x0000200000000000ULL
#define FLG_SECOND_ENC			0x0000400000000000ULL
/* --verbosity */
#define FLG_VERBOSITY			0x0000800000000000ULL
/* Sets FMT_NOT_EXACT, searching for cleartext collisions */
#define FLG_KEEP_GUESSING		0x0001000000000000ULL
/* Loops self-test forever */
#define FLG_LOOPTEST			0x0002000000000000ULL
/* Mask mode is stacked */
#define FLG_MASK_STACKED		0x0004000000000000ULL
/* Stacking modes */
#define FLG_STACKING			(FLG_MASK_CHK | FLG_REGEX_CHK)
/* Any stacking mode is active */
#define FLG_STACKED			(FLG_MASK_STACKED | FLG_REGEX_STACKED)
/* PRINCE mode enabled, options.wordlist is set to the file name, or
 * we get it from john.conf */
#define FLG_PRINCE_CHK			0x0008000000000000ULL
#define FLG_PRINCE_SET \
	(FLG_PRINCE_CHK | FLG_CRACKING_SET | FLG_RULES_ALLOW)
#define FLG_PRINCE_DIST			0x0010000000000000ULL
#define FLG_PRINCE_KEYSPACE		0x0020000000000000ULL
#define FLG_PRINCE_CASE_PERMUTE		0x0040000000000000ULL
#define FLG_PRINCE_LOOPBACK		0x0080000000000000ULL
#define FLG_PRINCE_MMAP			0x0100000000000000ULL
#define FLG_RULES_ALLOW			0x0200000000000000ULL
#define FLG_REGEX_STACKED		0x0400000000000000ULL
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
/* Option flags */
	opt_flags flags;

	/* Password files */
	struct list_main *passwd;

#ifdef DEBUG
/* debug options */
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

};

extern struct options_main options;

/*
 * Initializes the options structure.
 */
extern void opt_init(char *name, int argc, char **argv, int show_usage);

/*
 * Prints the "hidden" options usage
 */
extern void opt_print_hidden_usage(void);

#endif
