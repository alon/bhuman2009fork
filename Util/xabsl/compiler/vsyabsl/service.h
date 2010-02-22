
/***************************************************************************
         Copyright (c) Microsoft Corporation, All rights reserved.             
    This code sample is provided "AS IS" without warranty of any kind, 
    it is not recommended for use in a production environment.
***************************************************************************/

#ifndef service_h
#define service_h

#include "common.h"
#include "languagedef.h"
#include "stdservice.h"

#pragma warning (disable : 4244 )

class Service : public CommentService
{
protected:
  override const TokenInfo*     getTokenInfo()     const;
};


#endif