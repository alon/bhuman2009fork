%{
#include "service.h"
#include "parser.hpp"
%}

%s init
%s comment

White0          [ \t\r\f\v]
White           {White0}|\n

CommentStart    \/\*
CommentEnd      \*\/
Hint            {White0}*[A-Z0-9_]+{White0}*:.*\n


%%

				
<init>"//"[^\n]*				{ return LEX_LINE_COMMENT; }
<init>option { return KEYWORD; }
<init>initial { return KEYWORD; }
<init>target { return KEYWORD; }
<init>state { return KEYWORD; }
<init>common { return KEYWORD; }
<init>decision { return KEYWORD; }
<init>action { return KEYWORD; }
<init>stay { return KEYWORD; }
<init>goto { return KEYWORD; }
<init>if { return KEYWORD; }
<init>else { return KEYWORD; }
<init>state_time { return KEYWORD; }
<init>option_time { return KEYWORD; }
<init>action_done { return KEYWORD; }
<init>agent { return KEYWORD; }
<init>function { return KEYWORD; }
<init>enum { return KEYWORD; }
<init>input { return KEYWORD; }
<init>output { return KEYWORD; }
<init>bool { return KEYWORD; }
<init>float { return KEYWORD; }
<init>const { return KEYWORD; }
<init>namespace { return KEYWORD; }
<init>behavior { return KEYWORD; }
<init>include { return KEYWORD; }
<init>true { return KEYWORD; }
<init>false { return KEYWORD; }


<init>;                         { return ';';    }

<init>,                         { return ',';    }

<init>\{                        { return '{';    }
<init>\}                        { return '}';    }
<init>=                         { return '=';    }

<init>\^                        { return '^';    }
<init>\+                        { return '+';    }
<init>\-                        { return '-';    }
<init>\*                        { return '*';    }
<init>\/                        { return '/';    }
<init>\!                        { return '!';    }
<init>==                        { return EQ;     }
<init>\!=                       { return NEQ;    }
<init>\>                        { return GT;     }
<init>\>=                       { return GTE;    }
<init>\<                        { return LT;     }
<init>\<=                       { return LTE;    }
<init>\&                        { return '&';    }
<init>\&\&                      { return AMPAMP; }
<init>\|                        { return '|';    }
<init>\|\|                      { return BARBAR; }


<init>[0-9]+(\.[0-9]*)?|^[1-9]\.[0-9]*E(\+|-)?[0-9]+ { return NUMBER; }
<init>[a-zA-Z_][a-zA-Z0-9_\.]*[\(].*?[\)]    { return PROCEDURE; }
<init>[a-zA-Z_][a-zA-Z0-9_\.]*    { return IDENTIFIER; }

<init>\(                        { return '(';    }
<init>\)                        { return ')';    }
<init>\.			            { return '.';    }

<init>{CommentStart}            { g_service->enterComment( comment );
                                  yymore();
                                } 
<init>{White0}+                 { return LEX_WHITE; }
<init>\n                        { return LEX_WHITE; }
<init>.                         { char buf[80];
                                  sprintf( buf, "invalid character ('%c', 0x%x)", yytext[0], yytext[0] );
                                  g_service->lexicalError( SevError, buf );
                                  yymore();
                                }



<comment>{CommentStart}         { g_service->enterComment();
                                  yymore();
                                }
<comment>{CommentEnd}           { g_service->leaveComment();
                                  return LEX_COMMENT;
                                }
<comment>.                      { yymore(); }
<comment>\n                     { return LEX_COMMENT; }

.|\n                            { g_service->setLexState(init); yyless(0); }


%%

#include "stdservice.c"
