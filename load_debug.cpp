/*
 * load_debug.cpp
 *
 *  Created on: May 16, 2018
 *      Author: dad
 */

#ifdef WINDOZE
#pragma message( "Compiling " __FILE__ " on " __DATE__ " at " __TIME__ )
#pragma message( "File last modified on " __TIMESTAMP__ )
#pragma message( "  ")
#pragma comment( compiler )
#pragma comment( user, "File: " __FILE__ ". Compiled on " __DATE__ " at " __TIME__ ".  Last modified on " __TIMESTAMP__ )
#endif


#include "debug.h"
#include "debug01.h"
#include "common.h"
//#include "getopt.h"
#include "options.h"

extern DEBUGOPTIONSTYPE myoptions;

void load_debug () 
{
#ifdef DEBUG
	struct list_entry *current;
	int i;
	int j=0;
	int k=0;
	int iflagset;
	const char *isset[] = { "not set", "set" };

// note: the TRACE debug flag is initially turned on in debug_init to permit generation of
// debug print output prior to the load_debug call. We will now turn it back off. If the user
// wants TRACE turned on following this call, he will need to specify --debug-flags=TRACE as a
// calling argument to the program
	bdebug_flag_set[TRACE] = 0;

	if ( myoptions.debug_level > 0 ) {
		fprintf(stderr,"   debug verbosity = %i\n",myoptions.debug_level);
		debugInitialVirbosity = debugVirbosity =   myoptions.debug_level;
	}

	bdebug_flag_set[0] = bdebug_flag_set[NUMDEBUGFLAGS-1] = 0;

	if ( (current = myoptions.debug_flags->head) )
	{
		do {
			k++;
			for(i=0;i<NUMDEBUGFLAGS;i++) {
				if( strcmp(debug_flag[i], current->data) == 0 ) {
					bdebug_flag_set[i] = 1;
					j++;
				}
			}
			if ( j != k ) fprintf(stderr, " %s is not a valid debug flag\n", current->data);
		} while ((current = current->next));
	}
	if ( j != k ) {
		fprintf(stderr," see --list=debug-flags\n");
		exit(EXIT_FAILURE);
	}
	if ( bdebug_flag_set[ALLON] ) {
		for(i=0;i<NUMDEBUGFLAGS;i++){
			if ( i != QUIET && i != NONE ) bdebug_flag_set[i]=1;
		}
	} 
	if ( (current = myoptions.debug_flags->head) ) {
		fprintf(stderr,"   debug flags set : ");
		do {
			fprintf(stderr, "%s", current->data);
			if ( current->next ) fprintf(stderr, ",");
		} while ( ( current = current->next ) );
		fprintf(stderr,"\n");
	}

	int iDebugOutputDevice = -1;

	if ( myoptions.debug_server_name != NULL ) {
		printf("   server name = %s\n", myoptions.debug_server_name);
		lpDebugServerName = myoptions.debug_server_name;
	}
	if ( myoptions.degug_port_number != 0 ) {
		printf ("   port number = %i\n",myoptions.degug_port_number);
		port = myoptions.degug_port_number;
	}
	if ( myoptions.debug_filename != NULL ) {
		printf("   debug file name = %s\n",myoptions.debug_filename);
		lpDebugOutputFileName = myoptions.debug_filename;
	}
	if ( myoptions.debug_device != NULL ) {
		for(i = 0;i < DIMDEBUGDEVICES;i++) {
			if ( strcmp (debug_devices[i] , myoptions.debug_device) == 0 ) {
				iDebugOutputDevice = i;
				printf("   debug output device type index is %i, (%s)\n", i,debug_devices[i]);
			}
		}
		if ( iDebugOutputDevice == -1 ) {
			iDebugOutputDevice = BITBUCKET;
			fprintf(stderr," %s is not a valid debug output device, see --list=debug-device\n", myoptions.debug_device);
			exit(EXIT_FAILURE);
		}
	}
	if ( myoptions.debug_color_flag != 0 ) {
		printf("   color debug flag is set\n");
		ColorDebug = 1;
	}
	else {
		printf("   color debug flag is not set\n");
		ColorDebug = 0;
	}
	if ( myoptions.debug_clear_screen > 0 && myoptions.debug_clear_screen < 4) {
		printf("   debug clear screen on exit flag is set to %i\n", myoptions.debug_clear_screen);
		ClearDebugScreenOnExit = myoptions.debug_clear_screen;
	}
	else {
		printf("   debug clear screen on exit flag is not set\n");
		ClearDebugScreenOnExit = 0;
	}
	if ((current = myoptions.debug_mask_messages->head)) {
		printf("   messages to mask:\n");
		do {
			printf("      %s\n",current->data);
		} while ((current = current->next));
	}
	if ((current = myoptions.debug_display_messages->head)) {
		printf("   messages to display:\n");
		do {
			printf("      %s\n",current->data);
		} while ((current = current->next));
	}

//	iDebugOutputDevice = iDebugOutputDeviceSave;
	if ( DBGLVLTEST > NONE ) {
		if ( set_debug_device(myoptions.debug_device) < -1 ) {
			fprintf(stderr,"set_debug_device failed\n");
		} else {
			dfprintf2(__LINE__,__FILE__,DEBUGINIT,"\33[1;32mDebug output device set to type %i\33[0m (%s)\n", iDebugOutputDevice,debug_devices[iDebugOutputDevice]);
			iflagset = 0;
#ifdef _MSC_VER
			iflagset = 1;
#endif
			dfprintf2(__LINE__,__FILE__,DEBUGINIT,"\33[1;32m _MSC_VER is %s\33[0m\n", isset[iflagset]);
			iflagset = 0;
		}
		if ( bdebug_flag_set[QUIET] ) debugVirbosity = 0;

	}
#endif	// DEBUG
	return;
}
