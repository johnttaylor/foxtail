#ifndef Fxt_Card_Mock_TShell_Aout8_h
#define Fxt_Card_Mock_TShell_Aout8_h
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
#include "Fxt/Card/Mock/AnalogOut8.h"


///
namespace Fxt {
///
namespace Card {
///
namespace Mock {
///
namespace TShell {


/** This class implements a TShell command.  
 */
class Aout8 : public Cpl::TShell::Cmd::Command
{
public:
    /// The command verb/identifier
    static constexpr const char* verb = "aout8";

    /// The command usage string
    static constexpr const char* usage = "aout8\n"
                                         "aout8 setcard <chassisIdx> <slot - num>";

    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                                                          1         2         3         4         5         6         7         8
                                                 12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp = "  Reads the mock cards's outputs.  The user must first identify which card\n"
                                                "  instance to operate on by using the 'setcard' subcommand";

public:
    /// See Cpl::TShell::Command                                                               `
    const char* getUsage() const noexcept { return usage; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return detailedHelp; }


public:
    /// Constructor
    Aout8( Cpl::Container::Map<Cpl::TShell::Command>& commandList,
           Cpl::TShell::Security::Permission_T        minPermLevel=OPTION_TSHELL_CMD_COMMAND_DEFAULT_PERMISSION_LEVEL ) noexcept;


public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

protected:
    /// Pointer to the current card
    Fxt::Card::Mock::AnalogOut8* m_curCard;
};

};      // end namespaces
};
};
};
#endif  // end header latch
