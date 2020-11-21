/*
 * locateLineNum.cpp
 *
 *  Created on: Jun 9, 2018
 *      Author: dad
 */

// This function is based on the simplereader program included in the libdwarf-dev examples.
// On ubuntu 16.04, the original source can be found in the /usr/share/doc/libdwarf-dev/examples
// folder after installing libdwarf-dev.

/*
  Copyright (c) 2009-2010 David Anderson.  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of the example nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY David Anderson ''AS IS'' AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL David Anderson BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <sys/types.h> /* For open() */
#include <sys/stat.h>  /* For open() */
#include <fcntl.h>     /* For open() */
#include <stdlib.h>     /* For exit() */
#ifdef WINDOZE
#include "unistd.h"
#else   // WINDOZE
#include <unistd.h>     /* For close() */
#include <dwarf.h>
#endif  // WINDOZE
#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef BSD
#include </usr/local/include/libdwarf.h>
#else   // BSD
#ifndef WINDOZE
#include <libdwarf.h>
#include <err.h>
#endif  // WINDOZE
#endif  // BSD

#include "debug.h"
#include "debug01.h"

#ifndef WINDOZE

struct srcfilesdata {
    char ** srcfiles;
    Dwarf_Signed srcfilescount;
    int srcfilesres;
};

static int read_cu_list(Dwarf_Debug dbg, long long unsigned int lluRelAddress , char * szFunctionName);
static int print_die_data(Dwarf_Debug dbg, Dwarf_Die print_me,int level,
   struct srcfilesdata *sf, Dwarf_Unsigned *lluSubAdd, char * szFunctionName );
static int get_die_and_siblings(Dwarf_Debug dbg, Dwarf_Die in_die,int in_level,
   struct srcfilesdata *sf, Dwarf_Unsigned *lluSubAdd , char * szFunctionName);
static void resetsrcfiles(Dwarf_Debug dbg,struct srcfilesdata *sf);

static int namesoptionon = 1;

int locateLineNum ( long long unsigned int lluRelAddress , char * szFunctionName , char * szExeFile )
{
    Dwarf_Debug dbg = 0;
    int fd = -1;
    const char *filepath = "";
    int res = DW_DLV_ERROR;
    Dwarf_Error error;
    Dwarf_Handler errhand = NULL;
    Dwarf_Ptr errarg = NULL;
    int linenumber = 0;

    if ( szExeFile != NULL ) {
    	filepath = szExeFile;
    	fd = open(filepath,O_RDONLY);
    	if(fd < 0) {
    	    printf("Failure attempting to open \"%s\"\n",filepath);
    	    dfprintf(__LINE__,__FILE__,TRACE,"Failure attempting to open \"%s\"\n",filepath);
    	    return(-2);
    	}
    } else {
    	return(-1);
    }

    res = dwarf_init(fd,DW_DLC_READ,errhand,errarg, &dbg,&error);
    if(res != DW_DLV_OK) {
        printf("Giving up, cannot do DWARF processing\n");
        errx(EXIT_FAILURE, "dwarf_init: %s", (char *)dwarf_errmsg(error));
        dfprintf(__LINE__,__FILE__,TRACE,"Giving up, cannot do DWARF processing (%s)\n",dwarf_errmsg(error));
        return(-3);
    }

    linenumber = read_cu_list(dbg, lluRelAddress , szFunctionName);

    res = dwarf_finish(dbg,&error);
    if(res != DW_DLV_OK) {
        printf("dwarf_finish failed!\n");
        dfprintf(__LINE__,__FILE__,TRACE,"dwarf_finish failed!\n");
    }
    close(fd);
    return linenumber;
}


