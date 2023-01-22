#ifndef Fxt_Node_TShell_Node_h
#define Fxt_Node_TShell_Node_h
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "colony_config.h"
#include "Cpl/TShell/Cmd/Command.h"
#include "Fxt/Point/DatabaseApi.h"
#include "Fxt/Node/FactoryApi.h"



#define FXTNODETSHELL_VERB_NODE_        "node"

/** Usage
                                        "         1         2         3         4         5         6         7         8"
                                        "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
*/
#define FXTNODETSHELL_USAGE_NODE_       "node [verbose]\n" \
                                        "node start|stop\n" \
                                        "node download\n" \
                                        "node DELETE" \

/// Detailed Help text
#ifndef FXTNODETSHELL_DETAIL_NODE_
#define FXTNODETSHELL_DETAIL_NODE_      "  Display the current state of the node and allows the user to start/stop the\n" \
                                        "  node. To download a node definition at runtime - enter the 'download' sub-\n" \
                                        "  command followed by a carriage return, then the JSON text for the node\n" \
                                        "  definition.  Entering a Ctrl-Q + newline will terminate a stalled download"

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue


/// Size, in bytes, of buffer to capture the Node JSON file
#ifndef OPTION_FXT_NODE_TSHELL_NODE_MAX_JSON_FILE_SIZE
#define OPTION_FXT_NODE_TSHELL_NODE_MAX_JSON_FILE_SIZE      (8*1024)
#endif

/// Size, in bytes, of the Binary JSON document
#ifndef OPTION_FXT_NODE_TSHELL_NODE_MAX_JSON_DOC_SIZE
#define OPTION_FXT_NODE_TSHELL_NODE_MAX_JSON_DOC_SIZE       (8*1024)
#endif

///
namespace Fxt {
///
namespace Node {
///
namespace TShell {



/** This class implements a TShell command.  
 */
class Node : public Cpl::TShell::Cmd::Command
{
public:
    /// See Cpl::TShell::Command                                                               
    const char* getUsage() const noexcept { return FXTNODETSHELL_USAGE_NODE_; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return FXTNODETSHELL_DETAIL_NODE_; }


public:
    /// Constructor
    Node( Cpl::Container::Map<Cpl::TShell::Command>& commandList, 
          Fxt::Node::FactoryApi&                     nodeFactory,
          Fxt::Point::DatabaseApi&                   pointDb ) noexcept;


public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

protected:
    /// Reference to the Node Factory
    Fxt::Node::FactoryApi&      m_nodeFactory;

    /// Reference to the Point Database
    Fxt::Point::DatabaseApi&    m_pointDb;

    /// Memory to hold binary JSON definition
    StaticJsonDocument<OPTION_FXT_NODE_TSHELL_NODE_MAX_JSON_DOC_SIZE> m_jsonDoc;

    /// JSON file buffer
    uint8_t m_jsonBuffer[OPTION_FXT_NODE_TSHELL_NODE_MAX_JSON_FILE_SIZE+1];
};

};      // end namespaces
};
};
#endif  // end header latch
