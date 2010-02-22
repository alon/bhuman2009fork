
/***************************************************************************
         Copyright (c) Microsoft Corporation, All rights reserved.             
    This code sample is provided "AS IS" without warranty of any kind, 
    it is not recommended for use in a production environment.
***************************************************************************/

#ifndef babel_stdservice_h
#define babel_stdservice_h

//---------------------------------------------------------
//  Basic type definitions
//---------------------------------------------------------
typedef wchar_t  InputChar;      

const int MaxErrorMessage = 128;
const int MaxStr          = 512;

//---------------------------------------------------------
//  Documenting defines
//---------------------------------------------------------
#define override
#define final
#define pure            = 0
#define local   static
#define global


//---------------------------------------------------------
//  Handle Colorizer state
//---------------------------------------------------------
typedef unsigned State;
typedef unsigned Bits;
typedef unsigned LexState;

const Bits  StateBits     = 4;            //max 16 lex states
const Bits  CommentBits   = 8;            //max 256 nesting levels
const State MaxCommentNesting = 255;

class ColorizerState
{
private:
  State   m_state;
  Bits    m_bitsValid;

public:
  ColorizerState();        
  ColorizerState( in State );
  void  reset( in State );
  
  State getState(void);            

  State load( in Bits );
  void  save( in State, in Bits );
};

//---------------------------------------------------------
//  Locations
//---------------------------------------------------------

struct Location
{
public:
    int startRow, endRow;   //line number
    int startIdx, endIdx;   //character index ('\t' counts as one character)
    const char* text;       //the scanned text
    int   textLen;          //length of the text
    int   textOfs;          //offset in the *original* text buffer
    int   token;            //token at the location

// "initialize" initializes the location to the start state
    Location();
    Location( in const Location&, in const Location& );
    final void initialize(void);

// "update" updates the location given some text
    final void update( in int token, in const char* text, in int textLen );
};

//bison/yacc specific defines
#ifndef YYLTYPE
 #define YYLTYPE            Location
#endif

#ifndef YYSTYPE
 #define YYSTYPE            Location
#endif

#define first_line      startRow
#define first_column    startIdx
#define last_line       endRow
#define last_column     endIdx

//---------------------------------------------------------
//  Token information
//---------------------------------------------------------
typedef int      Token;
const Token      TokenEnd        = (Token)(-1);
const ColorClass ColorClassEnd   = (ColorClass)(-1);

struct TokenInfo 
{
  Token         token;
  ColorClass    colorClass;
  const char*   description;
  CharClass     charClass;
  TriggerClass  trigger;
};


//---------------------------------------------------------
//  Color information
//---------------------------------------------------------
struct ColorInfo 
{
  ColorClass  colorClass;
  const char* description;
  const char* style;
};

//---------------------------------------------------------
//  Method format information
//---------------------------------------------------------
struct MethodFormat
{
  const char* parStart;
  const char* parSep;
  const char* parEnd;
  const char* typeStart;
  const char* typeEnd;
  bool        typePrefixed; 
};

//---------------------------------------------------------
//  Comment format information
//---------------------------------------------------------
struct CommentFormat
{
  const char* lineStart;
  const char* blockStart;
  const char* blockEnd;
  bool        useLineComments; 
};


//---------------------------------------------------------
//  Colorizer state
//---------------------------------------------------------
struct yy_buffer_state;

struct LexBuffer
{
  LexBuffer*        prevBuffer;

  const InputChar*  inputLim;
  const InputChar*  input;
  const InputChar*  inputStart;
  yy_buffer_state*  yy_buffer;

  IParseSink*       sink;         //used by error messages
  ParseReason       reason;
  
  //these variables are influenced by the lexer
  ColorizerState    cstate;
  Token             token;
  Location          location;
  
  //sometimes these globals need to be saved/restored 
  YYLTYPE           yylloc;
  YYSTYPE           yylval;
  Service*          service;

  LexBuffer( int size = 16*1024);
  ~LexBuffer();
};
  
//---------------------------------------------------------
//  Basic service implementation for colorization and syntax checking
//---------------------------------------------------------
class StdService : public IBabelService
{
private:
    ULONG             m_refCount;
    ITypeInfo*        m_typeInfo;   //IBabelService typeinfo.
    
    CRITICAL_SECTION  m_lexerLock;
    CRITICAL_SECTION  m_parserLock;
        
    
protected:
    LexBuffer*    m_buffer;
    LexBuffer     m_lexerBuffer;
    LexBuffer     m_parserBuffer;

    final void    enterParser();
    final void    leaveParser();
    final void    enterLexer();  
    final void    leaveLexer();

    final void    enterSink() const; 
    final void    leaveSink() const;
    
