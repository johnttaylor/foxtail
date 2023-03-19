#! /usr/bin/env python3
"""
 
Converts a Node file with symbolic point IDs to numeric point IDs
================================================================================
usage: name2id.py [options] <infile> [<outfile>]
       name2id.py [options] --guids <srcpath>

Arguments:
    <infile>       Input file (should be a valid Node JSON file)
    <outfile>      Output file. Defaults to <infile>.id
    --guids        Searches for GUID strings in the source code and populates
                   the type database/dictionary.
    <srcpath>      Path to source code to search
                         
Options:
    -e             Echo's output to STDOUT (still writes to <outfile>)
    -p, --pretty   Output is formatted to be human readable
    -s             Strip unused (by the firmware) KVPs
    -c FILE        Generates a file containing #define symbols for each named
                   Point.

    --append       When performing a GUID search/update - append the latest
                   search results to the database/dictionary.

    -v             Be verbose 
    -w             Disable warnings
    -h, --help     Displays this information

Notes:
    1. The 'id' for Points that are 'initial' values - will be auto populated
       (i.e. their value will be overwritten by the script)
      
"""

import sys
import os
import re

from docopt.docopt import docopt
import utils
import fxt_types

# Current Point ID
_cur_point_id = 0
_point_list   = []
_point_dict_by_name = {}

_dictionary_file = "fxt_types.py"

#------------------------------------------------------------------------------
# Populate a single point
def populatePoint( pt_obj, is_card_pt = False ):
    global _cur_point_id
    global _point_dict_by_name
    global _point_list

    if ( not 'id' in pt_obj ):
        sys.exit( f"ERROR: Missing point 'id' for: {pt_obj}" )

    # Create numeric ID for the point and store its associated name
    id_symbol                      = pt_obj['id']
    pt_obj['id']                   = _cur_point_id
    pt_obj['name']                 = id_symbol
    _point_dict_by_name[id_symbol] = _cur_point_id
    _cur_point_id                  = _cur_point_id + 1
    _point_list.append(id_symbol)

    # Generate IO Register point
    if ( is_card_pt ):
        pt_obj['ioRegId'] = _cur_point_id
        _cur_point_id     = _cur_point_id + 1
        _point_list.append('<n/a>' )

    # Check for an 'initial' setter point
    if ( 'initial' in pt_obj ):
        setter_obj       = pt_obj['initial']
        setter_obj['id'] = _cur_point_id
        _cur_point_id    = _cur_point_id + 1
        _point_list.append('<n/a>')

# Resolves a Point reference a single point
def resolvePoint( pt_obj ):
    global _point_dict_by_name

    if ( not 'idRef' in pt_obj ):
        sys.exit( f"ERROR: Missing point 'idRef' for: {pt_obj}" )

    # Look-up point ID
    id_symbol  = pt_obj['idRef']
    if ( not id_symbol in _point_dict_by_name ):
        sys.exit( f"ERROR: Missing point reference: {id_symbol}" )
    pt_obj['idRef']     = _point_dict_by_name[id_symbol]
    pt_obj['idRefName'] = id_symbol;


# Scan the Scanners
def scanScanner( scanner_obj ):
    if ( not "cards" in scanner_obj ):
        sys.exit( "ERROR: Input file is missing 'cards' array in scanner object" )
    card_list = scanner_obj["cards"]

    for c in card_list:
        utils.print_verbose( f"    Processing Card: {c['name']}" )
        if ( not "points" in c ):
            sys.exit( "ERROR: Input file is missing 'points' object a card object" )
        points_obj = c["points"]
        
        # Input points
        if ( "inputs" in points_obj ):
            inputs_list = points_obj["inputs"]
            for channel in inputs_list:
                utils.print_verbose( f"      Processing Card Channel: Input #{channel['channel']}" )
                populatePoint( channel, True )

        # Output points
        if ( "outputs" in points_obj ):
            outputs_list = points_obj["outputs"]
            for channel in outputs_list:
                utils.print_verbose( f"      Processing Card Channel: Output #{channel['channel']}" )
                populatePoint( channel, True )