static int
read_cu_list(Dwarf_Debug dbg, long long unsigned int lluRelAddress , char * szFunctionName)
{
    Dwarf_Unsigned cu_header_length = 0;
    Dwarf_Half version_stamp = 0;
    Dwarf_Unsigned abbrev_offset = 0;
    Dwarf_Half address_size = 0;
    Dwarf_Unsigned next_cu_header = 0;
    Dwarf_Error error;
    int cu_number = 0;

/* added by jck:... */

Dwarf_Signed cnt;
Dwarf_Line *linebuf;
Dwarf_Line *line;
int sres;
unsigned int i;
Dwarf_Unsigned  returned_lineno;
//Dwarf_Signed  return_lineoff;
Dwarf_Error dwerror;
Dwarf_Addr return_lineaddr;
Dwarf_Unsigned lluSubAdd;
//char * return_linesrc;
int notfound = 1;
int linenumber = 0;


    while(notfound) {
    	cu_number++;
        struct srcfilesdata sf;
        sf.srcfilesres = DW_DLV_ERROR;
        sf.srcfiles = 0;
        sf.srcfilescount = 0;
        Dwarf_Die no_die = 0;
        Dwarf_Die cu_die = 0;
        int res = DW_DLV_ERROR;
        res = dwarf_next_cu_header(dbg,&cu_header_length,
            &version_stamp, &abbrev_offset, &address_size,
            &next_cu_header, &error);
        if(res == DW_DLV_ERROR) {
            printf("Error in dwarf_next_cu_header\n");
            dfprintf(__LINE__,__FILE__,TRACE,"Error in dwarf_next_cu_header\n");
            exit(1);
        }
        if(res == DW_DLV_NO_ENTRY) {
            /* Done. */
            return(linenumber);
        }
        /* The CU will have a single sibling, a cu_die. */
        res = dwarf_siblingof(dbg,no_die,&cu_die,&error);
        if(res == DW_DLV_ERROR) {
            printf("Error in dwarf_siblingof on CU die \n");
            dfprintf(__LINE__,__FILE__,TRACE,"Error in dwarf_siblingof on CU die \n");
            exit(1);
        }
        if(res == DW_DLV_NO_ENTRY) {
            /* Impossible case. */
            printf("no entry! in dwarf_siblingof on CU die \n");
            dfprintf(__LINE__,__FILE__,TRACE,"no entry! in dwarf_siblingof on CU die \n");
            exit(1);
        }
        if (get_die_and_siblings(dbg,cu_die,0,&sf, &lluSubAdd, szFunctionName )){
/* added by jck:... */
//        	printf("..looking for lluRelAddress+lluSubAdd = %llx\n",lluRelAddress+lluSubAdd);
        	dfprintf(__LINE__,__FILE__,TRACEDWARF,"..looking for lluRelAddress+lluSubAdd = %llx\n",lluRelAddress+lluSubAdd);
			sres = dwarf_srclines(cu_die, &linebuf,&cnt, &error);
			if (sres == DW_DLV_OK) {
				line=linebuf;
				for (i = 0; i < cnt; ++i) {
				  dwarf_lineno( *line, &returned_lineno, &dwerror);
//				  dwarf_lineoff( *line, &return_lineoff, &dwerror);
				  dwarf_lineaddr( *line, &return_lineaddr, &dwerror);
//				  dwarf_linesrc( *line, &return_linesrc, &dwerror);
//				  printf("Line: %10llu    SubAdd: %10llx    Address: %10llx    File: %s\n",
//								returned_lineno,
//								lluSubAdd,
//								return_lineaddr,
//								return_linesrc ? return_linesrc : "");
				  dfprintf(__LINE__,__FILE__,TRACEDWARF2,"Line: %10llu    SubAdd: %10llx    Address: %10llx    File: %s\n"
								,returned_lineno
 //								,lluSubAdd
								,0
								,return_lineaddr
 //								,return_linesrc ? return_linesrc : ""
								,""
								);

				  line++;
				  if ( (lluRelAddress+lluSubAdd)<return_lineaddr ) {
					  dfprintf(__LINE__,__FILE__,TRACEDWARF,"found line... it's number %i !!!\n",linenumber);
					  break;
				  }
				  linenumber = (int)returned_lineno;
				}
				dwarf_srclines_dealloc(dbg, linebuf, cnt);
				notfound = 0;
			}
        }
/*.........*/

        dwarf_dealloc(dbg,cu_die,DW_DLA_DIE);
        resetsrcfiles(dbg,&sf);
        if(!notfound)break;
    }
    return(linenumber);
}

static int
get_die_and_siblings(Dwarf_Debug dbg, Dwarf_Die in_die,int in_level,
   struct srcfilesdata *sf, Dwarf_Unsigned *lluSubAdd, char * szFunctionName )
{
    int res = DW_DLV_ERROR;
    Dwarf_Die cur_die=in_die;
    Dwarf_Die child = 0;
    Dwarf_Error error;
    int found = 0;

    if(print_die_data(dbg,in_die,in_level,sf, lluSubAdd, szFunctionName ))return(1);

