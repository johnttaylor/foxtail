#ifndef Fxt_Card_Mock_TShell_Dio8_h
#define Fxt_Card_Mock_TShell_Dio8_h
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
#include "Fxt/Card/Mock/Digital8.h"



#define FXTCARDMOCKTSHELL_VERB_DIO8_        "dio8"

/** Usage
                                            "         1         2         3         4         5         6         7         8"
                                            "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
*/
#define FXTCARDMOCKTSHELL_USAGE_DIO8_       "dio8\n" \
                                            "dio8 setcard <chassisIdx> <slot-num> \n" \
                                            "dio8 write <byte>\n" \
                                            "dio8 write <bit-idx> <bitValue>\n" \
                                            "dio8 invalid in|out" \

/// Detailed Help text
#ifndef FXTCARDMOCKTSHELL_DETAIL_DIO8_
#define FXTCARDMOCKTSHELL_DETAIL_DIO8_      "  Reads and writes the mock cards's inputs.  The user must first identify\n" \
                                            "  which card instance to operate on by using the 'setcard' subcommand"

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue



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
class Dio8 : public Cpl::TShell::Cmd::Command
{
public:
    /// See Cpl::TShell::Command                                                               `
    const char* getUsage() const noexcept { return FXTCARDMOCKTSHELL_USAGE_DIO8_; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return FXTCARDMOCKTSHELL_DETAIL_DIO8_; }


public:
    /// Constructor
    Dio8( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept;


public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

protected:
    /// Pointer to the current card
    Fxt::Card::Mock::Digital8* m_curCard;
};

};      // end namespaces
};
};
};
#endif  // end header latch