# Scan the ExecutionSet
def scanExecutionSet( exeset_obj ):
    if ( not "logicChains" in exeset_obj ):
        sys.exit( "ERROR: Input file is missing 'logicChains' array in Execution Set object" )
    logicChains_list = exeset_obj["logicChains"]

    for lc in logicChains_list:
        # Connector Points
        utils.print_verbose( f"    Processing Logic Chain: {lc['name']}" )
        if ( "connectionPts" in lc ):
            conPts_list = lc["connectionPts"]
            utils.print_verbose( f"      Processing Connector Points: {lc['name']}" )
            for pt in conPts_list:
                populatePoint( pt )

        # Auto Points
        if ( "autoPts" in lc ):
            autoPts_list = lc["autoPts"]
            utils.print_verbose( f"      Processing Auto Points: {lc['name']}" )
            for pt in autoPts_list:
                populatePoint( pt )

# Resolve references for a ExecutionSet
def resolveExecutionSet( exeset_obj ):
    logicChains_list = exeset_obj["logicChains"]

    for lc in logicChains_list:
        utils.print_verbose( f"    Resolving Logic Chain references: {lc['name']}" )

        # Components
        if ( not "components" in lc ):
            sys.exit( "ERROR: Input file is missing 'components' array in LogicChain object" )
        components_list = lc["components"]
        
        for c in components_list:
            utils.print_verbose( f"      Resolving Components Points: {c['name']}" )

            # Inputs Points
            if ( not "inputs" in c ):
                sys.exit( "ERROR: Input file is missing 'inputs' array in Components object" )
            inputs_list = c["inputs"]

            utils.print_verbose( f"        Resolving Components Inputs Points: {c['name']}" )
            for pt in inputs_list:
                resolvePoint( pt )

            # Output Points
            if ( not "outputs" in c ):
                sys.exit( "ERROR: Input file is missing 'outputs' array in Components object" )
            inputs_list = c["outputs"]

            utils.print_verbose( f"        Resolving Components Outputs Points: {c['name']}" )
            for pt in inputs_list:
                resolvePoint( pt )

#------------------------------------------------------------------------------
def generate_define_list( filename ):
    with open( filename, "w+" ) as file:
        idx       = 0
        idx_named = 0
        for p in _point_list:
            if ( p != '<n/a>' ):
                idx_named = idx_named + 1
                file.write( f"#define FXT_PT_{utils.convert_name_to_variable_name(p)}    {idx}\n" )

            idx = idx + 1

        file.write( "\n" )
        file.write( f"#define FXT_PT_TOTAL_NUM_PTS      {idx}\n" )
        file.write( f"#define FXT_PT_TOTAL_NANMED_PTS   {idx_named}\n" )
    
#------------------------------------------------------------------------------
# Creates numeric values for all points 
def scanPoints( json_dict ):
    print(json_dict)
    if ( not "chassis" in json_dict ):
        sys.exit( "ERROR: Input file is missing 'chassis' object" )
    chassis_list = json_dict["chassis"]
    
    # Process each chassis
    for c in chassis_list:
        print(c)
        utils.print_verbose( f"Processing Chassis: {c['name']}" )

        # Process shared points - if there are any  
        if ( "sharedPts" in c ):
            shared_pts_list = c["sharedPts"]
            for pt in shared_pts_list:
                populatePoint( pt )

        # Process scanners
        if ( not "scanners" in c ):
            sys.exit( f"ERROR: Input file is missing 'scanner' object (chassis {c['name']})" )
        scanner_list = c["scanners"]
        for s in scanner_list:
            utils.print_verbose( f"  Processing Scanner: {s['name']}" )
            scanScanner( s )

        # Process Execution Sets
        if ( not "executionSets" in c ):
            sys.exit( f"ERROR: Input file is missing 'executionSets' object (chassis {c['name']})" )
        executionSets_list = c["executionSets"]
        for exeset in executionSets_list:
            utils.print_verbose( f"  Processing Scanner: {exeset['name']}" )
            scanExecutionSet( exeset )


# Resolve Point references (in the individual components)
# NOTE: This method is called after scanPoints() - so I know that a lot of JSON file is 'well-formed' (i.e. less error checking needed)
def resolvePointReferences( json_dict ):
    chassis_list = json_dict["chassis"]
    
    # Process each chassis
    for c in chassis_list:
        utils.print_verbose( f"Resolving Chassis References: {c['name']}" )

        # Process Execution Sets
        executionSets_list = c["executionSets"]
        for exeset in executionSets_list:
            utils.print_verbose( f"  Resolving ExecutionSet References: {exeset['name']}" )
            resolveExecutionSet( exeset )