    for(;;) {
        Dwarf_Die sib_die = 0;
        res = dwarf_child(cur_die,&child,&error);
        if(res == DW_DLV_ERROR) {
            printf("Error in dwarf_child , level %d \n",in_level);
            dfprintf(__LINE__,__FILE__,TRACE,"Error in dwarf_child , level %d \n",in_level);
            exit(1);
        }
        if(res == DW_DLV_OK) {
            if (get_die_and_siblings(dbg,child,in_level+1,sf, lluSubAdd , szFunctionName)) found=1;
        }
        /* res == DW_DLV_NO_ENTRY */
        res = dwarf_siblingof(dbg,cur_die,&sib_die,&error);
        if(res == DW_DLV_ERROR) {
            printf("Error in dwarf_siblingof , level %d \n",in_level);
            dfprintf(__LINE__,__FILE__,TRACE,"Error in dwarf_siblingof , level %d \n",in_level);
            exit(1);
        }
        if(res == DW_DLV_NO_ENTRY) {
            /* Done at this level. */
            break;
        }
        /* res == DW_DLV_OK */
        if(cur_die != in_die) {
            dwarf_dealloc(dbg,cur_die,DW_DLA_DIE);
        }
        cur_die = sib_die;
        if(print_die_data(dbg,cur_die,in_level,sf, lluSubAdd, szFunctionName ))found=1;
    }
    return(found);
}
static void
get_addr(Dwarf_Attribute attr,Dwarf_Addr *val)
{
    Dwarf_Error error = 0;
    int res;
    Dwarf_Addr uval = 0;
    res = dwarf_formaddr(attr,&uval,&error);
    if(res == DW_DLV_OK) {
        *val = uval;
        return;
    }
    return;
}
static void
get_number(Dwarf_Attribute attr,Dwarf_Unsigned *val)
{
    Dwarf_Error error = 0;
    int res;
    Dwarf_Signed sval = 0;
    Dwarf_Unsigned uval = 0;
    res = dwarf_formudata(attr,&uval,&error);
    if(res == DW_DLV_OK) {
        *val = uval;
        return;
    }
    res = dwarf_formsdata(attr,&sval,&error);
    if(res == DW_DLV_OK) {
        *val = sval;
        return;
    }
    return;
}
static void
print_subprog(Dwarf_Debug dbg,Dwarf_Die die, int level,
    struct srcfilesdata *sf, Dwarf_Unsigned *lluSubAdd)
{
    int res;
    Dwarf_Error error = 0;
    Dwarf_Attribute *attrbuf = 0;
    Dwarf_Addr lowpc = 0;
    Dwarf_Addr highpc = 0;
    Dwarf_Signed attrcount = 0;
    Dwarf_Unsigned i;
    Dwarf_Unsigned filenum = 0;
    Dwarf_Unsigned linenum = 0;
    char *filename = 0;
    res = dwarf_attrlist(die,&attrbuf,&attrcount,&error);
    if(res != DW_DLV_OK) {
        return;
    }
    for(i = 0; i < (Dwarf_Unsigned)attrcount ; ++i) {
        Dwarf_Half aform;
        res = dwarf_whatattr(attrbuf[i],&aform,&error);
        if(res == DW_DLV_OK) {
            if(aform == DW_AT_decl_file) {
                get_number(attrbuf[i],&filenum);
                if((filenum > 0) && (sf->srcfilescount > (Dwarf_Signed)(filenum-1))) {
                    filename = sf->srcfiles[filenum-1];
                }
            }
            if(aform == DW_AT_decl_line) {
                get_number(attrbuf[i],&linenum);
            }
            if(aform == DW_AT_low_pc) {
                get_addr(attrbuf[i],&lowpc);
            }
            if(aform == DW_AT_high_pc) {
                get_addr(attrbuf[i],&highpc);
            }
        }
        dwarf_dealloc(dbg,attrbuf[i],DW_DLA_ATTR);
    }
    if(filenum || linenum) {
        dfprintf(__LINE__,__FILE__,TRACEDWARFSUB,"<%3d> file: %" DW_PR_DUu " %s  line %"
            DW_PR_DUu "\n",level,filenum,filename?filename:"",linenum);
    }
    if(lowpc) {
        dfprintf(__LINE__,__FILE__,TRACEDWARFSUB,"<%3d> low_pc : 0x%" DW_PR_DUx  "\n",
            level, (Dwarf_Unsigned)lowpc);
        *lluSubAdd = lowpc;
    }
    if(highpc) {
        dfprintf(__LINE__,__FILE__,TRACEDWARFSUB,"<%3d> high_pc: 0x%" DW_PR_DUx  "\n",
            level, (Dwarf_Unsigned)highpc);
        *lluSubAdd = highpc;
    }
    dwarf_dealloc(dbg,attrbuf,DW_DLA_LIST);
}

