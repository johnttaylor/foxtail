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

#include "Pt.h"
#include "Fxt/Point/Api.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/strip.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include <string.h>

///
using namespace Fxt::Point::TShell;


///////////////////////////
Pt::Pt( Cpl::Container::Map<Cpl::TShell::Command>& commandList, Fxt::Point::DatabaseApi& modelDatabase ) noexcept
	: Cpl::TShell::Cmd::Command( commandList, FXTPOINTTSHELL_VERB_PT_ )
	, m_database( modelDatabase )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T Pt::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
	Cpl::Text::String& outtext = context.getOutputBuffer();
	const char* subCmd = Cpl::Text::stripSpace( Cpl::Text::stripNotSpace( cmdString ) );
	bool        io     = true;

	// LIST sub-command
	if ( subCmd == 0 || *subCmd == '\0' || *subCmd == 'l' )
	{
		Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );

		// Too many args?
		if ( tokens.numParameters() > 4 )
		{
			return Command::eERROR_EXTRA_ARGS;
		}

		// Get the optional filter arg
		const char* filter = tokens.numParameters() == 3 ? tokens.getParameter( 2 ) : nullptr;

		// Walk the Model database
		for ( size_t i=0; i < m_database.getMaxNumPoints() && io; i++ )
		{
			Fxt::Point::Api* point = m_database.lookupById( i );
			if ( point != nullptr )
			{
				if ( filter == nullptr || strstr( point->getName(), filter ) != 0 )
				{
					outtext.format( "%lu  %s", point->getId(), point->getName() );
					io &= context.writeFrame( outtext );
				}
			}
		}

		// If I get here -->the command succeeded
		return Command::eSUCCESS;

	}

	// READ Sub-command
	else if ( *subCmd == 'r' )
	{
		Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );

		// Wrong number of args?
		if ( tokens.numParameters() < 2 )
		{
			return Command::eERROR_MISSING_ARGS;
		}
		else if ( tokens.numParameters() > 3 )
		{
			return Command::eERROR_EXTRA_ARGS;
		}

		// Parse the point ID
		unsigned long pointId;
		if ( Cpl::Text::a2ul( pointId, tokens.getParameter( 2 ) ) == false )
		{
			outtext.format( "Invalid Point ID (%s)", tokens.getParameter( 2 ) );
			context.writeFrame( outtext );
			return Command::eERROR_INVALID_ARGS;
		}

		// Generate the JSON object/string for the Point
		bool				truncated;
		int					outlen;
		Cpl::Text::String&	outtext    = context.getOutputBuffer();
		char*				outptr     = outtext.getBuffer( outlen );
		if ( m_database.toJSON( pointId, outptr, outlen, truncated, true, true ) == false )
		{
			outtext.format( "Point ID (%s) not found in the DB", tokens.getParameter( 2 ) );
			context.writeFrame( outtext );
			return Command::eERROR_INVALID_ARGS;
		}

		// Output the JSON object
		return context.writeFrame( outtext ) ? Command::eSUCCESS : Command::eERROR_IO;
	}

	// WRITE sub-command
	else if ( *subCmd == 'w' )
	{
		// Find the start of the JSON object
		const char* json   = Cpl::Text::stripSpace( Cpl::Text::stripNotSpace( subCmd ) );
		if ( *json != '{' )
		{
			return Command::eERROR_INVALID_ARGS;
		}

		// Attempt to update the Model Point
		Cpl::Text::FString<128> errorMsg;
		if ( !m_database.fromJSON( json, &errorMsg ) )
		{
			context.writeFrame( errorMsg );
			return Command::eERROR_INVALID_ARGS;
		}
		return Command::eSUCCESS;
	}

	// If I get here the command failed!
	return Command::eERROR_FAILED;
}