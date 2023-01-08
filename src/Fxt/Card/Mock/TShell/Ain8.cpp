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

#include "Ain8.h"
#include "Fxt/Card/Mock/AnalogIn8.h"
#include "Fxt/Node/Api.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include <string.h>

///
using namespace Fxt::Card::Mock::TShell;


///////////////////////////
Ain8::Ain8( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept
    : Cpl::TShell::Cmd::Command( commandList, FXTCARDMOCKTSHELL_VERB_AIN8_ )
    , m_curCard( nullptr )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T Ain8::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    Cpl::Text::String&              outtext = context.getOutputBuffer();
    bool                            io      = true;

    // Get handle to the node
    Fxt::Node::Api* node = Fxt::Node::Api::getNode();
    if ( node == nullptr )
    {
        context.writeFrame( "ERROR: No valid/working Node instanced defined." );
        return Command::eERROR_FAILED;
    }

    // Set Card
    if ( tokens.numParameters() == 4 && *(tokens.getParameter( 1 )) == 's' )
    {
        // Chassis Index
        unsigned           chassisIdx;
        Fxt::Chassis::Api* chassis;
        if ( Cpl::Text::a2ui( chassisIdx, tokens.getParameter( 2 ) ) == false || (chassis=node->getChassis( chassisIdx )) == nullptr )
        {
            outtext.format( "ERROR: invalid chassis index (%s)", tokens.getParameter( 2 ) );
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }

        // Slot number
        unsigned        slotNum;
        Fxt::Card::Api* card;
        if ( Cpl::Text::a2ui( slotNum, tokens.getParameter( 3 ) ) == false || (card=Fxt::Chassis::Api::getCard( *chassis, slotNum )) == nullptr )
        {
            outtext.format( "ERROR: invalid slot-number (%s)", tokens.getParameter( 3 ) );
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }

        // Is the card the correct type?
        if ( strcmp( card->getTypeGuid(), Fxt::Card::Mock::AnalogIn8::GUID_STRING ) != 0 )
        {
            outtext.format( "ERROR: Specified card is not an AnalogIn8 card (returned type: %s)", card->getTypeName() );
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }

        m_curCard = card;
        io &= context.writeFrame( "Card successfully found and set" );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // If I get here an unsupported sub-command/syntax was specified
    return Command::eERROR_BAD_SYNTAX;
}