static void
print_comp_dir(Dwarf_Debug dbg,Dwarf_Die die,int level, struct srcfilesdata *sf)
{
    int res;
    Dwarf_Error error = 0;
    Dwarf_Attribute *attrbuf = 0;
    Dwarf_Signed attrcount = 0;
    Dwarf_Unsigned i;
    res = dwarf_attrlist(die,&attrbuf,&attrcount,&error);
    if(res != DW_DLV_OK) {
        return;
    }
    sf->srcfilesres = dwarf_srcfiles(die,&sf->srcfiles,&sf->srcfilescount,
        &error);
    for(i = 0; i < (Dwarf_Unsigned)attrcount ; ++i) {
        Dwarf_Half aform;
        res = dwarf_whatattr(attrbuf[i],&aform,&error);
        if(res == DW_DLV_OK) {
            if(aform == DW_AT_comp_dir) {
                char *name = 0;
                res = dwarf_formstring(attrbuf[i],&name,&error);
                if(res == DW_DLV_OK) {
                    dfprintf(__LINE__,__FILE__,TRACEDWARFDIR,"<%3d> compilation directory : \"%s\"\n",
                        level,name);
                }
            }
            if(aform == DW_AT_stmt_list) {
                /* Offset of stmt list for this CU in .debug_line */
            }
        }
        dwarf_dealloc(dbg,attrbuf[i],DW_DLA_ATTR);
    }
    dwarf_dealloc(dbg,attrbuf,DW_DLA_LIST);
}

static void
resetsrcfiles(Dwarf_Debug dbg,struct srcfilesdata *sf)
{
    Dwarf_Signed sri = 0;
    for (sri = 0; sri < sf->srcfilescount; ++sri) {
        dwarf_dealloc(dbg, sf->srcfiles[sri], DW_DLA_STRING);
    }
    dwarf_dealloc(dbg, sf->srcfiles, DW_DLA_LIST);
    sf->srcfilesres = DW_DLV_ERROR;
    sf->srcfiles = 0;
    sf->srcfilescount = 0;
}

static int
print_die_data(Dwarf_Debug dbg, Dwarf_Die print_me,int level,
    struct srcfilesdata *sf, Dwarf_Unsigned *lluSubAdd, char * szFunctionName )
{
    char *name = 0;
    Dwarf_Error error = 0;
    Dwarf_Half tag = 0;
    const char *tagname = 0;
    int localname = 0;
    int found = 0;

    int res = dwarf_diename(print_me,&name,&error);

    if(res == DW_DLV_ERROR) {
        printf("Error in dwarf_diename , level %d \n",level);
        dfprintf(__LINE__,__FILE__,TRACE,"Error in dwarf_diename , level %d \n",level);
        exit(1);
    }
    if(res == DW_DLV_NO_ENTRY) {
        name = (char*)"<no DW_AT_name attr>";
        localname = 1;
    }
    res = dwarf_tag(print_me,&tag,&error);
    if(res != DW_DLV_OK) {
        printf("Error in dwarf_tag , level %d \n",level);
        dfprintf(__LINE__,__FILE__,TRACE,"Error in dwarf_tag , level %d \n",level);
        exit(1);
    }
    res = dwarf_get_TAG_name(tag,&tagname);
    if(res != DW_DLV_OK) {
        printf("Error in dwarf_get_TAG_name , level %d \n",level);
        dfprintf(__LINE__,__FILE__,TRACE,"Error in dwarf_get_TAG_name , level %d \n",level);
        exit(1);
    }
    if(namesoptionon) {
        if( tag == DW_TAG_subprogram) {
            dfprintf(__LINE__,__FILE__,TRACEDWARFDIE,"<%3d> subprogram            : \"%s\"\n",level,name);
            if (!strcmp (name , szFunctionName)) {
            	dfprintf(__LINE__,__FILE__,TRACEDWARFDIE,"%s found!!!!!\n",name);
            	found = 1;
            }
            print_subprog(dbg,print_me,level,sf, lluSubAdd);
        } else if (tag == DW_TAG_compile_unit ||
            tag == DW_TAG_partial_unit ||
            tag == DW_TAG_type_unit) {

            resetsrcfiles(dbg,sf);
            dfprintf(__LINE__,__FILE__,TRACEDWARFDIE,"<%3d> source file           : \"%s\"\n",level,name);
            print_comp_dir(dbg,print_me,level,sf);
        }
    } else {
        printf("<%d> tag: %d %s  name: \"%s\"\n",level,tag,tagname,name);
    }
    if(!localname) {
        dwarf_dealloc(dbg,name,DW_DLA_STRING);
    }
    return(found);
}

#endif


