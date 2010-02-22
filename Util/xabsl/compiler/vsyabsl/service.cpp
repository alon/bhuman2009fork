
/***************************************************************************
         Copyright (c) Microsoft Corporation, All rights reserved.             
    This code sample is provided "AS IS" without warranty of any kind, 
    it is not recommended for use in a production environment.
***************************************************************************/

#include "service.h"
#include "parser.hpp"

/*---------------------------------------------------------
  Globals
---------------------------------------------------------*/
const wchar_t*    g_languageName              = L"YABSL Behavior Language";
const wchar_t*    g_languageFileExtensions[]  = { L".yabsl", L".api", NULL };
const CLSID    g_languageCLSID             = {0xA1B0A1B0,0xCAFE,0xBABE,{0xA1,0xB0,0xA1,0xB0,0xCA,0xFE,0xBA,0xBE}};

const LanguageProperty g_languageProperties[] =
{
  { L"RequestStockColors", 1     },
  { L"ShowCompletion",     1     },

  { L"CodeSense",          1     },
  { L"CodeSenseDelay",     1500  },
  { L"MaxErrorMessages",   5     },
  { L"QuickInfo",          1     },
  { L"MatchBraces",        1     },
  { L"SortMemberList",     1     },
  { L"ShowMatchingBrace",  1     },
  { L"MatchBracesAtCaret", 1     },

  { NULL, 0 }
};

/*---------------------------------------------------------
  Create Service
---------------------------------------------------------*/
HRESULT CreateBabelService( out IBabelService** babelService )
{
  TRACE(L"CreateBabelService");
  OUTARG(babelService);

  *babelService = new Service();
  if (*babelService == NULL) return E_OUTOFMEMORY;

  return S_OK;
}



/*---------------------------------------------------------
  Tokens
---------------------------------------------------------*/
override const TokenInfo* Service::getTokenInfo() const
{
  static TokenInfo tokenInfoTable[] =
  {
    //TODO: add your own token information here
	{ KEYWORD,	   ClassKeyword,     "keyword ('%s')",	CharKeyword      },
    { IDENTIFIER,  ClassIdentifier,  "identifier '%s'", CharIdentifier	 },
	{ NUMBER,      ClassNumber,      "number ('%s')",	CharLiteral      },
    { ';',         ClassText,        "';'",				CharDelimiter    },     
  
    { LEX_WHITE,   ClassText,        "white space",		CharWhiteSpace   },
    { LEX_LINE_COMMENT, ClassComment,"comment",			CharLineComment  },
    { LEX_COMMENT, ClassComment,     "comment",			CharComment      },

    //always end with the 'TokenEnd' token.
    { TokenEnd,     ClassText,      "<unknown>" }
  };

  return tokenInfoTable;
};