    final void    initBuffer( inout LexBuffer& buffer );
    final void    switchBuffer( inout LexBuffer&    buffer,
                                in State            state,
                                in int              inputLen  = 0,
                                in const InputChar* input     = NULL,
                                in IParseSink*      sink      = NULL,
                                in ParseReason      reason    = ReasonColorize );
    final State   doneBuffer();  
    

friend int yyparse(void);       //now the parser can call "parserNextToken"

    StdService();
    virtual ~StdService();
   
//scan a new token, called by parser and ColorLine.
    virtual Token nextToken(void);    
    virtual Token parserNextToken(void);    //ignores 'white' tokens    

//which tokens are passed to the parser, override to see whitespace and comments
    virtual bool  isParserToken( in const TokenInfo& tokenInfo ) const;

//color info
//  this needs to be overridden only if the language provides
//  custom color classes instead of the default ones.
    virtual const ColorInfo* getColorInfo(void) const;

    final   const ColorInfo* lookupColorInfo( in ColorClass ) const;
    
//token information
// "getTokenInfo" needs to be overridden for each new language to
// map tokens to colorclasses and to tell if a token should be
// considered white space for the parser.
    virtual const TokenInfo* getTokenInfo(void) const;

    final   const TokenInfo* lookupTokenInfo( in Token ) const;
    final   const char*      getTokenDescription( in Token ) const;


//method format
//  override this method for custom formatting
    virtual const MethodFormat* getMethodFormat(void) const;

//comment format
//  override this method for custom comments
    virtual const CommentFormat* getCommentFormat(void) const;

//override these two functions to implement your own state
    virtual void initColorizerState( inout ColorizerState& );
    virtual void doneColorizerState( inout ColorizerState& );

     
public:
    //hook for the lexer (thats why it is public)
    virtual void  readInput( out   char* buffer, 
                             out   int*  result, 
                             in    int   maxSize );

    //send an error message (only sends within "ParseSource" context)
    virtual void  errorMessage( in Severity, 
                                in const char* message,
                                in const Location* = NULL )    const;

    //convenient error functions 
    final void fatalFlexError( in const char* message )  const;

    final void lexicalError( in Severity, 
                             in const char* message )     const;
    final void syntaxError( in const char* construct,
                            in const Location* = NULL )  const;

    final void expectError( in const char* construct,
                            in const char* expecting,
                            in const Location* = NULL )  const;
    
    //parenthesis
    final void matchPair( in const Location& loc1, in const Location& loc2 ) const;
    final void matchTriple( in const Location& loc1, in const Location& loc2, in const Location& loc3 ) const;

    //scope functions
    final Location range( in const Location& loc1, in const Location& loc2 ) const;
    final const char* tokenText( in const Location* = NULL, in const Location* = NULL ) const;
    final BSTR tokenBstr( in const Location* tokenStart = NULL
                        , in const Location* tokenEnd   = NULL ) const;

    final void addScope( in const Location& start, in const Location& end,
                         in ScopeKind kind, in ScopeAccess access, in ScopeStorage storage,
                         in const Location& name, 
                         in const Location& descStart, in const Location& descEnd,
                         in const Location* type = NULL,         
                         in long glyph = -1,
                         in bool merge = false,
                         in bool makeDescription = false
                        );
    final void addScopeText( in const Location& start, in const Location& end,
                             in ScopeKind kind, in ScopeAccess access, in ScopeStorage storage,
                             in const char* name    = NULL, 
                             in const char* description = NULL,
                             in const char* type    = NULL,
                             in const char* display = NULL, 
                             in long glyph = -1,
                             in bool merge = false );
    final void addExtern( in const Location& start, in const Location& end,
                          in IScope* scope );

    //member selection
    final void startName( in const Location& name) const;
    final void qualifyName( in const Location& selector, in const Location& name) const;
    
    final void autoExpression( in const Location& expr ) const;
    final void codeSpan( in const Location& start, in const Location& end ) const;

    //method info
    final void startParameters( in const Location& ) const;
    final void parameter(in const Location& ) const;
    final void endParameters(in const Location& ) const;


    //context
    void getProject( out IBabelProject** project );
    void getPackage( out IBabelPackage** package );
    void getFileName( out char* filePath, in int nameLen );    
    
    void searchFile( inout char* fileName, in int nameLen );
    void loadScope( in const char* fileName, out IScope** scope );

    //lexer interface
    final void     setLexState( in LexState );
    final LexState getLexState(void)    const;

    // IUnknown methods 
    STDMETHODIMP QueryInterface( in REFIID iid, out void** obj);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    //implement IDispatch (for IBabelService only)
    STDMETHODIMP GetTypeInfoCount( out UINT* count );
    STDMETHODIMP GetTypeInfo     ( in UINT index, in LCID lcid, out ITypeInfo** typeInfo );
    STDMETHODIMP GetIDsOfNames   ( in REFIID iid, in OLECHAR** names, in UINT count, in LCID lcid, out DISPID* dispids );
    STDMETHODIMP Invoke          ( in DISPID dispid, in REFIID iid, in LCID lcid, in WORD flags, in DISPPARAMS* args, out VARIANT* result, out EXCEPINFO* error, out UINT* errorArg );

