#ifndef Fxt_Point_TShell_Pt_h
#define Fxt_Point_TShell_Pt_h
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



#define FXTPOINTTSHELL_VERB_PT_         "pt"

/** Usage
                                        "         1         2         3         4         5         6         7         8"
                                        "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
*/
#define FXTPOINTTSHELL_USAGE_PT_        "pt\n" \
                                        "pt write {<pt-json>}\n" \
                                        "pt read <id>"

/// Detailed Help text
#ifndef FXTPOINTTSHELL_DETAIL_PT_
#define FXTPOINTTSHELL_DETAIL_PT_       "  Lists, updates, and displays Points contained in the Model Database.\n" \
                                        "  Updating a Point is done by specifying a JSON object. See the concrete class\n" \
                                        "  definition of the Point being updated for the JSON format."

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue



///
namespace Fxt {
///
namespace Point {
///
namespace TShell {



/** This class implements a TShell command.  
 */
class Pt : public Cpl::TShell::Cmd::Command
{
protected:
    /// Point Database to access
    Fxt::Point::DatabaseApi& m_database;

    /// Dynamic 
public:
    /// See Cpl::TShell::Command                                                               `
    const char* getUsage() const noexcept { return FXTPOINTTSHELL_USAGE_PT_; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return FXTPOINTTSHELL_DETAIL_PT_; }


public:
    /// Constructor
    Pt( Cpl::Container::Map<Cpl::TShell::Command>& commandList, Fxt::Point::DatabaseApi& modelDatabase ) noexcept;


public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

};

};      // end namespaces
};
};
#endif  // end header latch
