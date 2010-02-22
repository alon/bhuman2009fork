
/***************************************************************************
Copyright (c) Microsoft Corporation, All rights reserved.             
This code sample is provided "AS IS" without warranty of any kind, 
it is not recommended for use in a production environment.
***************************************************************************/

#include "common.h"
#include "languagedef.h"
#include "stdservice.h"
#include "dllmain.h"
#include <tchar.h>


#define CHECK(r)       ((r) == ReasonCheck)
#define SELECT(r)      ((r) >= ReasonMemberSelect && (r) <= ReasonMethodTip)
#define METHODTIP(r)   ((r) == ReasonMethodTip || (r) == ReasonQuickInfo)
#define MATCHBRACES(r) ((r) == ReasonHighlightBraces || (r) == ReasonMatchBraces)
#define AUTOS(r)       ((r) == ReasonAutos)
#define CODESPAN(r)    ((r) == ReasonCodeSpan)
#define DOUBLE_BYTE_BUF_SIZE 10 

//---------------------------------------------------------
//  
//---------------------------------------------------------
int yywrap()
{
    return 1;
}

void yyerror( const char* message )
{
    return;
}


global Service*    g_service    = NULL;


/*---------------------------------------------------------
BSTR
---------------------------------------------------------*/
BSTR BstrNew( const char* s )
{
    static OLECHAR buffer[MaxStr+1];
    if (s == NULL) return NULL;

    size_t convertedSize;
    size_t inputSize = strlen(s);
    if (0 != mbstowcs_s(&convertedSize, buffer, MaxStr+1, s, inputSize))
        return NULL;

    return SysAllocString( buffer );
}

BSTR BstrNewFormat( const char* s )
{
    if (s == NULL) return SysAllocString( L"" );
    if (s[0] == 0) return SysAllocString( L"" );
    return BstrNew(s);
}

/*---------------------------------------------------------
ColorizerState
---------------------------------------------------------*/
LexBuffer::LexBuffer( int size ) : cstate(0)
{
    yy_buffer = yy_create_buffer( NULL, size );
    prevBuffer= NULL;
    input     = NULL;
    inputStart= NULL;
    inputLim  = NULL;

    sink      = NULL;
    reason    = ReasonColorize;
    token     = NULL;
    location.initialize();

    yylloc.initialize();
    yylval.initialize();
    service   = NULL;
}

LexBuffer::~LexBuffer()
{
    yy_delete_buffer( yy_buffer );
    RELEASE(sink);
}

//---------------------------------------------------------
//  ColorizerState
//---------------------------------------------------------
const Bits MaxBitsValid   = sizeof(int) * 8;

ColorizerState::ColorizerState()
{
    m_bitsValid = 0;
    m_state     = 0;
}

ColorizerState::ColorizerState( in State state )
{
    reset(state);
}

void ColorizerState::reset( in State state )
{
    m_bitsValid = MaxBitsValid;
    m_state     = state;
}

State ColorizerState::getState()
{
    return m_state;
}

State ColorizerState::load( in Bits bits )
{
    ASSERT( bits <= m_bitsValid );

    State mask    = (1 << bits) - 1;
    State result  = m_state & mask;
    m_state     >>= bits;
    m_bitsValid  -= bits;
    return result;
}

void ColorizerState::save( in State state, in Bits bits )
{
    ASSERT( bits + m_bitsValid <= MaxBitsValid );

    State mask    = (1 << bits) - 1;
    ASSERT( state <= mask );

    m_state     <<= bits;
    m_state      |= state;
    m_bitsValid  += bits;
};


//---------------------------------------------------------
//  Location
//---------------------------------------------------------
local inline void updatePosition( in char c, 
                                 inout int& row,
                                 inout int& idx   )
{
    if (c == '\n')
    {
        idx = 0;
        row += 1;
    }
    else //if (c != '\r')
    {
        idx += 1;
    }
}

Location::Location()
{
    initialize();
}

Location::Location( in const Location& loc1, const Location& loc2 )
{
    startRow = loc1.startRow;
    startIdx = loc1.startIdx;
    endRow   = loc2.endRow;
    endIdx   = loc2.endIdx;
    text     = loc1.text;
    textOfs  = loc1.textOfs;
    textLen  = (loc2.textOfs - loc1.textOfs) + loc2.textLen;
    token    = 0;
}

final void Location::initialize(void)
{
    startRow = endRow = 0;
    startIdx = endIdx = 0;
    text     = NULL;
    textLen  = 0;
    textOfs  = 0;
    token    = 0;
}

final void Location::update( in int _token,
                            in const char* _text, 
                            in int _textLen )
{

    textOfs += textLen;
    startRow = endRow;
    startIdx = endIdx;

    token    = _token;
    text     = _text;
    textLen  = _textLen;

    for (int i = 0; i < textLen; i++)
    {
        updatePosition( text[i], endRow, endIdx );
    };
}

//---------------------------------------------------------
//  IBabelService: Init/Done
//---------------------------------------------------------
STDMETHODIMP  StdService::Init( in LCID lcid, in long reserved )
{
    return S_OK;
}

STDMETHODIMP  StdService::Done()
{
    return S_OK;
}

