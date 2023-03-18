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


#include "Scanner.h"
#include "Error.h"
#include "Cpl/System/Assert.h"
#include <new>


///
using namespace Fxt::Chassis;

//////////////////////////////////////////////////
Scanner::Scanner( Cpl::Memory::ContiguousAllocator&   generalAllocator,
                  uint16_t                            numCards,
                  size_t                              scanRateMultipler )
    : m_inputPeriod( *this )
    , m_outputPeriod( *this )
    , m_cards( nullptr )
    , m_error( Fxt::Type::Error::SUCCESS() )
    , m_srm( scanRateMultipler )
    , m_numCards( numCards )
    , m_chassisMboxPtr( nullptr )
    , m_nextCardIdx( 0 )
    , m_started( false )
{
    // Allocate my array of Card pointers
    m_cards = (Fxt::Card::Api**) generalAllocator.allocate( sizeof( Fxt::Card::Api* ) * numCards );
    if ( m_cards == nullptr )
    {
        m_numCards = 0;
        m_error    = fullErr( Err_T::NO_MEMORY_CARD_LIST );
        m_error.logIt();
    }
    else
    {
        // Zero the array so we can tell if there are missing cards
        memset( m_cards, 0, sizeof( Fxt::Card::Api* ) * numCards );
    }
}

Scanner::~Scanner()
{
    // Ensure stop is called first
    if ( m_chassisMboxPtr )
    {
        stop( *m_chassisMboxPtr );
    }

    // Call the destructors on all of the cards
    for ( uint16_t i=0; i < m_numCards; i++ )
    {
        if ( m_cards[i] )
        {
            m_cards[i]->~Api();
        }
    }
}

//////////////////////////////////////////////////
bool Scanner::start( Cpl::Itc::PostApi& chassisMbox, uint64_t currentElapsedTimeUsec ) noexcept
{
    // Do nothing if already started
    if ( !m_started && m_error == Fxt::Type::Error::SUCCESS() )
    {
        // Start the individual IO Cards (and additional error checking)
        for ( uint16_t i=0; i < m_numCards; i++ )
        {
            // Check that all cards got created
            if ( m_cards[i] == nullptr )
            {
                m_error = fullErr( Err_T::MISSING_CARDS );
                m_error.logIt();
                return false;
            }

            if ( m_cards[i]->start( chassisMbox, currentElapsedTimeUsec ) == false )
            {
                m_error = fullErr( Err_T::CARD_FAILED_START );
                m_error.logIt();
                return false;
            }
        }

        m_chassisMboxPtr = &chassisMbox;
        m_started = true;
    }

    return true;
}

void Scanner::stop( Cpl::Itc::PostApi& chassisMbox ) noexcept
{
    // Only stop if I have been started
    if ( m_started )
    {
        // Always stop the IO cards - even if there is an internal error
        for ( uint16_t i=0; i < m_numCards; i++ )
        {
            if ( m_cards[i] )
            {
                m_cards[i]->stop( chassisMbox );
            }
        }

        m_chassisMboxPtr = nullptr;
        m_started = false;
    }
}

bool Scanner::isStarted() const noexcept
{
    return m_started;
}

Fxt::Type::Error Scanner::getErrorCode() const noexcept
{
    return m_error;
}

size_t Scanner::getScanRateMultiplier() const noexcept
{
    return m_srm;
}

Fxt::System::PeriodApi& Scanner::getInputPeriod() noexcept
{
    return m_inputPeriod;
}

/// Set Fxt::Chassis::ScannerApi
Fxt::System::PeriodApi& Scanner::getOutputPeriod() noexcept
{
    return m_outputPeriod;
}

uint16_t Scanner::getNumCards() const noexcept
{
    return m_error == Fxt::Type::Error::SUCCESS() ? m_numCards : 0;
}

Fxt::Card::Api* Scanner::getCard( uint16_t cardIndex ) noexcept
{
    if ( cardIndex >= m_numCards || m_error != Fxt::Type::Error::SUCCESS() )
    {
        return nullptr;
    }
    return m_cards[cardIndex];
}

bool Scanner::scanInputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Only execute if there is no error AND the Scanner was actually started
    if ( m_error == Fxt::Type::Error::SUCCESS() && m_started )
    {
        // Scan all of the IO Cards
        for ( uint16_t i=0; i < m_numCards; i++ )
        {
            if ( m_cards[i]->scanInputs( currentElapsedTimeUsec ) == false )
            {
                m_error = fullErr( Err_T::CARD_SCAN_FAILURE );
                m_error.logIt();
                return false;
            }
        }
    }

    return true;
}

