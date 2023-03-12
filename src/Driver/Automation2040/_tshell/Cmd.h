#ifndef Driver_Automation2040_TShell_Cmd_h
#define Driver_Automation2040_TShell_Cmd_h
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


///
namespace Driver {
///
namespace Automation2040 {

/** This class implements a TShell command that exercises the Automation2040
    driver. The class is ABSOLUTELY NOT THREAD SAFE!  Its intended use is to 
    support unit testing.
 */
class TShellCmd : public Cpl::TShell::Cmd::Command
{
public:
    /// The command verb/identifier
    static constexpr const char* verb = "2040";

    /// The command usage string
    static constexpr const char* usage = "2040\n"
                                         "2040 start|stop\n"
                                         "2040 setsafe\n"
                                         "2040 out|relay <N> 0|1\n"
                                         "2040 adcled <N> enable|disable>\n"
                                         "2040 buttonled <N> <percent>\n"
                                         "2040 connled <percent>";


    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                                                          1         2         3         4         5         6         7         8
                                                 12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp = "  Exercises the Automation2040 Driver.  The command with no arguments displays\n"
                                                "  the state of the driver as well as the current input values. The user is\n"
                                                "  required to manually start (and stop) the driver for proper operation.";
protected:
    /// See Cpl::TShell::Command
    const char* getUsage() const noexcept { return usage; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return detailedHelp; }


public:
    /// Constructor
    TShellCmd( Cpl::Container::Map<Cpl::TShell::Command>& commandList,
               Cpl::TShell::Security::Permission_T        minPermLevel=OPTION_TSHELL_CMD_COMMAND_DEFAULT_PERMISSION_LEVEL ) noexcept;

public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

};

};      // end namespaces
};
#endif  // end header latch