int endsWith(const char* s1, const char* s2)
{
    int l1 = strlen(s1);
    int l2 = strlen(s2);

    if(l2 == 0)
        return 1;

    if(l1 < l2)
        return 0;

    return strncmp(s1 + l1 - l2 , s2, l2) == 0;
}


//---------------------------------------------------------
//  IBabelService: ParseSource
//---------------------------------------------------------
STDMETHODIMP  StdService::ParseSource( in BSTR source,
                                      in IParseSink* sink,
                                      in ParseReason reason,
                                      in long reserved,
                                      out IScope** scope )
{
    TRACE(L"StdService::ParseSource");

    enterParser();
    {      
        switchBuffer( m_parserBuffer, 0, (source ? SysStringLen(source) : 0), source, sink, reason );
        if (scope) *scope = NULL;

       

        char filePath[1024];
        getFileName(filePath, 1024);

       /*  FILE * pFile;
        pFile = fopen ("c:/log1.txt","wt");
        if (pFile!=NULL)
        {
            fputs (filePath,pFile);
            fclose (pFile);
        }*/


        if(endsWith(filePath, "yabsl.api"))
        {
            yyparse();
        }
        else
        {
            IScope* scope = NULL;


            char temp[1024];
            char apiFilename[1024];

            DWORD dwBufLen = 1024;
            HKEY hKey;
            LONG lRet;

            RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\VisualStudio\\8.0"), 0, KEY_QUERY_VALUE, &hKey);
            lRet = RegQueryValueEx(hKey, TEXT("InstallDir"), NULL, NULL, (LPBYTE)temp, &dwBufLen);

            CharToOem((LPCTSTR)temp, (LPSTR)apiFilename);
            strcat_s(apiFilename, 1024 ,"yabsl.api");


            //loadScope does IParseSink::GetPackage and IBabelPackage::LoadScope.
            loadScope( apiFilename, &scope );

            if (scope != NULL && m_buffer->sink != NULL  && CHECK(m_buffer->reason)) 
            { 
                enterSink();
                {

                    m_buffer->sink->AddExtern( 1, 0, 1, 0, scope );

                }
                leaveSink();
                scope->Release(); 
            }




        }

        //      if (yyparse() != 0)
        //		errorMessage( SevFatal, "sorry, parsing could not recover" );

        doneBuffer();
    }
    leaveParser();

    return S_OK;
} 


//---------------------------------------------------------
//  IBabelService: ColorLine
//---------------------------------------------------------
STDMETHODIMP  StdService::ColorLine( in BSTR line, 
                                    in IColorSink* sink,
                                    inout long* state )
{
    TRACE(L"StdService::ColorLine");
    if (!state) return E_POINTER;

    enterLexer();
    {            
        //if (g_service) { TRACE(L"entering colorizer while parsing"); }
        switchBuffer( m_lexerBuffer, *state, (line ? SysStringLen(line) : 0), line, NULL, ReasonColorize );

        if (line) 
        {
            int   pos = 0;
            Token token;

            while ( (token = nextToken()) != 0)
            {
                if (sink)
                {  
                    const TokenInfo* tokenInfo  = lookupTokenInfo(token);

                    sink->Colorize( pos, pos + yyleng - 1
                        , tokenInfo->colorClass 
                        , tokenInfo->charClass 
                        , tokenInfo->trigger );
                    pos += yyleng;
                }
            }
        }

        *state = doneBuffer();

        //if (g_service) { TRACE(L"leaving colorizer while parsing"); }      
    }
    leaveLexer();

    return S_OK;
};

//---------------------------------------------------------
//  NextToken 
//---------------------------------------------------------
bool    StdService::isParserToken( in const TokenInfo& info) const
{
    return (  info.charClass != CharWhiteSpace
        && info.charClass != CharLineComment
        && info.charClass != CharComment );
}

Token   StdService::parserNextToken(void)
{
    Token            token;
    const TokenInfo* info;

    enterLexer();
    {

        do { 
            token  = nextToken();
            info   = lookupTokenInfo( token );
        } 
        while ( !isParserToken(*info) );
        yylloc  = m_buffer->location;
        yylval  = m_buffer->location;
    }
    leaveLexer();

    return token;
};


Token   StdService::nextToken(void)
{
    m_buffer->token = yylex();
    m_buffer->location.update( m_buffer->token, yytext, yyleng );
    return m_buffer->token;
};


//---------------------------------------------------------
//  IBabelService: ColorCount && GetColorInfo
//---------------------------------------------------------

STDMETHODIMP StdService::ColorCount( out ColorClass* count)
{
    TRACE(L"StdService::ColorCount");
    if (!count)  return E_POINTER;
    else *count = 0;

    const ColorInfo* info    = getColorInfo();
    ColorClass       counter = 0;

    ASSERT(info);
    if (info)
    {
        while (info->colorClass != ColorClassEnd) { info++; counter++; }
    };

    *count = counter;
    return S_OK;
};


