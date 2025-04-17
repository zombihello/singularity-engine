%{
    #include "ecsfileparser_bison.cpp.h"
    #include "core/core.h"
    #include "parserlib/token_stream.h"
    #include "tools/ecscompiler/ecsfileparser.h"

    // Some Bison defines
    #define YYDEBUG                 1
    #define YYERROR_VERBOSE         1
    #define YYPARSE_PARAM           param 
    #define YYLEX_PARAM             param 
    #define YYSTYPE                 yystypeFile_t

    #define g_pFileContext           ( ( CEcsFileParserInterface* )param )
    #define g_pFileParser            ( ( ( CEcsFileParserInterface* )param )->GetFileParser() )

    /**
    * @ingroup ecscompiler
    * @brief Structure to pass around parser parameters
    */
    struct yystypeFile_t
    {
        /**
         * @brief Constructor
         */
        yystypeFile_t()
            : pContext( NULL )
        {}

        /**
         * @brief Constructor of copy
         * @param other   Other yystypeFile_t
         */
        yystypeFile_t( const yystypeFile_t& other )
            : pContext( other.pContext )
            , token( other.token )
        {}

        /**
         * @brief Constructor of move
         * @param other   Other yystypeFile_t
         */
         yystypeFile_t( yystypeFile_t&& other )
            : pContext( std::move( other.pContext ) )
            , token( std::move( other.token ) )
        {}

        /**
         * @brief Destructor
         */
        ~yystypeFile_t()
        {}

        /**
         * @brief Override operator of copy
         * @param other   Other yystypeFile_t
         * @return Return reference to self
         */
        FORCEINLINE yystypeFile_t& operator=( const yystypeFile_t& other )
	    {
	    	if ( this != &other )
	    	{
	    		pContext    = other.pContext;
	    		token       = other.token;
	    	}
	    	return *this;
	    }

        /**
         * @brief Override operator of move
         * @param other   Other yystypeFile_t
         * @return Return reference to self
         */
        FORCEINLINE yystypeFile_t& operator=( yystypeFile_t&& other )
	    {
	    	if ( this != &other )
	    	{
	    		pContext    = std::move( other.pContext );
	    		token       = std::move( other.token );
	    	}
	    	return *this;
	    }

        parserFileContext_t*    pContext;   /**< Parser file context */
        std::string_view        token;      /**< Token in string format */
    };

    /**
    * @ingroup ecscompiler
    * @brief ECS file parser interface
    */
    class CEcsFileParserInterface
    {
    public:
        /**
         * @brief Constructor
         * @param tokens        Tokens stream
         * @param pFileParser   File parser
         */
         CEcsFileParserInterface( CParserTokenStream& tokens, CEcsFileParser& pFileParser )
            : pTokens( &tokens )
            , pFileParser( &pFileParser )
            , pCurrentToken( NULL )
         {}

        /**
         * @brief Get next token
         * @param lvalp   yystypeFile_t
         * @return Return ID of the next token. If is end of stream returns 0
         */
        int yylex( YYSTYPE* lvalp )
        {
            if ( pTokens->IsEndOfStream() )
            {
                return 0;
            }

            // Get next token
            parserToken_t&    token = pTokens->GetReadToken();
            pTokens->IncrementReadPosition();

            // Update yystypeFile_t
            lvalp->pContext     = &token.context;
		    lvalp->token        = token.tokenString;

            // Remember current token
            pCurrentToken = &token;
            return token.tokenID;
        }

        /**
         * @brief Emit error
         * @param pMessage    Error message
         * @param lvalp       yystypeFile_t
         * @return Return zero if need continue, otherwise not-zero value
         */
        int yyerror( const achar* pMessage, YYSTYPE* lvalp )
        {
		    // Emit error and continue
            pFileParser->EmitError( lvalp->pContext, S_Sprintf( "%s, near '%s'", pMessage, !lvalp->token.empty() ? lvalp->token.data() : "<TOKEN_EMPTY>" ).c_str() );
		    return 0;
        }

        /**
         * @brief Get file parser
         * @return Return pointer to the file parser
         */
         CEcsFileParser* GetFileParser()
	    {
		    return pFileParser;
	    }

        /**
         * @brief Get current token line
         * @return Return current token line. If token isn't valid returns -1
         */
        int32 GetCurrentTokenLine() const
        {
            if( !pCurrentToken )
            {
                return -1;
            }
            return pCurrentToken->context.line;
        }

        /**
         * @brief Get last token line
         * @return Returns the line of the last token in current context
         */       
        int32 GetLastTokenLine() const
        {
            if ( pTokens->IsEmpty() )
            {
                return -1;
            }
            return pTokens->GetLastToken().context.line;
        }

    private:
        CParserTokenStream*		pTokens;                /**< Tokens stream */
	    CEcsFileParser*		    pFileParser;            /**< File parser */
        parserToken_t*			pCurrentToken;          /**< Current token */
    };

    // Some Bison functions
    /*
    ==================
    yyerror_file_thread_safe
    Function for Bison to handle error cases
    ==================
    */
    static int yyerror_file_thread_safe( const achar* pMessage, void* pParam, YYSTYPE* lvalp )
    {
        CEcsFileParserInterface*     pFileParserInterface = ( CEcsFileParserInterface* )pParam;
        return pFileParserInterface->yyerror( pMessage, lvalp );
    }

    /*
    ==================
    yylex_file_thread_safe
    Function for Bison to take tokens
    ==================
    */    
    static int yylex_file_thread_safe( YYSTYPE* lvalp, void* pParam )
    {
        CEcsFileParserInterface*     pFileParserInterface = ( CEcsFileParserInterface* )pParam;
        return pFileParserInterface->yylex( lvalp );	
    }

    // Redefine some Bison functions
    #define yyparse                 yyparse_file
    #define yydebug                 yydebug_file
    #define yylex                   yylex_file_thread_safe
    #define yyerror( Message )      yyerror_file_thread_safe( Message, YYPARSE_PARAM, &yylval )
%}

