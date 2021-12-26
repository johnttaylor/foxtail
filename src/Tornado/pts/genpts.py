#!/usr/bin/python3
"""Invokes NQBP's genpts_base.py script.  This script is setup to run as 
   NQBP pre-processing script

   The script is ALWAYS passed <arg1> ... <arg6> where:
    <arg1>: build|clean         
    <arg2>: verbose|terse
    <arg3>: <workspace-dir>
    <arg4>: <package-dir> 
    <arg5>: <project-dir> 
    <arg6>: <current-dir> 

"""

import os
import sys

# MAIN
if __name__ == '__main__':
    # Create path to the 'real' script
    script = os.path.join(sys.argv[4], "scripts", "genpts_base.py" )
    args   = " ".join(sys.argv[1:]) 

    # Run the 'actual' pre-processing script
    sys.exit( os.system( f'{script} {args}' ) )
