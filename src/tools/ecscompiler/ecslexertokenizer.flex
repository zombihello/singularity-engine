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
    #include "parserlib/lexer_listener.h"

    #define YY_FATAL_ERROR( Message )           { yyguts_t* yyg = ( struct yyguts_t* )yyscanner; yyextra->EmitError( Message ); }
    #define YY_USER_ACTION                      yyextra->StoreTokenStart(); yyextra->UpdateContext( yytext, yyleng );

    #define EMIT_TOKEN( TokeID )                yyextra->EmitToken( TokeID )
    #define EMIT_SEQUENCE( TokeID )             yyextra->EmitSequence( TokeID )
    #define EMIT_COMMENT()                      yyextra->EmitComment()
    #define EMIT_ERROR( Message )               yyextra->EmitError( Message )

    /**
     * @ingroup ecscompiler
     * @brief Lexer scope type
     */
    enum ecsLexerScope_t
    {
        ECS_LEXER_SCOPE_DEFAULT,    /**< Default */
        ECS_LEXER_SCOPE_COMPONENT,  /**< Component */
        ECS_LEXER_SCOPE_SYSTEM      /**< System */
    };

    /*
    ==================
    EcsLexer_CountUtf8Bytes
    ==================
    */
    static uint32 EcsLexer_CountUtf8Bytes( const achar uint8Byte )
    {
        unsigned long    index   = 0;
        unsigned long    mask    = ( ~uint8Byte );

        #if PLATFORM_WINDOWS
            _BitScanReverse( &index, mask );
        #else
            // We're counting the bits 
	        index = 31 - __lzcnt32( mask );
        #endif // PLATFORM_WINDOWS

        // We have the 0-index context of the most significant bit, which we need to turn into a count of the number of bits (or 1-index context)
	    // Then reverse the order so that we have a count of the number of bits leading up to the MSB
	    return 8 - ( index + 1 );
    }

    /*
    ==================
    EcsLexer_CountCharacters
    ==================
    */  
    static uint32 EcsLexer_CountCharacters( const achar* pToken, uint32 size )
    {
        uint32      length = 0;
	    for ( uint32 index = 0; index < size; ++index )
	    {
	    	if ( ( pToken[index] & 0x80 ) )
	    	{
	    		const uint32    sizeOfUtf8Character = EcsLexer_CountUtf8Bytes( pToken[index] );
	    		const uint32    bytesToSkip         = sizeOfUtf8Character - 1;
			    index += bytesToSkip;
		    }
		    ++length;
	    }

	    return length;   
    }

    /**
     * @ingroup ecscompiler
     * @brief The lexer's internal state
     */
    struct ecsLexerStateInternal_t : public parserLexerState_t
    {
        /**
         * @brief Constructor
         * @param pSource          The raw input buffer
         * @param pLexerListener   Lexer listener
         */
         ecsLexerStateInternal_t( const achar* pSourceCode, CParserLexerListenerBase* pLexerListener )
            : parserLexerState_t( pSourceCode )
            , lexerScope( ECS_LEXER_SCOPE_DEFAULT )
            , bracketScopeLevel( 0 )
            , pLexerListener( pLexerListener )
        {}

        /**
         * @brief Next line
         */
        FORCEINLINE void NextLine()
        {
            ++currentContext.line;
            lineStart = currentContext;
        }

        /**
         * @brief Start to store token
         */
        FORCEINLINE void StoreTokenStart()
        {
            tokenStart = currentContext;
        }

        /**
         * @brief Start to store sequence
         * @param charactersToSkip    Number characters to skip
         */
        FORCEINLINE void StoreSequenceStart( uint32 charactersToSkip = 0 )
        {
            sequenceStart = tokenStart;
            if ( charactersToSkip )
            {
                sequenceStart.byteOffset    += charactersToSkip;
                sequenceStart.charOffset    += charactersToSkip;
            }

            sequenceLineStart = lineStart;
        }

        /**
         * @brief Update current context
         * @param pToken            The token
         * @param tokenByteLength   Length of the token in bytes
         */
        FORCEINLINE void UpdateContext( const achar* pToken, uint32 tokenByteLength )
        {
            currentContext.byteOffset += tokenByteLength;
            currentContext.charOffset += EcsLexer_CountCharacters( pToken, tokenByteLength );
        }

        /**
         * @brief Emit token
         * @param tokenID     Token ID
         */
        FORCEINLINE void EmitToken( uint32 tokenID )
        {
            pLexerListener->Token( *this, tokenID );
        }

        /**
         * @brief Emit sequence
         * @param tokenID     Token ID
         */
        FORCEINLINE void EmitSequence( uint32 tokenID )
        {
            pLexerListener->Sequence( *this, tokenID );
        }

        /**
         * @brief Emit comment
         */
        FORCEINLINE void EmitComment()
        {
            pLexerListener->Comment( *this );
        }

        /**
         * @brief Emit error
         * @param pMessage  Message
         */
        FORCEINLINE void EmitError( const achar* pMessage )
        {
            pLexerListener->Error( *this, pMessage );
        }
 
        /**
         * @brief Set lexer scope
         * @param lexerScope    A new lexer scope
         */
        FORCEINLINE void SetScope( ecsLexerScope_t lexerScope )
        {
            if (  ecsLexerStateInternal_t::lexerScope != ECS_LEXER_SCOPE_DEFAULT || scopeLevel > 0 )
            {
                EmitError( "Invalid changing scope, components, systems and etc - must be outside of any scopes" );
            }
            ecsLexerStateInternal_t::lexerScope = lexerScope;
        }

        /**
         * @brief Reset the lexer scope
         */
        FORCEINLINE void ResetScope()
        {
            if ( scopeLevel > 0 )
            {
                EmitError( "Invalid changing scope to main one, something went wrong with scope levels" );
            }
            lexerScope = ECS_LEXER_SCOPE_DEFAULT;
        }

        /**
         * @brief Is current scope a scope
         * @param lexerScope    Scope to check
         * @return Return TRUE if the current scope is the lexerScope, otherwise FALSE
         */
        FORCEINLINE bool IsScopeA( ecsLexerScope_t lexerScope ) const
        {
            return ecsLexerStateInternal_t::lexerScope == lexerScope;
        }

        uint32                          bracketScopeLevel;  /**< Bracket scope level */

    private:
        ecsLexerScope_t                 lexerScope;         /**< Lexer scope */
        CParserLexerListenerBase*       pLexerListener;     /**< Lexer listener */
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
"defaults"                                      EMIT_TOKEN( TOKEN_DEFAULTS );
"system"                                        { EMIT_TOKEN( TOKEN_SYSTEM ); yyextra->SetScope( ECS_LEXER_SCOPE_SYSTEM ); }
"stage"                                         EMIT_TOKEN( TOKEN_METAINFO_STAGE );
"read"                                          EMIT_TOKEN( TOKEN_METAINFO_READ );
"write"                                         EMIT_TOKEN( TOKEN_METAINFO_WRITE );
"include"                                       EMIT_TOKEN( TOKEN_METAINFO_INCLUDE );
"exclude"                                       EMIT_TOKEN( TOKEN_METAINFO_EXCLUDE );

    /* Syntax */
":"                                             EMIT_TOKEN( ':' );
","                                             EMIT_TOKEN( ',' );
";"                                             EMIT_TOKEN( ';' );
"="                                             EMIT_TOKEN( '=' );
">"                                             EMIT_TOKEN( '>' );
"_"                                             EMIT_TOKEN( '_' );
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
([a-zA-Z_][a-zA-Z_0-9]*|::|<|>|,|\*|&)+         EMIT_TOKEN( TOKEN_CPP_IDENT );

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

/**
 * @ingroup ecscompiler
 * @brief Entry function to tokenize ECS source code
 * @param pSourceCode      Source code
 * @param pLexerListener   Lexer listener
 */
void EcsCode_Tokenize( const achar* pSourceCode, CParserLexerListenerBase* pLexerListener )
{
    yyscan_t                    scanner;
    ecsLexerStateInternal_t     lexerState( pSourceCode, pLexerListener );
    
	yylex_init_extra( &lexerState, &scanner );
    YY_BUFFER_STATE     buffer = yy_scan_string( pSourceCode, scanner );    // This buffer will be parsed
    yylex( scanner );

    yy_delete_buffer( buffer, scanner );
    yylex_destroy( scanner );
}