STDMETHODIMP  StdService::GetColorInfo( in ColorClass index, 
                                       out BSTR* description,
                                       out BSTR* style )
{
    TRACE(L"StdService::GetColorInfo");
    OLECHAR buffer[MaxStr+1];

    const ColorInfo* info = lookupColorInfo( index );
    if (info->colorClass != index) return E_INVALIDARG;

    HRESULT hr = S_OK;
    if (description)
    {
        if (info->description == NULL)
        {
            hr = E_FAIL;
            goto Error;
        }
        size_t convertedSize;
        size_t descriptionLen = strlen(info->description);
        if (0 != mbstowcs_s(&convertedSize, buffer, MaxStr+1, info->description, descriptionLen))
        {
            hr = E_FAIL;
            goto Error;
        }
        *description = SysAllocString( buffer );
        if ((*description) == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto Error;
        }
    }

    if (style)
    {
        if (info->style == NULL)
        {
            hr = E_FAIL;
            goto Error;
        }
        size_t convertedSize;
        size_t styleSize = strlen(info->style);
        if (0 != mbstowcs_s(&convertedSize, buffer, MaxStr+1, info->style, styleSize))
        {
            hr = E_FAIL;
            goto Error;
        }
        *style = SysAllocString( buffer );
        if ((*style) == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto Error;
        }
    }
Error:
    if (FAILED(hr))
    {
        if (description)
            bstrFree(*description);
        if (style)
            bstrFree(*style);
    }
    return hr;
};


//---------------------------------------------------------
//  Token info
//---------------------------------------------------------
static TokenInfo defaultTokenInfo = { TokenEnd, ClassDefault, "<unknown>", CharDefault };

final const char* StdService::getTokenDescription( in Token token ) const
{
    static char buffer[MaxErrorMessage];

    const TokenInfo* info = lookupTokenInfo( token );
    ASSERT(info);

    if (info && info->token == token) //found a fit description
    {
        const char* text  = (m_buffer->location.text == NULL) ? "<unknown>" : m_buffer->location.text;
        if (info->description != NULL)
        {
            if (0!=sprintf_s(buffer, MaxErrorMessage, info->description, text ))
                buffer[0] = 0;
        }
        else
        {
            buffer[0] = 0;
        }
        return buffer;
    }
    else
    {
        if (token == 0)   return "end-of-file";

        if (token >= ' ')  sprintf_s( buffer, MaxErrorMessage, "token ('%c')", token );
        else sprintf_s( buffer, MaxErrorMessage, "token (0x%x)", token );
        buffer[MaxErrorMessage-1] = 0;
        return buffer;
    }
};


final const TokenInfo* StdService::lookupTokenInfo( Token token ) const
{
    const TokenInfo* info  = getTokenInfo();
    ASSERT(info);

    if (info==NULL) info   = &defaultTokenInfo;
    while (info->token != TokenEnd)       //walk through info in a linear fasion
    {
        if (info->token == token) break;
        info++;
    }

    return info;
}


const TokenInfo* StdService::getTokenInfo(void) const
{
    return &defaultTokenInfo;
}

//---------------------------------------------------------
//  Color info
//---------------------------------------------------------
local const ColorClass ColorMask = 0xFFFF;

final const ColorInfo* StdService::lookupColorInfo( in ColorClass colorClass ) const
{
    static ColorInfo defaultColorInfo = { ColorClassEnd, "Text", "" };
    const ColorInfo* info  = getColorInfo();

    ASSERT(info);
    if (info == NULL) info = &defaultColorInfo;

    colorClass &= ColorMask;    //mask off attributes (ie. ClassHumanText)
    while (info->colorClass != ColorClassEnd )
    {
        if (info->colorClass == colorClass) break;
        info++;
    }

    ASSERT( info && info->colorClass != ColorClassEnd );
    return info;
}

const ColorInfo* StdService::getColorInfo() const
{
    static ColorInfo defaultColorInfoTable[] =
    {
        //       { ClassText,        "Text",     "" },
        { ClassKeyword,     "Keyword",    "color: blue"  },
        { ClassComment,     "Comment",    "color: darkgreen; text-kind: humantext" },
        { ClassIdentifier,  "Identifier", "" },
        { ClassString,      "String",     "color: purple" },
        { ClassNumber,      "Number",     "" },
        { ColorClassEnd,    "Text",       "" }
    };

    return defaultColorInfoTable;
}

//---------------------------------------------------------
//  Error messages
//---------------------------------------------------------

static char errorBuffer[MaxErrorMessage];


void StdService::fatalFlexError( in const char* message ) const
{
    ASSERT(0);
    lexicalError( SevFatal, message );
}

void StdService::lexicalError( in Severity sev, 
                              in const char* message ) const
{
    if (!CHECK(m_buffer->reason)) return;
    if (!m_buffer->sink)          return;

    Location location = m_buffer->location;
    location.update( m_buffer->token, yytext, yyleng );
    errorMessage( sev, message, &location );
};

void StdService::expectError( in const char* construct, 
                             in const char* expecting,
                             in const Location* loc ) const
{
    if (!CHECK(m_buffer->reason)) return;
    sprintf_s( errorBuffer, MaxErrorMessage, 
        "Syntax error in %s; expecting \"%s\"", construct, expecting );
    errorBuffer[MaxErrorMessage-1] = 0;
    errorMessage( SevError, errorBuffer, loc );
}

