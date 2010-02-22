
/***************************************************************************
         Copyright (c) Microsoft Corporation, All rights reserved.             
    This code sample is provided "AS IS" without warranty of any kind, 
    it is not recommended for use in a production environment.
***************************************************************************/

void primSetLexState( LexState state )
{
    BEGIN(state);
}

LexState primGetLexState( void )
{
    return YY_START;
}



void primSaveLexCompleteState( inout LexCompleteState& lexState )
{
  //yy_flush_buffer( yy_current_buffer );
  lexState.yy_current_buffer = yy_current_buffer; 
  yy_switch_to_buffer( yy_create_buffer(NULL,YY_BUF_SIZE) );
  return;
}

void primRestoreLexCompleteState( inout LexCompleteState& lexState )
{
  yy_delete_buffer( yy_current_buffer );
  yy_switch_to_buffer( lexState.yy_current_buffer );
  yy_switch_to_buffer( yy_current_buffer );
  return;
}
