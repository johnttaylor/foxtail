#ifndef Fxt_Point_CreateFuncDatabase_h_
#define Fxt_Point_CreateFuncDatabase_h_
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


#include "Fxt/Point/CreateFuncDatabaseApi.h"

///
namespace Fxt {
///
namespace Point {


/** This concrete template class implements a simple Point Create Function 
    Database.

    Template Args:
        N   - The maximum number of Create Functions that can be stored in the Database
  */
template<int N>
class CreateFuncDatabase : public CreateFuncDatabaseApi
{
protected:
    /// Binds a GUID to a Create function
    struct FuncEntry_T
    {
        const char*              guid;   //!< The Point's type guid
        Descriptor::CreateFunc_T func;   //!< The Point's create function
    };

public:
    /// Constructor.  Use this constructor when creating the instance AFTER main() executes
    CreateFuncDatabase() noexcept
        :m_numFuncs( 0 )
    {
        memset( m_functions, 0, sizeof( m_functions ) );
    }



    /// Constructor.  Use this constructor when creating a static instance, i.e. BEFORE main() executes
    CreateFuncDatabase( const char* dummyArgUsedToCreateStaticConstructorSignature ) noexcept
    {
    }



public:
    /// See Fxt::Point::CreateFuncDatabaseApi
    Descriptor::CreateFunc_T lookup( const char* guidPointTypeId ) noexcept
    {
        for ( uint16_t i=0; i < N; i++ )
        {
            if ( m_functions[i].guid != nullptr && strcmp( guidPointTypeId, m_functions[i].guid ) == 0 )
            {
                return m_functions[i].func;
            }
        }

        return nullptr;
    }

    /// See Fxt::Point::CreateFuncDatabaseApi
    void insert_( Descriptor::CreateFunc_T createFunctionToAdd, const char* guidPointTypeId ) noexcept
    {
        if ( m_numFuncs < N )
        {
            m_functions[m_numFuncs].guid = guidPointTypeId;
            m_functions[m_numFuncs].func = createFunctionToAdd;
            m_numFuncs++;
        }
    }

protected:
    /// Memory for Function list
    FuncEntry_T    m_functions[N];

    /// Number of functions in the function list
    uint16_t       m_numFuncs;
};



};      // end namespaces
};
#endif  // end header latch