#------------------------------------------------------------------------------
def add_guid( d ):
    if ( "typeName" in d ):
        new_entry = { "type" : fxt_types._types_dict[d["typeName"]] }
        d.update( new_entry )

def typenames_to_guids( json_dict ):
    try:
        # Node
        add_guid(json_dict)

        chassis_list = json_dict["chassis"]
        for ch in chassis_list:
            # Process shared points - if there are any  
            if ( "sharedPts" in ch ):
                shared_pts_list = ch["sharedPts"]
                for pt in shared_pts_list:
                    add_guid(pt)

            # Process scanners
            scanner_list = ch["scanners"]
            for s in scanner_list:
                # Cards
                card_list = s["cards"]
                for c in card_list:
                    add_guid(c)

                    points_obj = c["points"]
                
                    # Input points
                    if ( "inputs" in points_obj ):
                        inputs_list = points_obj["inputs"]
                        for channel in inputs_list:
                            add_guid( channel )

                    # Output points
                    if ( "outputs" in points_obj ):
                        outputs_list = points_obj["outputs"]
                        for channel in outputs_list:
                            add_guid( channel )



            # Process Execution Sets
            executionSets_list = ch["executionSets"]
            for exeset in executionSets_list:
                # Logic chains
                logicChains_list = exeset["logicChains"]
                for lc in logicChains_list:
                    # Components
                    components_list = lc["components"]
                    for comp in components_list:
                        add_guid( comp )

                        # Inputs Points
                        inputs_list = comp["inputs"]
                        for pt in inputs_list:
                            add_guid( pt )

                        # Output Points
                        inputs_list = comp["outputs"]
                        for pt in inputs_list:
                            add_guid( pt )

                    # Connector Points
                    if ( "connectionPts" in lc ):
                        conPts_list = lc["connectionPts"]
                        for pt in conPts_list:
                            add_guid( pt )

                    # Auto Points
                    if ( "autoPts" in lc ):
                        autoPts_list = lc["autoPts"]
                        for pt in autoPts_list:
                            add_guid( pt )

    except Exception as e:
        exit(f'Failed typeName to guid look up. {e}')


#------------------------------------------------------------------------------
def strip_unused_kvp_point( pt_obj ):
    if ( "typeName" in pt_obj ):
        del pt_obj["typeName"]
    if ( "name" in pt_obj ):
        del pt_obj["name"]
    if ( "idRefName" in pt_obj ):
        del pt_obj["idRefName"]

def strip_unused_kvp( json_dict ):
    # Strip top level Node KVPs
    if ( "name" in json_dict ):
        del json_dict["name"]
    if ( "typeName" in json_dict ):
        del json_dict["typeName"]

    chassis_list = json_dict["chassis"]
    for ch in chassis_list:
        # Strip top level chassis KVPs
        if ( "id" in ch ):
            del ch["id"]
        if ( "name" in ch ):
            del ch["name"]

        # Process shared points - if there are any  
        if ( "sharedPts" in ch ):
            shared_pts_list = ch["sharedPts"]
            for pt in shared_pts_list:
                strip_unused_kvp_point( pt )

        # Process scanners
        scanner_list = ch["scanners"]
        for s in scanner_list:
            # Strip top level scanner KVPs
            if ( "id" in s ):
                del s["id"]
            if ( "name" in s ):
                del s["name"]

            card_list = s["cards"]
            for c in card_list:
                # Strip top level cards KVPs
                if ( "id" in c ):
                    del c["id"]
                if ( "name" in c ):
                    del c["name"]
                if ( "typeName" in c ):
                    del c["typeName"]

                points_obj = c["points"]
                
                # Input points
                if ( "inputs" in points_obj ):
                    inputs_list = points_obj["inputs"]
                    for channel in inputs_list:
                        strip_unused_kvp_point( channel )

                # Output points
                if ( "outputs" in points_obj ):
                    outputs_list = points_obj["outputs"]
                    for channel in outputs_list:
                        strip_unused_kvp_point( channel )



        # Process Execution Sets
        executionSets_list = ch["executionSets"]
        for exeset in executionSets_list:
            # Strip top level exeset KVPs
            if ( "id" in exeset ):
                del exeset["id"]
            if ( "name" in exeset ):
                del exeset["name"]

            logicChains_list = exeset["logicChains"]
            for lc in logicChains_list:
                # Strip top level LogicChain KVPs
                if ( "id" in lc ):
                    del lc["id"]
                if ( "name" in lc ):
                    del lc["name"]

                components_list = lc["components"]
                for comp in components_list:
                    # Strip top level LogicChain KVPs
                    if ( "name" in comp ):
                        del comp["name"]
                    if ( "typeName" in comp ):
                        del comp["typeName"]

                    # Inputs Points
                    inputs_list = comp["inputs"]
                    for pt in inputs_list:
                        strip_unused_kvp_point( pt )

                    # Output Points
                    inputs_list = comp["outputs"]
                    for pt in inputs_list:
                        strip_unused_kvp_point( pt )

                # Connector Points
                if ( "connectionPts" in lc ):
                    conPts_list = lc["connectionPts"]
                    for pt in conPts_list:
                        strip_unused_kvp_point( pt )

                # Auto Points
                if ( "autoPts" in lc ):
                    autoPts_list = lc["autoPts"]
                    for pt in autoPts_list:
                        strip_unused_kvp_point( pt )


