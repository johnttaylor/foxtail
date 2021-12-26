#!/usr/bin/python3
#=============================================================================
# Generates the Foxtail Point Identifiers and code that allocated the 
# point instances
#
#    The script is ALWAYS passed <arg1> ... <arg6> where:
#       <arg1>: build|clean         
#       <arg2>: verbose|terse
#       <arg3>: <workspace-dir>
#       <arg4>: <package-dir> 
#       <arg5>: <project-dir> 
#       <arg6>: <current-dir> 
#=============================================================================

#
import sys   
import os
import subprocess

ALLOCATE_FILE_BASE_NAME = "Points"
POINT_MACRO             = "FXT_POINT_DEFINE"
EXCLUDE_HEADER          = os.path.join( 'Fxt', 'Point', "AutoGen.h" )
START_MARKER            = 'MARKER_FXT_BEGIN_AUTO_GENERATION'
END_MARKER              = 'MARKER_FXT_END_AUTO_GENERATION'

copyright_header = """* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice."""

#
import subprocess
import re
import sys

#-----------------------------------------------------------------------------
def standardize_dir_sep( pathinfo ):
    return pathinfo.replace( '/', os.sep ).replace( '\\', os.sep )

def getHeader():
    return  '/*-----------------------------------------------------------------------------\n' + copyright_header + '\n*----------------------------------------------------------------------------*/\n/** @file */\n\n'

#-----------------------------------------------------------------------------
# Gets a list of header files
def get_includes( headerfile ):
    headers = []
    with open( headerfile ) as h:
        for line in h:
            if ( line.find( '#include "' ) != -1 ):
                f = standardize_dir_sep(line.strip( '#include "' ).strip().strip('"') )
                if ( os.sep in f and f != EXCLUDE_HEADER ):
                    headers.append( f )
    return headers


# Gets a list of Point definitions
def get_point_list( allocate_file, header_files, pkg_root, src_root="src" ):
    points = []
    idx    = 0

    for h in header_files:
        path = os.path.join( pkg_root, src_root, h )
        with open( path ) as h:
            for line in h:
                if ( line.find( POINT_MACRO ) != -1 ):
                    name,type = line.strip( POINT_MACRO ).strip().strip('(').strip(')').split() 
                    points.append( (name.strip(), type.strip(), idx) )
                    idx = idx + 1

    with open( allocate_file ) as h:
        for line in h:
            if ( line.find( POINT_MACRO ) != -1 ):
                name,type = line.strip( POINT_MACRO ).strip().strip('(').strip(')').split() 
                points.append( (name.strip(), type.strip(), idx) )
                idx = idx + 1

    return points


# returns a dictionary, where each value for the dictionary entry is a list of points
def get_type_lists( point_list ):
    types = {}
    for p in point_list:
        ptype = p[1]
        if ( not ptype in types ):
            types[ptype] = []
        types[ptype].append( p )

    return types

#-----------------------------------------------------------------------------
def generate_point_identifiers( headerfile, points ):
    tmpfile  = headerfile + ".tmp"
    skipping = False
    with open( headerfile ) as inf:
        with open( tmpfile, "w") as outf:  
            for line in inf:
                if ( not skipping ):
                    outf.write( line )
                    if (  START_MARKER in line ):
                        skipping = True
                        gen_identifiers( points, outf )
                else:
                    if ( END_MARKER in line ):
                        outf.write( line )
                        skipping = False
    
    os.remove( headerfile )
    os.rename( tmpfile, headerfile )

def gen_identifiers( points, fd ):
    fd.write( "my auto gen code here!\n" )

#-----------------------------------------------------------------------------
# MAIN
if __name__ == '__main__':
    
    # Housekeeping
    #global copyright_header
    pkgroot              = sys.argv[4]
    allocate_header_file = ALLOCATE_FILE_BASE_NAME + ".h"

    # Generate a list of points and list of point types
    files  = get_includes( allocate_header_file )
    points = get_point_list( allocate_header_file, files, pkgroot )
    types  = get_type_lists( points )
    
    # Update the allocation header file with the Point Identifiers
    print( points )
    print( types )
    generate_point_identifiers( allocate_header_file, points )