void StdService::syntaxError( in const char* construct,
                             in const Location* loc   ) const
{
    if (!CHECK(m_buffer->reason)) return;
    const char* description = getTokenDescription( (yychar < 0 ? m_buffer->token : yychar ) );

    sprintf_s( errorBuffer, MaxErrorMessage, 
        "Syntax error in %s  (unexpected %s)", construct, description );
    errorBuffer[MaxErrorMessage-1] = 0;
    errorMessage( SevError, errorBuffer, loc );
}

void StdService::errorMessage( in Severity sev, 
                              in const char* message,
                              in const Location* loc  ) const
{
    if (!CHECK(m_buffer->reason)) return;
    if (!m_buffer->sink)          return;

    if (loc == NULL) loc = &yylloc;

    BSTR bstrMessage = BstrNew( message );
    if (bstrMessage)
    {
        enterSink();
        {
            m_buffer->sink->ErrorMessage( loc->startRow, loc->endRow,
                loc->startIdx, loc->endIdx,
                sev, bstrMessage );
            SysFreeString(bstrMessage);
        }
        leaveSink();
    }
}

//---------------------------------------------------------
//  IBabelService::GetMethodFormat
//---------------------------------------------------------
STDMETHODIMP StdService::GetMethodFormat(out BSTR* parStart, out BSTR* parSep, out BSTR* parEnd,
                                         out BSTR* typeStart, out BSTR* typeEnd,
                                         out VARIANT_BOOL* typePrefixed )
{
    const MethodFormat* methodFormat = getMethodFormat();
    if (methodFormat)
    {
        if (parStart)     *parStart = BstrNewFormat(methodFormat->parStart);
        if (parSep)       *parSep   = BstrNewFormat(methodFormat->parSep);
        if (parEnd)       *parEnd   = BstrNewFormat(methodFormat->parEnd);
        if (typeStart)    *typeStart= BstrNewFormat(methodFormat->typeStart);
        if (typeEnd)      *typeEnd  = BstrNewFormat(methodFormat->typeEnd);
        if (typePrefixed) *typePrefixed = methodFormat->typePrefixed;
        return S_OK;
    }
    else
        return E_NOTIMPL;
}

const MethodFormat* StdService::getMethodFormat() const
{
    return NULL;
}

//---------------------------------------------------------
//  IBabelService::GetImageList
//---------------------------------------------------------
STDMETHODIMP StdService::GetImageList( out long* imageListHandle, out long* glyphCount )
{
    OUTARG(imageListHandle);
    OUTARG(glyphCount);

    return E_NOTIMPL;
}


//---------------------------------------------------------
//  IBabelService::GetCommentFormat
//---------------------------------------------------------
STDMETHODIMP StdService::GetCommentFormat( out BSTR* lineStart, 
                                          out BSTR* blockStart, out BSTR* blockEnd,
                                          out VARIANT_BOOL* useLineComments )
{
    if (!lineStart) return E_INVALIDARG;
    if (!blockStart) return E_INVALIDARG;
    if (!blockEnd)  return E_INVALIDARG;
    if (!useLineComments) return E_INVALIDARG;

    const CommentFormat* commentFormat = getCommentFormat();
    if (commentFormat)
    {
        *lineStart  = BstrNewFormat(commentFormat->lineStart);
        *blockStart = BstrNewFormat(commentFormat->blockStart);
        *blockEnd   = BstrNewFormat(commentFormat->blockEnd);
        *useLineComments = commentFormat->useLineComments;
        return S_OK;
    }
    else
        return E_NOTIMPL;
}

const CommentFormat* StdService::getCommentFormat() const
{
    return NULL;
}

//---------------------------------------------------------
//  Parenthesis
//---------------------------------------------------------
void StdService::matchPair( in const Location& loc1, in const Location& loc2 ) const
{
    if (!m_buffer->sink) return;
    if (!MATCHBRACES(m_buffer->reason)) return;

    enterSink();
    {
        m_buffer->sink->MatchPair( loc1.startRow, loc1.startIdx, loc1.endRow, loc1.endIdx
            , loc2.startRow, loc2.startIdx, loc2.endRow, loc2.endIdx );
    }
    leaveSink();
}


void StdService::matchTriple( in const Location& loc1, in const Location& loc2, in const Location& loc3 ) const
{
    if (!m_buffer->sink) return;
    if (!MATCHBRACES(m_buffer->reason)) return;

    enterSink();
    {
        m_buffer->sink->MatchTriple( loc1.startRow, loc1.startIdx, loc1.endRow, loc1.endIdx
            , loc2.startRow, loc2.startIdx, loc2.endRow, loc2.endIdx
            , loc3.startRow, loc3.startIdx, loc3.endRow, loc3.endIdx);
    }
    leaveSink();
}