#------------------------------------------------------------------------------
def extract_guid_and_type( fname ):
    guid = None
    type = None
    guid_re = '.*GUID_STRING.*=.*"(.+?)".*;'
    type_re = '.*TYPE_NAME.*=.*"(.+?)".*;'
    with open( fname ) as f:
        for line in f:
            line = line.lstrip()
            m = re.search( guid_re, line )
            if ( m ):
                guid = m.group(1)
            else:
                m = re.search( type_re, line )
                if ( m ):
                    type = m.group(1)
            
    return ( guid, type )

#------------------------------------------------------------------------------
# BEGIN
if __name__ == '__main__':

    # Get script location
    script_dir = os.path.dirname(sys.argv[0])

    # Parse command line
    args = docopt(__doc__, version='0.0.1' )

    # Set quite & verbose modes
    utils.set_quite_mode( args['-w'] )
    utils.set_verbose_mode( args['-v'] )
    
    # Convert file
    if ( not args['--guids']  ):
        # Load input file
        json_dict = utils.load_node_file( args['<infile>'] )
        if ( json_dict == None ):
            sys.exit( f"ERROR: Can not open file: {args['<infile>']}, OR invalid JSON syntax" );

        # Set output file name
        root,ext  = os.path.splitext( args['<infile>'] )
        path,name = os.path.split( root )
        outfile   = os.path.join( path, name ) + ".id" + ext
        if ( args['<outfile>'] != None ):
            outfile = args['<outfile>']
    
        # Generate Point ID and resolve references
        scanPoints( json_dict )
        resolvePointReferences( json_dict )
    
        # Auto-populate the type GUIDs
        typenames_to_guids( json_dict )

        # Compact the json output by removing KVP not used by the firmware
        if ( args['-s'] ):
            strip_unused_kvp( json_dict )

        utils.write_node_file( json_dict, outfile, args['--pretty'], args['-e'] )
        if ( args['-c'] != None ):
            generate_define_list( args['-c'] )

    # Generate GUID dictionary
    else:
        # Get a list of header files
        file_list = utils.walk_file_list( "*.h", args['<srcpath>'] )
        fxttypes = {}
        for fname in file_list:
            g,t = extract_guid_and_type( fname )
            if ( g != None and t != None ):
                fxttypes[t] = g

        print(fxttypes)
        
        # Create type dictionary
        if ( args['--append'] ):
            fxttypes.update(fxt_types._types_dict)

        fname = os.path.join( script_dir, _dictionary_file )
        with open( fname, "w+" ) as fd:
            fd.write("# Known Foxtail types\n")
            fd.write( "_types_dict = {\n" )
            num_entries = len(fxttypes)
            i           = 1
            separator   = ",\n"
            for k in fxttypes:
                if ( i == num_entries ):
                    separator = "\n"
                fd.write(f'    "{k}": "{fxttypes[k]}"{separator}' )
                i = i + 1
            fd.write( "}" )
        
