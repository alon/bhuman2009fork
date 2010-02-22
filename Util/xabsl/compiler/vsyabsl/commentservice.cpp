
/***************************************************************************
         Copyright (c) Microsoft Corporation, All rights reserved.             
    This code sample is provided "AS IS" without warranty of any kind, 
    it is not recommended for use in a production environment.
***************************************************************************/

#include "common.h"
#include "languagedef.h"
#include "stdservice.h"

CommentService::CommentService() : StdService()
{
    m_commentNesting    = 0;
    m_returnState       = 0;
}

//---------------------------------------------------------
//  State init/done
//---------------------------------------------------------

override void CommentService::initColorizerState( inout ColorizerState& cstate )
{
    StdService::initColorizerState( cstate );

    m_commentNesting = cstate.load( CommentBits );
    m_returnState    = cstate.load( StateBits );
};

override void CommentService::doneColorizerState( inout ColorizerState& cstate )
{
    //this only has effect during a "checkSyntax"
    if (m_commentNesting > 0)
        lexicalError( SevError, "unexpected end of comment" );

    cstate.save( m_returnState, StateBits );
    cstate.save( m_commentNesting, CommentBits );

    StdService::doneColorizerState( cstate );
};


//---------------------------------------------------------
//  Comment functionality
//---------------------------------------------------------
final void CommentService::enterComment( in LexState commentState )
{    
    if (m_commentNesting == MaxCommentNesting)
        lexicalError( SevError, "maximal comment nesting level reached" );
    else
        m_commentNesting++;

    if (commentState != 0)
    {
        ASSERT( m_returnState == 0 );
        if (m_returnState == 0)
            m_returnState = getLexState();
        setLexState( commentState );
    }
}

final void CommentService::leaveComment(void)
{
    if (m_commentNesting > 0) 
        m_commentNesting--;
    
    if (m_commentNesting == 0)
    {
        setLexState( m_returnState );
        m_returnState = 0;
    }
}

final bool CommentService::inComment(void) const
{
    return (m_commentNesting > 0);
}
