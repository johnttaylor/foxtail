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

#include "Cmd.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/System/Api.h"
#include "Driver/Automation2040/Api.h"
#include <string.h>         

///
using namespace Driver::Automation2040;


#define ADC_CHANNEL_TO_ADC_ENUM(c)      ((Driver::Automation2040::Api::AInputId_T)((c)+Driver::Automation2040::Api::eAINPUT_1-1))
#define OD_CHANNEL_TO_OD_ENUM(c)        ((Driver::Automation2040::Api::DOutputId_T)((c)+Driver::Automation2040::Api::eDOUTPUT_1-1))    
#define RELAY_CHANNEL_TO_RELAY_ENUM(c)  ((Driver::Automation2040::Api::RelayId_T)((c)+Driver::Automation2040::Api::eRELAY_1-1))    

///////////////////////////
TShellCmd::TShellCmd( Cpl::Container::Map<Cpl::TShell::Command>& commandList,
                      Cpl::TShell::Security::Permission_T        minPermLevel ) noexcept
    :Command( commandList, verb, minPermLevel )
{
}


///////////////////////////
static bool display_value( Cpl::Text::String& buffer, Cpl::TShell::Context_& context, const char* label, bool value )
{
    buffer.format( "%-18s  %d", label, value );
    return context.writeFrame( buffer.getString() );
}
static bool display_value( Cpl::Text::String& buffer, Cpl::TShell::Context_& context, const char* label, float value )
{
    buffer.format( "%-18s  %g", label, value );
    return context.writeFrame( buffer.getString() );
}

