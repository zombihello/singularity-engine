    /* Options */
%option nounistd
%option noyywrap
%option 8bit
%option reentrant
%option nodefault
%option extra-type="ecsLexerStateInternal_t*"
%option verbose

%option nounput
%option noyyget_lineno  noyyset_lineno
%option noyyget_debug   noyyset_debug
%option noyyget_lval    noyyset_lval
%option noyyget_out     noyyset_out
%option noyyget_in      noyyset_in

    /* C++ Section */
%{
    #include "ecsfileparser_bison.cpp.h"
    #include "core/debug.h"
    #include "parserlib/lexerstate.h"
    #include "parserlib/lexerlistener.h"

    #define YY_FATAL_ERROR( Message )           { yyguts_t* yyg = ( struct yyguts_t* )yyscanner; yyextra->EmitError( Message ); }
    #define YY_USER_ACTION                      yyextra->StoreTokenStart(); yyextra->UpdateContext( yytext, yyleng );

    #define EMIT_TOKEN( TokeID )                yyextra->EmitToken( TokeID )
    #define EMIT_SEQUENCE( TokeID )             yyextra->EmitSequence( TokeID )
    #define EMIT_COMMENT()                      yyextra->EmitComment()
    #define EMIT_ERROR( Message )               yyextra->EmitError( Message )

    enum ecsLexerScope_t
    {
        ECS_LEXER_SCOPE_DEFAULT,
        ECS_LEXER_SCOPE_COMPONENT,
        ECS_LEXER_SCOPE_RESOURCE,
        ECS_LEXER_SCOPE_SYSTEM
    };


    struct ecsLexerStateInternal_t : public parserLexerState_t
    {
         ecsLexerStateInternal_t( const achar* pSourceCode, CParserLexerListener* pLexerListener )
            : parserLexerState_t( pSourceCode, pLexerListener )
            , lexerScope( ECS_LEXER_SCOPE_DEFAULT )
            , bracketScopeLevel( 0 )
        {}

        FORCEINLINE void SetScope( ecsLexerScope_t lexerScope )
        {
            if ( ecsLexerStateInternal_t::lexerScope != ECS_LEXER_SCOPE_DEFAULT || scopeLevel > 0 )
            {
                EmitError( "Invalid changing scope, components, systems and etc - must be outside of any scopes" );
            }
            ecsLexerStateInternal_t::lexerScope = lexerScope;
        }

        FORCEINLINE void ResetScope()
        {
            if ( scopeLevel > 0 )
            {
                EmitError( "Invalid changing scope to main one, something went wrong with scope levels" );
            }
            lexerScope = ECS_LEXER_SCOPE_DEFAULT;
        }

        FORCEINLINE bool IsScopeA( ecsLexerScope_t lexerScope ) const
        {
            return ecsLexerStateInternal_t::lexerScope == lexerScope;
        }

        uint32                      bracketScopeLevel;

    private:
        ecsLexerScope_t             lexerScope;
    };
%}

    /* C++ Top Section */
%top {
    #include <stdint.h>
    #include <string>

    #include "core/platform.h"

    #if PLATFORM_WINDOWS
        #include <io.h>         // For fix error with unknown 'isatty'
    #endif // PLATFORM_WINDOWS
}

    /* Lexer states */
%x MultiLineComment
%x StringLiteral
%x CppText

%%
    /* Rules */
    /* General rules */
    /* UTF-8 Byte Order Mark */
\xef\xbb\xbf									yyextra->currentContext.charOffset = 0;

    /* Keywords */
"module"                                        EMIT_TOKEN( TOKEN_MODULE );
"using"                                         EMIT_TOKEN( TOKEN_USING );
"component"                                     { EMIT_TOKEN( TOKEN_COMPONENT ); yyextra->SetScope( ECS_LEXER_SCOPE_COMPONENT ); }
"resource"                                      { EMIT_TOKEN( TOKEN_RESOURCE ); yyextra->SetScope( ECS_LEXER_SCOPE_RESOURCE ); }
"defaults"                                      EMIT_TOKEN( TOKEN_DEFAULTS );
"system"                                        { EMIT_TOKEN( TOKEN_SYSTEM ); yyextra->SetScope( ECS_LEXER_SCOPE_SYSTEM ); }
"stage"                                         EMIT_TOKEN( TOKEN_SYSTEM_STAGE );
"on_start"                                      EMIT_TOKEN( TOKEN_SYSTEM_STAGE_ONSTART );
"on_load"                                       EMIT_TOKEN( TOKEN_SYSTEM_STAGE_ONLOAD );
"post_load"                                     EMIT_TOKEN( TOKEN_SYSTEM_STAGE_POSTLOAD );
"pre_update"                                    EMIT_TOKEN( TOKEN_SYSTEM_STAGE_PREUPDATE );
"on_update"                                     EMIT_TOKEN( TOKEN_SYSTEM_STAGE_ONUPDATE );
"on_validate"                                   EMIT_TOKEN( TOKEN_SYSTEM_STAGE_ONVALIDATE );
"post_update"                                   EMIT_TOKEN( TOKEN_SYSTEM_STAGE_POSTUPDATE );
"pre_store"                                     EMIT_TOKEN( TOKEN_SYSTEM_STAGE_PRESTORE );
"on_store"                                      EMIT_TOKEN( TOKEN_SYSTEM_STAGE_ONSTORE );
"read"                                          EMIT_TOKEN( TOKEN_SYSTEM_READ );
"read_optional"                                 EMIT_TOKEN( TOKEN_SYSTEM_READ_OPTIONAL );
"read_resource"                                 EMIT_TOKEN( TOKEN_SYSTEM_READ_RESOURCE );
"write"                                         EMIT_TOKEN( TOKEN_SYSTEM_WRITE );
"write_optional"                                EMIT_TOKEN( TOKEN_SYSTEM_WRITE_OPTIONAL );
"write_resource"                                EMIT_TOKEN( TOKEN_SYSTEM_WRITE_RESOURCE );
"include"                                       EMIT_TOKEN( TOKEN_SYSTEM_INCLUDE );
"exclude"                                       EMIT_TOKEN( TOKEN_SYSTEM_EXCLUDE );
"const"                                         EMIT_TOKEN( TOKEN_CPP_CONST );
"::"                                            EMIT_TOKEN( TOKEN_CPP_NAMESPACE );

    /* Syntax */