%define api.pure

/* ------------------------------------------------------------------
   Token Definitions
   ------------------------------------------------------------------ */

/* Expect 0 shift/reduce conflicts */
%expect 0

/* Data tokens */
%token TOKEN_IDENT
%token TOKEN_CPP_IDENT
%token TOKEN_STRING

/* Keywords */
%token TOKEN_MODULE
%token TOKEN_USING
%token TOKEN_COMPONENT
%token TOKEN_DEFAULTS
%token TOKEN_SYSTEM
%token TOKEN_CPP_CODE
%token TOKEN_METAINFO_STAGE
%token TOKEN_METAINFO_READ
%token TOKEN_METAINFO_WRITE
%token TOKEN_METAINFO_INCLUDE
%token TOKEN_METAINFO_EXCLUDE

%%

////////////////////////////////
// Module
////////////////////////////////

module
    : module_header module_body                                         { g_pFileParser->EndDefinition( g_pFileContext->GetCurrentTokenLine(), $<pContext>1, $<pContext>2 ); }
    | /* empty */
    ;

module_header
    : TOKEN_MODULE TOKEN_IDENT ';'                                      { g_pFileParser->StartModule( $<pContext>2, $<token>2.data() ); }
    ;

module_body
    : module_body using                                                 { $<pContext>$ = $<pContext>2; }
    | module_body component                                             { $<pContext>$ = $<pContext>2; }
    | module_body semicolon                                             {}
    | /* empty */
    ;

////////////////////////////////
// Using
////////////////////////////////

using
    : TOKEN_USING TOKEN_STRING ';'                                      { g_pFileParser->AddUsing( $<pContext>2, $<token>2.data() ); $<pContext>$ = $<pContext>3; }
    ;

////////////////////////////////
// Component
////////////////////////////////

component
    : component_header '{' component_body '}'                           { g_pFileParser->EndDefinition( g_pFileContext->GetCurrentTokenLine(), $<pContext>1, $<pContext>4  ); $<pContext>$ = $<pContext>4; }
    ;

component_header
    : TOKEN_COMPONENT TOKEN_IDENT                                       { g_pFileParser->StartComponent( $<pContext>2, $<token>2.data() ); $<pContext>$ = $<pContext>2; }
    ;

component_body
    : component_body component_constructor                              {}
    | component_body component_field                                    {}
    | component_body semicolon                                          {}
    | /* empty */
    ;

component_constructor
    : TOKEN_DEFAULTS ';'                                                {}
    | TOKEN_DEFAULTS '=' '>' component_constructor_fields ';'           {}
    ;

component_constructor_fields
    : component_constructor_field ',' component_constructor_fields      {}
    | component_constructor_field                                       {}
    ;

component_constructor_field
    : TOKEN_IDENT '(' TOKEN_CPP_CODE ')'                                { g_pFileParser->SetDefaultFieldValue( $<pContext>1, $<pContext>3, $<token>1.data(), $<token>3.data() ); }
    ;

component_field
    : TOKEN_CPP_IDENT TOKEN_IDENT ';'                                   { g_pFileParser->AddField( $<pContext>2, $<pContext>1, $<token>2.data(), $<token>1.data() ); }
    | TOKEN_IDENT TOKEN_IDENT ';'                                       { g_pFileParser->AddField( $<pContext>2, $<pContext>1, $<token>2.data(), $<token>1.data() ); }
    ;

////////////////////////////////
// TERMINALS
////////////////////////////////

semicolon
    : ';'
    ;

%%

/**
 * @ingroup ecscompiler
 * @brief Entry function to parse grammar a ECS source code
 * @param tokens        Token stream
 * @param fileParser    File parser
 * @return Return the result returned by yyparse
 */
int EcsCode_GrammarFile( CParserTokenStream& tokens, CEcsFileParser& fileParser )
{
    CEcsFileParserInterface      fileParserInterface( tokens, fileParser );
    return yyparse( &fileParserInterface );
}