"""Collection of helper functions"""


import os, sys, errno, fnmatch, subprocess, time, copy
import json
import re

# Module globals
_dirstack = []
quite_mode   = False
verbose_mode = False

#-----------------------------------------------------------------------------
def convert_name_to_variable_name( name, upper_case = True ):
    new_name = re.sub(r'\W+|^(?=\d)','_', name)
    if ( upper_case ):
        new_name = new_name.upper()
    return new_name

#-----------------------------------------------------------------------------
def print_warning( msg ):
    if ( not quite_mode ):
        print( "Warn:  " + msg )

def set_quite_mode( newstate ):
    global quite_mode
    quite_mode = newstate
       

#-----------------------------------------------------------------------------
def print_verbose( msg, no_new_line=False, bare=False ):
    if ( verbose_mode ):
        if ( not no_new_line ):
            if ( bare ):
                print( msg )
            else:
                print( "Info:  " + msg )
        else:
            if ( bare ):
                print( msg, end='' )
            else:
                print( "Info:  " + msg, end='' )
        
def set_verbose_mode( newstate ):
    global verbose_mode
    verbose_mode = newstate
       
#-----------------------------------------------------------------------------
def walk_file_list( pattern, pkgpath ):
    list = []
    for root, dirs, files in os.walk(pkgpath):
        for f in fnmatch.filter(files,pattern):
            list.append( standardize_dir_sep( os.path.join(root,f) ))
            
    return list        

#-----------------------------------------------------------------------------
def standardize_dir_sep( pathinfo ):
    return pathinfo.replace( '/', os.sep ).replace( '\\', os.sep )

def push_dir( newDir ):
    global _dirstack
    
    if ( not os.path.exists(newDir) ):
        exit( "ERROR: Invalid path: "+ newDir )
    
    _dirstack.append( os.getcwd() )
    os.chdir( newDir )
    
    
def pop_dir():
    global _dirstack
    os.chdir( _dirstack.pop() )

def mkdirs( dirpath, exist_ok=True ):
    try:
        os.makedirs(dirpath,exist_ok=exist_ok)
    except: 
        sys.exit( "ERROR: cannot create directory tree: {}.[{}]".format( dirpath, sys.exc_info()[0] ) )

#-----------------------------------------------------------------------------
def load_node_file( filename ):
    try:
        with open(filename) as f:
            json_dict = json.load( f )
            return json_dict

    except Exception as e:
        return None

def write_node_file( json_dict, filename, pretty=False ):

    with open( filename, "w+" ) as file:
        if ( pretty ):
            file.write( json.dumps( json_dict, indent=2 ) )
        else:
            file.write( json.dumps( json_dict ) )