//---------------------------------------------------------
//  Scope functions
//---------------------------------------------------------
final void StdService::addScope( in const Location& start, in const Location& end,
                                in ScopeKind kind, in ScopeAccess access, in ScopeStorage storage,
                                in const Location& name, 
                                in const Location& descStart, in const Location& descEnd,
                                in const Location* type,             
                                in long glyph,
                                in bool merge,
                                in bool makeDescription /* = false */
                                )
{
    if (!m_buffer->sink) return;
    if (!CHECK(m_buffer->reason)) return;


    BSTR bstrName = tokenBstr(&name);
    BSTR bstrType = type ? tokenBstr(type) : NULL;

    BSTR bstrDescription = NULL;
    if (makeDescription && bstrType)
    {
        const size_t bufferSize = MaxStr+5;
        OLECHAR buffer[bufferSize];
        buffer[0] = 0;
        int iTypeLen = ::SysStringLen(bstrType);
        int iNameLen = ::SysStringLen(bstrName);
        if ( iTypeLen + iNameLen < MaxStr)
        {
            if (iTypeLen > 0)
            {
                if (0==wcscpy_s(buffer, bufferSize, bstrType))
                    wcscat_s(buffer, bufferSize, L" ");
            }
            if (iNameLen > 0)
                wcscat_s(buffer, bufferSize, bstrName);
            bstrDescription = ::SysAllocString(buffer);
        }
    }
    else
    {
        bstrDescription = tokenBstr(&descStart,&descEnd);
    }
    enterSink();
    {
        m_buffer->sink->AddScope( start.startRow, start.startIdx, end.endRow, end.endIdx,
            kind, access, storage, glyph,
            bstrName, bstrType,
            bstrName, bstrDescription, merge );
    }
    leaveSink();
    // free the strings
    bstrFree(bstrName);
    bstrFree(bstrType);
    bstrFree(bstrDescription);

}

final void StdService::addScopeText( in const Location& start, in const Location& end,
                                    in ScopeKind kind, in ScopeAccess access, in ScopeStorage storage,
                                    in const char* name, 
                                    in const char* description ,
                                    in const char* type    ,
                                    in const char* display , 
                                    in long glyph,
                                    in bool merge )
{
    if (!m_buffer->sink) return;
    if (!CHECK(m_buffer->reason)) return;


    BSTR bstrName=BstrNew(name);
    BSTR bstrType=BstrNew(type);
    BSTR bstrDescription=BstrNew(description);
    BSTR bstrDisplay=BstrNew(display);
    enterSink();
    {
        m_buffer->sink->AddScope( start.startRow, start.startIdx, end.endRow, end.endIdx,
            kind, access, storage, glyph,
            bstrName, bstrType, bstrDisplay, bstrDescription, 
            merge );
    }
    leaveSink();
    // free the strings
    bstrFree(bstrName);
    bstrFree(bstrType);
    bstrFree(bstrDescription);
    bstrFree(bstrDisplay);


}

//---------------------------------------------------------
//  Scope functions
//---------------------------------------------------------
Location StdService::range( in const Location& loc1, in const Location& loc2 ) const
{
    Location result(loc1,loc2);
    return result;
}

const char* StdService::tokenText( in const Location* tokenStart
                                  , in const Location* tokenEnd ) const
{
    if (tokenStart == NULL) tokenStart = &m_buffer->location;
    if (tokenEnd   == NULL) tokenEnd   = tokenStart;
    {
        static char buffer[MaxStr+1];
        Location token( *tokenStart, *tokenEnd );
        int len = MaxStr > token.textLen ? token.textLen : MaxStr;

        size_t convertedSize;
        if (0!=wcstombs_s(&convertedSize, buffer, MaxStr+1, m_buffer->inputStart + token.textOfs, len))
            buffer[0] = 0;
        return buffer;
    }
}

BSTR StdService::tokenBstr( in const Location* tokenStart
                           , in const Location* tokenEnd ) const
{
    if (tokenStart == NULL) tokenStart = &m_buffer->location;
    if (tokenEnd   == NULL) tokenEnd   = tokenStart;

    Location token( *tokenStart, *tokenEnd );
    int len = token.textLen;
    return SysAllocStringLen( m_buffer->inputStart + token.textOfs, token.textLen );
}


final void StdService::addExtern( in const Location& start, in const Location& end,
                                 in IScope* scope )
{
    if (!m_buffer->sink) return;
    if (!CHECK(m_buffer->reason)) return;

    enterSink();
    {
        m_buffer->sink->AddExtern( start.startRow, start.startIdx, end.endRow, end.endIdx,
            scope );
    }
    leaveSink();
}

//---------------------------------------------------------
//  Context
//---------------------------------------------------------
void StdService::getProject( out IBabelProject** project )
{
    if (project) *project = NULL;

    if (!m_buffer->sink) return;
    if (!CHECK(m_buffer->reason)) return;
    if (!project) return;

    HRESULT hr;
    enterSink();
    {
        hr = m_buffer->sink->GetProject( project );
    }
    leaveSink();

    if (FAILED(hr)) { *project = NULL; }
    return;
}

void StdService::getPackage( out IBabelPackage** package )
{
    if (package) *package = NULL;

    if (!m_buffer->sink) return;
    if (!CHECK(m_buffer->reason)) return;
    if (!package) {
        return;}

    HRESULT hr;
    enterSink();
    {
        hr = m_buffer->sink->GetPackage( package );
    }
    leaveSink();

    if (FAILED(hr)) {return;}
}