bool Scanner::flushOutputs( uint64_t currentElapsedTimeUsec ) noexcept
{
    // Only execute if there is no error AND the Scanner was actually started
    if ( m_error == Fxt::Type::Error::SUCCESS() && m_started )
    {
        // Flush all of the IO Cards
        for ( uint16_t i=0; i < m_numCards; i++ )
        {
            if ( m_cards[i]->flushOutputs( currentElapsedTimeUsec ) == false )
            {
                m_error = fullErr( Err_T::CARD_FLUSH_FAILURE );
                m_error.logIt();
                return false;
            }
        }
    }

    return true;
}


//////////////////////////////////////////////////
Fxt::Type::Error Scanner::add( Fxt::Card::Api& cardToAdd ) noexcept
{
    if ( m_error == Fxt::Type::Error::SUCCESS() )
    {
        if ( m_nextCardIdx >= m_numCards )
        {
            m_error = fullErr( Err_T::TOO_MANY_CARDS );
            m_error.logIt();
        }
        else
        {
            m_cards[m_nextCardIdx] = &cardToAdd;
            m_nextCardIdx++;
        }
    }

    return m_error;
}


//////////////////////////////////////////////////
ScannerApi* ScannerApi::createScannerfromJSON( JsonVariant                         scannerJsonObject,
                                               Cpl::Memory::ContiguousAllocator&   generalAllocator,
                                               Cpl::Memory::ContiguousAllocator&   cardStatefulDataAllocator,
                                               Cpl::Memory::ContiguousAllocator&   haStatefulDataAllocator,
                                               Fxt::Card::FactoryDatabaseApi&      cardFactoryDb,
                                               Fxt::Point::FactoryDatabaseApi&     pointFactoryDb,
                                               Fxt::Point::DatabaseApi&            dbForPoints,
                                               Fxt::Type::Error&                   scannerErrorode ) noexcept
{
    // Minimal syntax checking of the JSON input
    if ( scannerJsonObject["cards"].is<JsonArray>() == false )
    {
        scannerErrorode = fullErr( Err_T::PARSE_CARDS_ARRAY );
        scannerErrorode.logIt();
        return nullptr;
    }

    JsonArray cards    = scannerJsonObject["cards"];
    size_t    numCards = cards.size();
    if ( numCards == 0 )
    {
        scannerErrorode = fullErr( Err_T::NO_CARDS );
        scannerErrorode.logIt();
        return nullptr;
    }

    // Parse Scan Rate Multiplier
    size_t srm = scannerJsonObject["scanRateMultiplier"] | ((size_t) (-1));
    if ( srm == ((size_t) (-1)) )
    {
        scannerErrorode = fullErr( Err_T::SCANNER_MISSING_SRM );
        scannerErrorode.logIt();
        return nullptr;
    }

    // Create Scanner instance
    void* memScanner = generalAllocator.allocate( sizeof( Scanner ) );
    if ( memScanner == nullptr )
    {
        scannerErrorode = fullErr( Err_T::NO_MEMORY_SCANNER );
        scannerErrorode.logIt();
        return nullptr;
    }
    ScannerApi* scanner = new(memScanner) Scanner( generalAllocator, (uint16_t) numCards, srm );

    // Create IO Cards
    for ( uint16_t i=0; i < numCards; i++ )
    {
        Fxt::Type::Error    errorCode = Fxt::Type::Error::SUCCESS();
        JsonObject          cardJson  = cards[i];
        Fxt::Card::Api*     card      = cardFactoryDb.createCardfromJSON( cardJson,
                                                                          generalAllocator,
                                                                          cardStatefulDataAllocator,
                                                                          haStatefulDataAllocator,
                                                                          pointFactoryDb,
                                                                          dbForPoints,
                                                                          errorCode );
        if ( card == nullptr )
        {
            scannerErrorode = fullErr( Err_T::FAILED_CREATE_CARD );
            scannerErrorode.logIt();
            scanner->~ScannerApi();
            return nullptr;
        }
        if ( errorCode != Fxt::Type::Error::SUCCESS() )
        {
            scannerErrorode = fullErr( Err_T::CARD_CREATE_ERROR );
            scannerErrorode.logIt();
            scanner->~ScannerApi();
            return nullptr;
        }
        scannerErrorode = scanner->add( *card );
        if ( scannerErrorode != Fxt::Type::Error::SUCCESS() )
        {
            scanner->~ScannerApi();
            return nullptr;
        }
    }

    // If I get here -->everything worked
    return scanner;
}