":"                                             EMIT_TOKEN( ':' );
","                                             EMIT_TOKEN( ',' );
";"                                             EMIT_TOKEN( ';' );
"="                                             EMIT_TOKEN( '=' );
"<"                                             EMIT_TOKEN( '<' );
">"                                             EMIT_TOKEN( '>' );
"_"                                             EMIT_TOKEN( '_' );
"*"                                             EMIT_TOKEN( '*' );
"&"                                             EMIT_TOKEN( '&' );
"{"                                             { EMIT_TOKEN( '{' ); ++yyextra->scopeLevel; }
"}"                                             { 
                                                    EMIT_TOKEN( '}' ); 
                                                    --yyextra->scopeLevel; 
                                                    if ( yyextra->scopeLevel == 0 )
                                                    {
                                                        yyextra->ResetScope();
                                                    }
                                                }
"["                                             EMIT_TOKEN( '[' );
"]"                                             EMIT_TOKEN( ']' );
"("                                             {
                                                    // Emit only '(' if we in a default scope
                                                    if ( yyextra->IsScopeA( ECS_LEXER_SCOPE_DEFAULT ) )
                                                    {
                                                        EMIT_TOKEN( '(' );
                                                    }
                                                    // Otherwise treat it as C++ text
                                                    else
                                                    {
                                                        if ( yyextra->bracketScopeLevel == 0 )
                                                        {
                                                            EMIT_TOKEN( '(' );
                                                            BEGIN( CppText );
                                                            yyextra->StoreSequenceStart();
                                                            ++yyextra->bracketScopeLevel;
                                                        }
                                                        else
                                                        {
                                                            EMIT_ERROR( "Initial bracket must be out of any brackets" );
                                                        }
                                                    }
                                                }
")"                                             EMIT_TOKEN( ')' );
<CppText>
{
    [^()\n]*
    <*>\n                                       yyextra->NextLine();
    "("                                         ++yyextra->bracketScopeLevel;
    ")"                                         {
                                                    --yyextra->bracketScopeLevel;
                                                    if ( yyextra->bracketScopeLevel == 0 )
                                                    { 
                                                        EMIT_SEQUENCE( TOKEN_CPP_CODE );
                                                        EMIT_TOKEN( ')' );
                                                        BEGIN( INITIAL ); 
                                                    }
                                                }
}


    /* Identifier */
[a-zA-Z_]+[a-zA-Z_0-9]*							EMIT_TOKEN( TOKEN_IDENT );

    /* Invalid Unicode chars */
[^\x00-\x7F]*									EMIT_ERROR( "Invalid Unicode chars" );

    /* Whitespace */
[ \t\r]

    /* String */
\"                                              { yyextra->StoreSequenceStart(); BEGIN( StringLiteral ); }
<StringLiteral>
{
    [^"]*
	\"											{ EMIT_SEQUENCE( TOKEN_STRING ), BEGIN( INITIAL ); }
}

    /* Comments */
"//"[^\r\n]*									{ yyextra->StoreSequenceStart(); EMIT_COMMENT(); }
"/*"											{ yyextra->StoreSequenceStart(); BEGIN( MultiLineComment ); }
<MultiLineComment>
{
	[^*\n]+
	"*"
	"*/"										{ EMIT_COMMENT(); BEGIN( INITIAL ); }
}

    /* Next line */
<*>\n                                           yyextra->NextLine();

    /* Default Rule */
.|\n                                            EMIT_ERROR( "Unknown token" );
%%

void EcsCode_Tokenize( const achar* pSourceCode, CParserLexerListener* pLexerListener )
{
    yyscan_t                    scanner;
    ecsLexerStateInternal_t     lexerState( pSourceCode, pLexerListener );
    
	yylex_init_extra( &lexerState, &scanner );
    YY_BUFFER_STATE     buffer = yy_scan_string( pSourceCode, scanner );    // This buffer will be parsed
    yylex( scanner );

    yy_delete_buffer( buffer, scanner );
    yylex_destroy( scanner );
}