void StdService::getFileName( out char* filePath, in int nameLen )
{
    if (filePath) filePath[0] = 0;

    if (!m_buffer->sink) return;
    if (!CHECK(m_buffer->reason)) return;
    if (!filePath || nameLen <= 0) return;
    HRESULT hr;

    BSTR bstrFilePath = NULL;
    enterSink();
    {
            hr = m_buffer->sink->GetFileName( &bstrFilePath );
    }
    leaveSink();

    if (hr == S_OK && bstrFilePath)
    {
        size_t convertedSize;
        wcstombs_s(&convertedSize, filePath, nameLen, bstrFilePath, ::SysStringLen(bstrFilePath));
        SysFreeString(bstrFilePath);
     }

    return;
}


void StdService::searchFile( inout char* fileName, in int nameLen )
{
    if (!fileName || nameLen <= 0) return;
    IBabelProject* project = NULL;

    getProject( &project );
    if (!project) return;

    BSTR bstrFileName = BstrNew(fileName);
    if (!bstrFileName) { RELEASE(project); return; }

    BSTR bstrFilePath = NULL;
    HRESULT hr = project->SearchFile( bstrFileName, &bstrFilePath );
    RELEASE(project);
    SysFreeString(bstrFileName);
    if (FAILED(hr)) return;

    size_t convertedSize;
    wcstombs_s(&convertedSize, fileName, nameLen, bstrFilePath, ::SysStringLen(bstrFilePath));
    SysFreeString(bstrFilePath);
    return;
}

void StdService::loadScope( in const char* fileName, out IScope** scope )
{
    if (!scope)return;
    else *scope = NULL;
    if (!fileName) {return;}

    IBabelPackage* package = NULL;
    getPackage( &package );
    if (!package) {return; }

    IBabelProject* project = NULL;
    getProject( &project );

    BSTR bstrFileName = BstrNew(fileName);
    if (!bstrFileName) { RELEASE(package); RELEASE(project); return;}

    HRESULT hr = package->LoadScope( 0, bstrFileName, project, scope );
    RELEASE(project);
    RELEASE(package);
    SysFreeString(bstrFileName);
    if (FAILED(hr)) {return;}

    return;
}

//---------------------------------------------------------
//  Member selection 
//---------------------------------------------------------
void StdService::startName( in const Location& loc ) const
{
    if (!SELECT(m_buffer->reason) && !AUTOS(m_buffer->reason)) return;
    if (!m_buffer->sink) return;
    enterSink();
    {
        m_buffer->sink->StartName( loc.endRow, loc.startIdx, loc.endIdx );
    }
    leaveSink();
}

void StdService::qualifyName( in const Location& locSelector, in const Location& loc) const
{
    if (!SELECT(m_buffer->reason) && !AUTOS(m_buffer->reason)) return;  
    if (!m_buffer->sink) return;
    enterSink();
    {
        m_buffer->sink->QualifyName( locSelector.endRow, locSelector.startIdx, locSelector.endIdx,
            loc.endRow, loc.startIdx, loc.endIdx );
    }
    leaveSink();
}


void StdService::autoExpression( in const Location& loc ) const
{
    if (!AUTOS(m_buffer->reason)) return;
    if (!m_buffer->sink) return;

    enterSink();
    {
        m_buffer->sink->AutoExpression( loc.startRow, loc.startIdx, loc.endRow, loc.endIdx );
    }
    leaveSink();
}

void StdService::codeSpan( in const Location& start, in const Location& end ) const
{
    if (!CODESPAN(m_buffer->reason)) return;
    if (!m_buffer->sink) return;

    enterSink();
    {
        m_buffer->sink->CodeSpan( start.startRow, start.startIdx, end.endRow, end.endIdx );
    }
    leaveSink();
}

//---------------------------------------------------------
//  Method info
//---------------------------------------------------------
void StdService::startParameters( in const Location& loc ) const
{
    if (!m_buffer->sink) return;
    if (!METHODTIP(m_buffer->reason)) return;
    enterSink();
    {
        m_buffer->sink->StartParameters(loc.endRow,loc.endIdx);
    }
    leaveSink();
}

void StdService::parameter(in const Location& loc) const
{
    if (!m_buffer->sink) return;
    if (!METHODTIP(m_buffer->reason)) return;
    enterSink();
    {
        m_buffer->sink->Parameter(loc.endRow,loc.endIdx);
    }
    leaveSink();
}

void StdService::endParameters(in const Location& loc) const
{
    if (!m_buffer->sink) return;
    if (!METHODTIP(m_buffer->reason)) return;
    enterSink();
    {
        m_buffer->sink->EndParameters(loc.endRow,loc.endIdx);
    }
    leaveSink();
}


//---------------------------------------------------------
//  init/done
//---------------------------------------------------------

void StdService::initColorizerState( inout ColorizerState& cstate )
{
    LexState lexstate = cstate.load( StateBits );
    setLexState( lexstate );
}

void StdService::doneColorizerState( inout ColorizerState& cstate )
{
    cstate.save( getLexState(), StateBits );
}

//---------------------------------------------------------
//  Construction & Destruction
//---------------------------------------------------------
StdService::StdService()
: m_lexerBuffer(1024), m_parserBuffer()
{
    m_refCount = 1;

    InitializeCriticalSection( &m_parserLock );
    InitializeCriticalSection( &m_lexerLock  );

    IncRefCount( &g_dllRefCount );
    g_service  = (Service*)this;
    m_buffer   = NULL;  

    ColorizerState cstate( 0 );
    initColorizerState(cstate);
};

