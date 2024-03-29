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

#include "Dio8.h"
#include "Fxt/Node/Api.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include <string.h>

///
using namespace Fxt::Card::Mock::TShell;


///////////////////////////
Dio8::Dio8( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept
    : Cpl::TShell::Cmd::Command( commandList, FXTCARDMOCKTSHELL_VERB_DIO8_ )
    , m_curCard( nullptr )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T Dio8::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    Cpl::Text::String&              outtext  = context.getOutputBuffer();
    Cpl::Text::String&              smallBuf = context.getTokenBuffer();
    bool                            io       = true;

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
        if ( strcmp( card->getTypeGuid(), Fxt::Card::Mock::Digital8::GUID_STRING ) != 0 )
        {
            outtext.format( "ERROR: Specified card is not an Digtial8 card (returned type: %s)", card->getTypeName() );
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }

        m_curCard = (Fxt::Card::Mock::Digital8 *) card;
        io &= context.writeFrame( "Card successfully found and set" );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // All other commands require that a card be set
    if ( m_curCard == nullptr )
    {
        context.writeFrame( "ERROR: No card set. Type 'help dio8' for additional details." );
        return Command::eERROR_FAILED;
    }

    // Display Card state
    if ( tokens.numParameters() == 1 )
    {
        // Get Input values
        uint8_t value;
        bool  valid = m_curCard->getInputs( value );
        if ( valid )
        {
            Cpl::Text::bufferToAsciiBinary( &value, sizeof( value ), smallBuf );
            outtext.format( "Inputs:   %s", smallBuf.getString() );
        }
        else
        {
            outtext.format( "Inputs:   <invalid>" );
        }
        io &= context.writeFrame( outtext );
        
        // Get Output values
        valid = m_curCard->getOutputs( value );
        if ( valid )
        {
            Cpl::Text::bufferToAsciiBinary( &value, sizeof( value ), smallBuf );
            outtext.format( "Outputs:  %s", smallBuf.getString() );
        }
        else
        {
            outtext.format( "Outputs:  <invalid>" );
        }
        io &= context.writeFrame( outtext );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Write Input Byte value
    if ( tokens.numParameters() == 3 && *(tokens.getParameter( 1 )) == 'w' )
    {
        // Get byte to write (allow hex values)
        unsigned value;
        if ( Cpl::Text::a2ui( value, tokens.getParameter( 2 ), 0 ) == false || value > 0xFF )
        {
            outtext.format( "ERROR: invalid byte value (%s)", tokens.getParameter( 2 ) );
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }


        m_curCard->writeInputs( value );
        Cpl::Text::bufferToAsciiBinary( &value, sizeof( uint8_t ), smallBuf );
        outtext.format( "Inputs set to: %s", smallBuf.getString() );
        io &= context.writeFrame( outtext );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Write Input Bit value
    if ( tokens.numParameters() == 4 && *(tokens.getParameter( 1 )) == 'w' )
    {
        // Get bit offset to write
        unsigned bitOffset;
        if ( Cpl::Text::a2ui( bitOffset, tokens.getParameter( 2 ) ) == false || bitOffset > 7 )
        {
            outtext.format( "ERROR: invalid bit offset (%s)", tokens.getParameter( 2 ) );
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }

        // Get bit value 
        unsigned bitValue;
        if ( Cpl::Text::a2ui( bitValue, tokens.getParameter( 3 ) ) == false || bitValue > 1 )
        {
            outtext.format( "ERROR: invalid bit value (%s)", tokens.getParameter( 3 ) );
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }

        // Write the input value
        m_curCard->writeInput( bitOffset, bitValue );

        uint8_t newValue = 0;
        m_curCard->getInputs( newValue );
        Cpl::Text::bufferToAsciiBinary( &newValue, sizeof( newValue ), smallBuf );
        outtext.format( "Inputs updated to: %s", smallBuf.getString() );
        io &= context.writeFrame( outtext );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Invalid inputs or outputs
    if ( tokens.numParameters() == 3 && *(tokens.getParameter( 1 )) == 'i' )
    {
        // Inputs
        if ( *(tokens.getParameter( 1 )) == 'i' )
        {
            m_curCard->setInputsInvalid();
            io &= context.writeFrame( "Inputs have been invalidate" );
        }
        else
        {
            m_curCard->setOutputsInvalid();
            io &= context.writeFrame( "Outputs have been invalidate" );
        }
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }
  

    // If I get here an unsupported sub-command/syntax was specified
    return Command::eERROR_BAD_SYNTAX;
}