    //implement IBabelService
    STDMETHODIMP  Init( in LCID lcid, in long reserved );
    STDMETHODIMP  Done();

    STDMETHODIMP  ColorCount  ( out ColorClass* count );
    STDMETHODIMP  GetColorInfo( in  ColorClass index, 
                                out BSTR* description,
                                out BSTR* style ); 
    STDMETHODIMP  ColorLine   ( in BSTR line, 
                                in IColorSink* sink, 
                                inout long* state );
    STDMETHODIMP  ParseSource ( in BSTR text,
                                in IParseSink* sink,
                                in ParseReason reason,
                                in long reserved,
                                out IScope** scope );
    STDMETHODIMP GetMethodFormat(out BSTR* parStart, out BSTR* parSep, out BSTR* parEnd,
                                 out BSTR* typeStart, out BSTR* typeEnd,
                                 out VARIANT_BOOL* typePrefixed );
    STDMETHODIMP GetCommentFormat( out BSTR* lineStart, 
                                   out BSTR* blockStart, out BSTR* blockEnd,
                                   out VARIANT_BOOL* useLineComments );
    STDMETHODIMP GetImageList( out long* imageListHandle, out long* glyphCount );
};


//---------------------------------------------------------
//  Extended service that implements nested comments
//---------------------------------------------------------
class CommentService : public StdService
{
private:
    State    m_commentNesting;
    LexState m_returnState;

protected:
//implement our own state
    override void initColorizerState( inout ColorizerState& );
    override void doneColorizerState( inout ColorizerState& );

public:
    CommentService();

//commands
    void  enterComment( in LexState commentState = 0 );       
    void  leaveComment(void);

//queries
    bool  inComment(void)   const;
};


    

//---------------------------------------------------------
//  Prototypes for both the lexer and parser
//---------------------------------------------------------
extern int      yylex();
extern void     yyrestart( in FILE* );
extern "C" int yywrap();

extern char*    yytext;
extern int      yyleng;

#ifndef YY_NEVER_INTERACTIVE
#define YY_NEVER_INTERACTIVE 1
#endif

#define YY_FATAL_ERROR(msg) g_service->fatalFlexError( msg ) 

extern int      yyparse();
extern "C" void yyerror( const char* message );

extern YYSTYPE  yylval;
extern YYLTYPE  yylloc;
extern int      yychar;

#ifndef YYLSP_NEEDED
#define YYLSP_NEEDED 1              //we need 'yylloc'
#endif

//make the lexer read its input from our input buffer
#define YY_INPUT(buf,result,maxSize)  g_service->readInput( buf, &result, maxSize )



//make the parser use our lexer
#ifdef YYBISON           //only if we are inside the parser
#define yylex            g_service->parserNextToken
#endif


//multi-threading means we have to save the entire lexer global state, sigh

struct LexCompleteState
{
  yy_buffer_state*    yy_current_buffer;

  char  yy_hold_char; /* yy_hold_char holds the character lost when yytext is formed. */
  int   yy_n_chars;		/* number of characters read into yy_ch_buf */
  int   yyleng;

  /* Points to current character in buffer. */
  char *yy_c_buf_p;
  int   yy_init;		/* whether we need to initialize */
  int   yy_start;	/* start state number */

  /* Flag which is used to allow yywrap()'s to do buffer switches
   * instead of setting up a fresh yyin.  Could be done better...
   */
  int yy_did_buffer_switch_on_eof;
  FILE *yyin;
  FILE *yyout;

  char    *yytext;
  int     yy_last_accepting_state;
  char    *yy_last_accepting_cpos;

  /* The intent behind this definition is that it'll catch
   * any uses of REJECT which flex missed.
   */
  int yy_more_flag;
  int yy_more_len;

  /* sometimes needed but not yet supported */
  int yy_start_stack_ptr;
  int yy_start_stack_depth;
  int* yy_start_stack;

  int* yy_state_buf;
  int* yy_state_ptr;

  char* yy_full_match;
  int* yy_full_state;
  int yy_full_lp;

  int yy_lp;
  int yy_looking_for_trail_begin;
};

//things that just need to be defined in the lex-spec
extern void     primSetLexState( in LexState );
extern LexState primGetLexState( void );

extern void     yy_switch_to_buffer( yy_buffer_state* );
extern void     yy_flush_buffer( yy_buffer_state* );
extern void     yy_delete_buffer( yy_buffer_state* );
extern yy_buffer_state* yy_create_buffer( FILE*, int );


extern void     primSaveLexCompleteState( out LexCompleteState& );
extern void     primRestoreLexCompleteState( in LexCompleteState& );

#endif