StdService::~StdService()
{
    ASSERT(m_buffer == NULL);
    g_service = NULL;
    DeleteCriticalSection( &m_lexerLock );
    DeleteCriticalSection( &m_parserLock );
    DecRefCount( &g_dllRefCount );
}

final void StdService::enterParser()
{
    EnterCriticalSection( &m_parserLock );
}

final void StdService::leaveParser()
{
    LeaveCriticalSection( &m_parserLock );
}

final void StdService::enterLexer() 
{
    EnterCriticalSection( &m_lexerLock );
}

final void StdService::leaveLexer() 
{
    LeaveCriticalSection( &m_lexerLock );
}

//enter/leaveSink simply use the lexer lock
final void StdService::enterSink() const
{
    EnterCriticalSection( const_cast<LPCRITICAL_SECTION>(&m_lexerLock) );
}

final void StdService::leaveSink() const
{
    LeaveCriticalSection( const_cast<LPCRITICAL_SECTION>(&m_lexerLock) );
}

final void StdService::switchBuffer( 
                                    in LexBuffer&       buffer,
                                    in State            state,
                                    in int              inputLen,
                                    in const InputChar* input,
                                    in IParseSink*      sink,
                                    in ParseReason      reason )
{
    enterLexer();
    {
        ASSERT(buffer.prevBuffer == NULL);
        ASSERT(buffer.input == NULL);

        //save previous buffer
        buffer.prevBuffer = m_buffer;
        if (m_buffer)
        {
            TRACE(L"save buffer");
            //save context
            doneColorizerState( m_buffer->cstate );
            m_buffer->yylloc  = yylloc;
            m_buffer->yylval  = yylval;
            m_buffer->service = g_service;
        }

        buffer.cstate.reset( state ); 
        buffer.location.initialize();
        buffer.token      = 0;
        buffer.sink       = sink;   
        if (buffer.sink) buffer.sink->AddRef();
        buffer.reason     = reason;
        buffer.input      = input;
        buffer.inputStart = input;
        buffer.inputLim   = input + inputLen;
        yy_flush_buffer( buffer.yy_buffer );

        //set new buffer
        m_buffer = &buffer;

        //and switch
        yy_switch_to_buffer( buffer.yy_buffer );
        initColorizerState( buffer.cstate );
    }
    leaveLexer();
    return;
}

final State StdService::doneBuffer()  
{
    State state = 0;
    enterLexer();
    {
        ASSERT(m_buffer);

        doneColorizerState( m_buffer->cstate );
        state = m_buffer->cstate.getState();

        yy_flush_buffer( m_buffer->yy_buffer );
        m_buffer->inputLim  = NULL;
        m_buffer->input     = NULL;
        m_buffer->inputStart= NULL;
        RELEASE(m_buffer->sink);
        m_buffer->token     = 0;
        m_buffer->location.initialize();

        if (m_buffer->prevBuffer)
        {
            TRACE(L"restore buffer");
            LexBuffer* prev = m_buffer->prevBuffer;
            m_buffer->prevBuffer = NULL;

            //restore previous buffer
            m_buffer  = prev;

            yylloc    = m_buffer->yylloc;
            yylval    = m_buffer->yylval;
            g_service = m_buffer->service;

            yy_switch_to_buffer( m_buffer->yy_buffer );
            initColorizerState( m_buffer->cstate );
        }
        else
        {
            m_buffer = NULL;
        }
    }
    leaveLexer();
    return state;
}


/*final void StdService::internInit( in State state, 
in int inputLen,
in const InputChar* input,
in IParseSink* sink )
{
enter();
{
g_service    = (Service*)this;

m_buffer->location.initialize();
m_buffer->token      = 0;
m_buffer->input      = input;
m_buffer->inputStart = input;
m_buffer->inputLim   = input + inputLen;
m_buffer->reason     = ReasonMatchingPair;
m_buffer->sink       = sink;
if (m_buffer->sink) m_buffer->sink->AddRef();

ColorizerState cstate( state );
init( cstate );
}
leave();
};

State StdService::internDone()
{
State state;

enter();
{
ColorizerState cstate;
done( cstate );    

if (m_buffer->sink) 
{
m_buffer->sink->Release();
m_buffer->sink = NULL;
}

m_buffer->input     = NULL;
m_buffer->inputLim  = NULL;

m_buffer->token     = 0;
m_buffer->location.initialize();

g_service = NULL;
state = cstate.getState();  
}
leave();
return state;
};*/


//---------------------------------------------------------
//  Lexer state
//---------------------------------------------------------

void StdService::setLexState( in LexState lexstate )
{
    primSetLexState( lexstate );
}

LexState StdService::getLexState() const
{
    return primGetLexState();
}