Cpl::TShell::Command::Result_T TShellCmd::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    Cpl::Text::String&              outtext  = context.getOutputBuffer();
    bool                            io       = true;

    // Start
    if ( tokens.numParameters() == 2 && strcmp( tokens.getParameter( 1 ), "start" ) == 0 )
    {
        Driver::Automation2040::Api::start();
        io = context.writeFrame( "Driver started" );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Stop
    if ( tokens.numParameters() == 2 && strcmp( tokens.getParameter( 1 ), "stop" ) == 0 )
    {
        Driver::Automation2040::Api::stop();
        io = context.writeFrame( "Driver stopped" );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // SetSafe
    if ( tokens.numParameters() == 2 && strcmp( tokens.getParameter( 1 ), "setsafe" ) == 0 )
    {
        Driver::Automation2040::Api::setSafe();
        io = context.writeFrame( "Driver put into its safe state" );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Display state/inputs
    if ( tokens.numParameters() == 1 )
    {
        io &= display_value( outtext, context, "ADC1 (v)", Driver::Automation2040::Api::getAnalogValue( Driver::Automation2040::Api::eAINPUT_1 ) );
        io &= display_value( outtext, context, "ADC2 (v)", Driver::Automation2040::Api::getAnalogValue( Driver::Automation2040::Api::eAINPUT_2 ) );
        io &= display_value( outtext, context, "ADC3 (v)", Driver::Automation2040::Api::getAnalogValue( Driver::Automation2040::Api::eAINPUT_3 ) );
        io &= display_value( outtext, context, "DIN1", Driver::Automation2040::Api::getInputState( Driver::Automation2040::Api::eDINPUT_1 ) );
        io &= display_value( outtext, context, "DIN2", Driver::Automation2040::Api::getInputState( Driver::Automation2040::Api::eDINPUT_2 ) );
        io &= display_value( outtext, context, "DIN3", Driver::Automation2040::Api::getInputState( Driver::Automation2040::Api::eDINPUT_3 ) );
        io &= display_value( outtext, context, "DIN4", Driver::Automation2040::Api::getInputState( Driver::Automation2040::Api::eDINPUT_4 ) );
        io &= display_value( outtext, context, "ButtonA", Driver::Automation2040::Api::userButtonPressed( Driver::Automation2040::Api::eBOARD_BUTTON_A ) );
        io &= display_value( outtext, context, "ButtonB", Driver::Automation2040::Api::userButtonPressed( Driver::Automation2040::Api::eBOARD_BUTTON_B ) );
        io &= display_value( outtext, context, "Board Temp 'C", Driver::Automation2040::Api::getBoardTemperature() );
        io &= display_value( outtext, context, "DO1", Driver::Automation2040::Api::getOutputState( Driver::Automation2040::Api::eDOUTPUT_1 ) );
        io &= display_value( outtext, context, "DO2", Driver::Automation2040::Api::getOutputState( Driver::Automation2040::Api::eDOUTPUT_2 ) );
        io &= display_value( outtext, context, "DO3", Driver::Automation2040::Api::getOutputState( Driver::Automation2040::Api::eDOUTPUT_3 ) );
        io &= display_value( outtext, context, "Relay1", Driver::Automation2040::Api::getRelayState( Driver::Automation2040::Api::eRELAY_1 ) );
        io &= display_value( outtext, context, "Relay2", Driver::Automation2040::Api::getRelayState( Driver::Automation2040::Api::eRELAY_2 ) );
        io &= display_value( outtext, context, "Relay3", Driver::Automation2040::Api::getRelayState( Driver::Automation2040::Api::eRELAY_3 ) );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Enable/Disable ADC LEDs
    if ( tokens.numParameters() == 4 && *(tokens.getParameter( 1 )) == 'a' )
    {
        // Enable/disable state
        bool enable = true;
        if ( *(tokens.getParameter( 3 )) == 'd' )
        {
            enable = false;
        }

        // Channel number
        int adcChannel;
        if ( Cpl::Text::a2i( adcChannel, tokens.getParameter( 2 ) ) == false || adcChannel > 3 || adcChannel <= 0 )
        {
            outtext.format( "Invalid ADC Channel (%s). Range is: 1-3", tokens.getParameter( 2 ) );
            context.writeFrame( outtext.getString() );
            return Command::eERROR_INVALID_ARGS;
        }

        Driver::Automation2040::Api::setAdcLEDBehavior( ADC_CHANNEL_TO_ADC_ENUM( adcChannel ), enable );
        outtext.format( "LED %s for ADC Channel %s", enable? "enabled": "disabled", tokens.getParameter( 2 ) );
        io = context.writeFrame( outtext.getString() );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Button LEDs
    if ( tokens.numParameters() == 4 && *(tokens.getParameter( 1 )) == 'b' )
    {
        // Button ID
        if ( *(tokens.getParameter( 2 )) != 'a' && *(tokens.getParameter( 2 )) != 'b' )
        {
            outtext.format( "Invalid Button Channel (%s). Must be: 'a' or 'b'", tokens.getParameter( 2 ) );
            context.writeFrame( outtext.getString() );
            return Command::eERROR_INVALID_ARGS;
        }
        Driver::Automation2040::Api::ButtonId_T buttonChannel = Driver::Automation2040::Api::eBOARD_BUTTON_A;
        if ( *(tokens.getParameter( 2 )) == 'b' )
        {
            buttonChannel = Driver::Automation2040::Api::eBOARD_BUTTON_B;
        }

        // Percent on
        unsigned percentOn;
        if ( Cpl::Text::a2ui( percentOn, tokens.getParameter( 3 ) ) == false || percentOn > 100 )
        {
            outtext.format( "Invalid <percent> argument (%s). Range is: 0-100", tokens.getParameter( 3 ) );
            context.writeFrame( outtext.getString() );
            return Command::eERROR_INVALID_ARGS;
        }

        Driver::Automation2040::Api::setButtonLED( buttonChannel, (float) percentOn );
        outtext.format( "Button LED %s set to %s percent on", tokens.getParameter( 2 ), tokens.getParameter( 3 ) );
        context.writeFrame( outtext.getString() );
        return Command::eSUCCESS;
    }

    // Digital Output
    if ( tokens.numParameters() == 4 && *(tokens.getParameter( 1 )) == 'o' )
    {
        // Output ID
        int doChannel;
        if ( Cpl::Text::a2i( doChannel, tokens.getParameter( 2 ) ) == false || doChannel > 3 || doChannel <= 0 )
        {
            outtext.format( "Invalid Digital Output Channel (%s). Range is: 1-3", tokens.getParameter( 2 ) );
            context.writeFrame( outtext.getString() );
            return Command::eERROR_INVALID_ARGS;
        }

        // On/off
        if ( *(tokens.getParameter( 3 )) != '1' && *(tokens.getParameter( 3 )) != '0' )
        {
            outtext.format( "Invalid Digital Output state (%s). Must be: '0' or '1'", tokens.getParameter( 3 ) );
            context.writeFrame( outtext.getString() );
            return Command::eERROR_INVALID_ARGS;
        }
        bool outstate = true;
        if ( *(tokens.getParameter( 3 )) == '0' )
        {
            outstate = false;
        }

        Driver::Automation2040::Api::setOutputState( OD_CHANNEL_TO_OD_ENUM(doChannel), outstate );
        outtext.format( "Digital output %s set to %s", tokens.getParameter( 2 ), tokens.getParameter( 3 ) );
        context.writeFrame( outtext.getString() );
        return Command::eSUCCESS;
    }

    // Relay Output
    if ( tokens.numParameters() == 4 && *(tokens.getParameter( 1 )) == 'r' )
    {
        // Relay ID
        int relayChannel;
        if ( Cpl::Text::a2i( relayChannel, tokens.getParameter( 2 ) ) == false || relayChannel > 3 || relayChannel <= 0 )
        {
            outtext.format( "Invalid DI Channel (%s). Range is: 1-3", tokens.getParameter( 2 ) );
            context.writeFrame( outtext.getString() );
            return Command::eERROR_INVALID_ARGS;
        }

        // On/off
        if ( *(tokens.getParameter( 3 )) != '1' && *(tokens.getParameter( 3 )) != '0' )
        {
            outtext.format( "Invalid Relay state (%s). Must be: '0' or '1'", tokens.getParameter( 3 ) );
            context.writeFrame( outtext.getString() );
            return Command::eERROR_INVALID_ARGS;
        }
        bool outstate = true;
        if ( *(tokens.getParameter( 3 )) == '0' )
        {
            outstate = false;
        }

        Driver::Automation2040::Api::setRelayState( RELAY_CHANNEL_TO_RELAY_ENUM( relayChannel ), outstate );
        outtext.format( "Relay %s set to %s", tokens.getParameter( 2 ), tokens.getParameter( 3 ) );
        context.writeFrame( outtext.getString() );
        return Command::eSUCCESS;
    }

    // Copnection LED
    if ( tokens.numParameters() == 3 && *(tokens.getParameter( 1 )) == 'c' )
    {
        // Percent on
        unsigned percentOn;
        if ( Cpl::Text::a2ui( percentOn, tokens.getParameter( 2 ) ) == false || percentOn > 100 )
        {
            outtext.format( "Invalid <percent> argument (%s). Range is: 0-100", tokens.getParameter( 2 ) );
            context.writeFrame( outtext.getString() );
            return Command::eERROR_INVALID_ARGS;
        }

        Driver::Automation2040::Api::setConnectedLED( (float) percentOn );
        outtext.format( "Connection LED set to %s percent on", tokens.getParameter( 2 ) );
        context.writeFrame( outtext.getString() );
        return Command::eSUCCESS;
    }

    // If I get here -->the argument(s) where bad
    return Cpl::TShell::Command::eERROR_INVALID_ARGS;
}

