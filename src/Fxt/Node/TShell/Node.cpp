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

#include "Node.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/strip.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/Json/ObjectDetector.h"
#include <string.h>
#include "Fxt/Node/Api.h"
#include "Fxt/System/ElapsedTime.h"

///
using namespace Fxt::Node::TShell;


///////////////////////////
Node::Node( Cpl::Container::Map<Cpl::TShell::Command>& commandList,
            Fxt::Node::FactoryApi&                     nodeFactory,
            Fxt::Point::DatabaseApi&                   pointDb ) noexcept
    : Cpl::TShell::Cmd::Command( commandList, FXTNODETSHELL_VERB_NODE_ )
    , m_nodeFactory( nodeFactory )
    , m_pointDb( pointDb )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T Node::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    Cpl::Text::String&              outtext  = context.getOutputBuffer();
    Cpl::Text::String&              smallBuf = context.getTokenBuffer();
    bool                            io       = true;

    // Get handle to the node
    Fxt::Node::Api* node = Fxt::Node::Api::getNode();

    // Delete Node
    if ( tokens.numParameters() == 2 && strcmp(tokens.getParameter( 1 ), "DELETE") == 0 )
    {
        // Fail if there is no node
        if ( node == nullptr )
        {
            context.writeFrame( "ERROR: No valid/working Node instanced defined." );
            return Command::eERROR_FAILED;
        }

        // Ensure the node is stop then destroy the node
        if ( node->isStarted() )
        {
            context.writeFrame( "The Node is in the started state.  The Node must be stopped before deleting it" );
            return Command::eERROR_FAILED;
        }

        m_pointDb.clearPoints();    // Must clear BEFORE destroying the node
        m_nodeFactory.destroy( *node );
        io &= context.writeFrame( "Node deleted." );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Start Node
    if ( tokens.numParameters() == 2 && strcmp( tokens.getParameter( 1 ), "start" ) == 0 )
    {
        // Fail if there is no node
        if ( node == nullptr )
        {
            context.writeFrame( "ERROR: No valid/working Node instanced defined." );
            return Command::eERROR_FAILED;
        }

        // Fail if the node is already started
        if ( node->isStarted() )
        {
            context.writeFrame( "ERROR: Node is already started." );
            return Command::eERROR_FAILED;
        }

        // Start the node
        bool result = node->start( Fxt::System::ElapsedTime::now() );
        if ( !result )
        {
            outtext.format( "ERROR: Node failed to start (%s)", node->getErrorCode().toText( smallBuf ) );
            context.writeFrame( outtext );
            return Command::eERROR_FAILED;
        }

        io &= context.writeFrame( "Node started." );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Stop Node
    if ( tokens.numParameters() == 2 && strcmp( tokens.getParameter( 1 ), "stop" ) == 0 )
    {
        // Fail if there is no node
        if ( node == nullptr )
        {
            context.writeFrame( "ERROR: No valid/working Node instanced defined." );
            return Command::eERROR_FAILED;
        }

        // Fail if the node is already stopped
        if ( !node->isStarted() )
        {
            context.writeFrame( "ERROR: Node is already stopped." );
            return Command::eERROR_FAILED;
        }

        // Stop the node
        node->stop();
        io &= context.writeFrame( "Node stopped." );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Display node status
    if ( tokens.numParameters() == 1 ||
         (tokens.numParameters() == 2 && *tokens.getParameter( 1 ) == 'v') )
    {
        if ( node == nullptr )
        {
            outtext.format( "Node Factory Type:  %s", m_nodeFactory.getTypeName() );
            io &= context.writeFrame( outtext );
            io &= context.writeFrame( "No Node is current defined." );
            return io ? Command::eSUCCESS : Command::eERROR_IO;
        }

        // Node exists
        uint16_t numChassis = node->getNumChassis();
        outtext.format( "Node Type:      %s", node->getTypeName() );
        io &= context.writeFrame( outtext );
        outtext.format( "Node state:     %s", node->getErrorCode().toText( smallBuf ) );
        io &= context.writeFrame( outtext );
        outtext.format( "Type:           %s", node->getTypeName() );
        io &= context.writeFrame( outtext );
        outtext.format( "Started:        %s", node->isStarted() ? "YES" : "no" );
        io &= context.writeFrame( outtext );
        outtext.format( "Chassis count:  %u", numChassis );
        io &= context.writeFrame( outtext );

        if ( tokens.numParameters() > 1 )
        {
            // Iterate over the chassis
            for ( uint16_t chassisIdx=0; chassisIdx < numChassis; chassisIdx++ )
            {
                Fxt::Chassis::Api* chassis = node->getChassis( chassisIdx );
                if ( chassis != nullptr )
                {
                    outtext.format( "  Chassis Index: %u", chassisIdx );
                    io &= context.writeFrame( outtext );
                    outtext.format( "    State:     %s", chassis->getErrorCode().toText( smallBuf ) );
                    io &= context.writeFrame( outtext );
                    outtext.format( "    FER:       %llu", chassis->getFER() );
                    io &= context.writeFrame( outtext );
                    uint16_t numScanners = chassis->getNumScanners();
                    outtext.format( "    Scanners:  %u", numScanners );
                    io &= context.writeFrame( outtext );

                    // Iterate over the Scanners
                    for ( uint16_t scannerIdx=0; scannerIdx < numScanners; scannerIdx++ )
                    {
                        Fxt::Chassis::ScannerApi* scanner = chassis->getScanner( scannerIdx );
                        if ( scanner != nullptr )
                        {
                            outtext.format( "      Scanner Index: %u", scannerIdx );
                            io &= context.writeFrame( outtext );
                            outtext.format( "        State:  %s", scanner->getErrorCode().toText( smallBuf ) );
                            io &= context.writeFrame( outtext );
                            outtext.format( "        SRM:    %lu", scanner->getScanRateMultiplier() );
                            io &= context.writeFrame( outtext );
                            uint16_t numCards = scanner->getNumCards();
                            outtext.format( "        Cards:  %u", numCards );
                            io &= context.writeFrame( outtext );

                            // Iterate over the Cards
                            for ( uint16_t cardIdx=0; cardIdx < numCards; cardIdx++ )
                            {
                                Fxt::Card::Api* card = scanner->getCard( cardIdx );
                                if ( card != nullptr )
                                {
                                    outtext.format( "          Card Index: %u", cardIdx );
                                    io &= context.writeFrame( outtext );
                                    outtext.format( "            State: %s", card->getErrorCode().toText( smallBuf ) );
                                    io &= context.writeFrame( outtext );
                                    outtext.format( "            Type:  %s", card->getTypeName() );
                                    io &= context.writeFrame( outtext );
                                    outtext.format( "            Slot:  %u", card->getSlotNumber() );
                                    io &= context.writeFrame( outtext );
                                }
                            }
                        }
                    }

                    uint16_t numExeSets = chassis->getNumExecutionSets();
                    outtext.format( "    Num ExeSets:   %u", numExeSets );
                    io &= context.writeFrame( outtext );

                    // Iterate over the Execution Sets
                    for ( uint16_t exeSetIdx=0; exeSetIdx < numExeSets; exeSetIdx++ )
                    {
                        Fxt::Chassis::ExecutionSetApi* exeSet = chassis->getExecutionSet( exeSetIdx );
                        if ( exeSet != nullptr )
                        {
                            outtext.format( "      ExeSet Index: %u", exeSetIdx );
                            io &= context.writeFrame( outtext );
                            outtext.format( "        State:         %s", exeSet->getErrorCode().toText( smallBuf ) );
                            io &= context.writeFrame( outtext );
                            outtext.format( "        ERM:           %lu", exeSet->getExecutionRateMultiplier() );
                            io &= context.writeFrame( outtext );

                            uint16_t numChains = exeSet->getNumLogicChains();
                            outtext.format( "        Logic Chains:  %u", numChains );
                            io &= context.writeFrame( outtext );

                            // Iterate over the Logic Chains
                            for ( uint16_t chainIdx=0; chainIdx < numChains; chainIdx++ )
                            {
                                Fxt::LogicChain::Api* chain = exeSet->getLogicChain( chainIdx );
                                if ( chain != nullptr )
                                {
                                    outtext.format( "          Logic Chain Index: %u", chainIdx );
                                    io &= context.writeFrame( outtext );
                                    outtext.format( "            State:       %s", chain->getErrorCode().toText( smallBuf ) );
                                    io &= context.writeFrame( outtext );


                                    uint16_t numComponents = chain->getNumComponents();
                                    outtext.format( "            Components:  %u", numComponents );
                                    io &= context.writeFrame( outtext );

                                    // Iterate over the Components
                                    for ( uint16_t componentIdx=0; componentIdx < numComponents; componentIdx++ )
                                    {
                                        Fxt::Component::Api* component = chain->getComponent( componentIdx );
                                        if ( component != nullptr )
                                        {
                                            outtext.format( "              Component Index: %u", componentIdx );
                                            io &= context.writeFrame( outtext );
                                            outtext.format( "                State:  %s", component->getErrorCode().toText( smallBuf ) );
                                            io &= context.writeFrame( outtext );
                                            outtext.format( "                Type:   %s", component->getTypeName() );
                                            io &= context.writeFrame( outtext );
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // DOWNLOAD
    if ( tokens.numParameters() == 2 && *tokens.getParameter( 1 ) == 'd' )
    {
        // Cannot download if a node already exists
        if ( node )
        {
            context.writeFrame( "ERROR: Node Already exists (you must stop and delete it before downloading)." );
            return Command::eERROR_FAILED;
        }

        // Download via an Out-of-Bounds read
        Cpl::Json::ObjectDetector detector;
        uint8_t* dstBufPtr      = m_jsonBuffer;
        int      remainingBytes = OPTION_FXT_NODE_TSHELL_NODE_MAX_JSON_FILE_SIZE;
        int      bytesRead;
        while ( context.oobRead( dstBufPtr, remainingBytes, bytesRead ) )
        {
            // Null terminate the current input
            dstBufPtr[bytesRead] = '\0';

            size_t startPosition;
            size_t endPosition;

            if ( detector.scan( dstBufPtr, bytesRead, startPosition, endPosition ) )
            {
                DeserializationError err = deserializeJson( m_jsonDoc, m_jsonBuffer + startPosition, endPosition - startPosition + 1 );
                if ( err != DeserializationError::Ok )
                {
                    outtext.format( "ERROR: JSON object invalid: %s", err.c_str() );
                    context.writeFrame( outtext );
                    return Command::eERROR_FAILED;
                }

                JsonVariant      nodeJsonObj = m_jsonDoc.as<JsonVariant>();
                Fxt::Type::Error nodeError;
                Fxt::Node::Api* uut = m_nodeFactory.createFromJSON( nodeJsonObj,
                                                                    m_pointDb,
                                                                    nodeError );
                if ( uut == nullptr )
                {
                    // Clean up after the failure
                    m_pointDb.cleanupPointsAfterNodeCreateFailure();

                    outtext.format( "ERROR: Failed to create Node: %s", nodeError.toText( smallBuf ) );
                    context.writeFrame( outtext );
                    return Command::eERROR_FAILED;
                }

                outtext.format( "Download Successful (length=%lu)", endPosition - startPosition + 1 );
                io &= context.writeFrame( outtext );
                return io ? Command::eSUCCESS : Command::eERROR_IO;
            }

            // Check for Ctrl-Q exit
            if ( strchr( (const char*) dstBufPtr, 0x11 ) != nullptr )
            {
                context.writeFrame( "Download ABORTED" );
                return Command::eERROR_FAILED;
            }

            // Setup to read the next block of stuffs
            remainingBytes -= bytesRead;
            if ( remainingBytes <= 0 )
            {
                context.writeFrame( "Download Buffer space exceeded - download ABORTED" );
                return Command::eERROR_FAILED;
            }
            dstBufPtr += bytesRead;
        }
    }

    // If I get here the command failed!
    return Command::eERROR_FAILED;
}