//---------------------------------------------------------
//  Read input from our input buffer (called by the lexer)
//---------------------------------------------------------
void StdService::readInput( out char* buffer,
                           out int* result,
                           in  int maxSize )
{
    enterLexer();
    {
        ASSERT(m_buffer);
        ASSERT(buffer);
        ASSERT(result);

        // Check our output parameters for NULL
        if (buffer == NULL || result == NULL)   
        {
            return;    
        }

        // Check that we have characters in the input buffer and space in the
        // output buffer before the first iteration of the "do" loop.
        if (m_buffer->input >= m_buffer->inputLim || m_buffer->input == NULL || maxSize == 0)   
        {
            *result = 0;    
        }
        else 
        {               
            char* current  = buffer;

            do
            {  
                //treat "\r\n" as " \n"  (MS-DOS CR/LF)
                //treat just "\r" as "\n"  (UNIX)
                if (m_buffer->input[0] == '\r') 
                {
                    m_buffer->input++;
                    // If we have reached then end of our input characters or we are
                    // at the end of the output buffer we insert a "\n". If not we
                    // check the next character and if it is a "\n" we insert a space
                    // for the "\r". Any other character and we insert a "\n" for the "\r".
                    if ((m_buffer->input < m_buffer->inputLim) && ((current - buffer) < (maxSize - 1)))
                    {
                        // We have not reached the end of our input or output buffers
                        // so we can check if the next character is a "\n"
                        if (m_buffer->input[0] == '\n')
                            current[0] = ' ';
                        else
                            current[0] = '\n';
                    }
                    else
                    {
                        current[0] = '\n';
                    }
                    current++;
                }
                //treat "\0" (end of file) as "\n"
                else if (m_buffer->input[0] == '\0' )
                {
                    current[0] = '\n';
                    current++;
                    m_buffer->input++;
                }
                else
                {
                    // Convert our input wide character to multibyte representation.
                    // Note we need to ensure there is sufficent space in our output
                    // buffer and we also need to test for failure
                    char tempBuf[DOUBLE_BYTE_BUF_SIZE];   // Much bigger than we will ever need
                    int  count;
                    if (0== wctomb_s(&count, tempBuf, DOUBLE_BYTE_BUF_SIZE, m_buffer->input[0]) &&
                        count > 0)
                    {
                        // If the conversion is successful then copy the byte(s) to the output
                        if ((current - buffer) <= (maxSize - count)) // Check for room in the output buffer
                        {
                            memcpy(current, tempBuf, count);
                            current += count;
                        }
                        else
                        {
                            // We don't have room so we output a space
                            current[0] = ' ';
                            current++;
                        }
                    }
                    m_buffer->input++; 
                }          
            }                   
            while (m_buffer->input < m_buffer->inputLim && ((current - buffer) < maxSize)); 

            *result = (int)(current - buffer);
        }
    }
    leaveLexer();
}



/*---------------------------------------------------------
IUnknown
-----------------------------------------------------------*/
STDMETHODIMP StdService::QueryInterface( in REFIID iid, out void** obj )
{
    OUTARG(obj);

    if (iid == IID_IUnknown)
    {
        TRACE(L"StdService::QueryInterface for IUnknown");
        *obj = static_cast<IUnknown*>(this);
    }
    else if (iid == IID_IDispatch)
    {
        TRACE(L"StdService::QueryInterface for IDispatch");
        *obj = static_cast<IDispatch*>(this);
    }
    else if (iid == IID_IBabelService)
    {
        TRACE(L"StdService::QueryInterface for IBabelService");
        *obj = static_cast<IBabelService*>(this);
    }
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) StdService::AddRef()
{
    return IncRefCount(&m_refCount);
}

STDMETHODIMP_(ULONG) StdService::Release()
{
    if (DecRefCount(&m_refCount) == 0)
    {
        delete this;
        return 0;
    }
    else
        return m_refCount;
}

/*---------------------------------------------------------
implement IDispatch (for IBabelService only)
-----------------------------------------------------------*/
STDMETHODIMP StdService::GetTypeInfoCount( out UINT* count )
{
    OUTARG(count);

    if (g_typeInfoBabelService) *count = 1;
    else *count = 0;
    return S_OK;
}

STDMETHODIMP StdService::GetTypeInfo( in UINT index, in LCID lcid, out ITypeInfo** typeInfo )
{
    OUTARG(typeInfo);
    if (index != 0) return E_INVALIDARG;
    if (!g_typeInfoBabelService) return TYPE_E_CANTLOADLIBRARY;

    *typeInfo = g_typeInfoBabelService;
    return S_OK;
}

STDMETHODIMP StdService::GetIDsOfNames( in REFIID iid, in OLECHAR** names, in UINT count, 
                                       in LCID lcid, out DISPID* dispids )
{
    if (!g_typeInfoBabelService) return TYPE_E_CANTLOADLIBRARY;
    return g_typeInfoBabelService->GetIDsOfNames( names, count, dispids );
}

STDMETHODIMP StdService::Invoke( in DISPID dispid, in REFIID iid, in LCID lcid, 
                                in WORD flags, in DISPPARAMS* args, 
                                out VARIANT* result, out EXCEPINFO* error, out UINT* errorArg )
{
    if (!g_typeInfoBabelService) return TYPE_E_CANTLOADLIBRARY;  
    return g_typeInfoBabelService->Invoke( static_cast<IBabelService*>(this), 
        dispid, flags, args, result, error, errorArg );
}

