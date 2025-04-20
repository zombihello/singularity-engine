%{
    #include "ecsfileparser_bison.cpp.h"
    #include "core/core.h"
    #include "parserlib/tokenstream.h"
    #include "parserlib/grammarinterface.h"
    #include "tools/ecscompiler/ecsfileparser.h"

    // Some Bison defines
    #define YYDEBUG                 1
    #define YYERROR_VERBOSE         1
    #define YYPARSE_PARAM           param 
    #define YYLEX_PARAM             param 
    #define YYSTYPE                 yystypeFile_t

    struct yystypeFile_t
    {
        yystypeFile_t()
            : pContext( NULL )
        {}
        yystypeFile_t( const yystypeFile_t& other )
            : pContext( other.pContext )
            , token( other.token )
        {}
         yystypeFile_t( yystypeFile_t&& other )
            : pContext( std::move( other.pContext ) )
            , token( std::move( other.token ) )
        {}
        ~yystypeFile_t()
        {}

        FORCEINLINE yystypeFile_t& operator=( const yystypeFile_t& other )
	    {
	    	if ( this != &other )
	    	{
	    		pContext    = other.pContext;
	    		token       = other.token;
	    	}
	    	return *this;
	    }

        FORCEINLINE yystypeFile_t& operator=( yystypeFile_t&& other )
	    {
	    	if ( this != &other )
	    	{
	    		pContext    = std::move( other.pContext );
	    		token       = std::move( other.token );
	    	}
	    	return *this;
	    }

        parserFileContext_t*    pContext;
        std::string_view        token;
    };

    typedef TGrammarInterface<CEcsFileParser, yystypeFile_t>        ecsGrammarInterface_t;
    #define g_pFileContext                                          ( ( ecsGrammarInterface_t* )param )
    #define g_pFileParser                                           ( ( ( ecsGrammarInterface_t* )param )->GetFileParser() )

    // Some Bison functions
    /*
    ==================
    yyerror_file_thread_safe
    Function for Bison to handle error cases
    ==================
    */
    static int yyerror_file_thread_safe( const achar* pMessage, void* pParam, YYSTYPE* lvalp )
    {
        ecsGrammarInterface_t*     pGrammarInterface = ( ecsGrammarInterface_t* )pParam;
        pGrammarInterface->EmitError( pMessage, lvalp );
        return 0;
    }

    /*
    ==================
    yylex_file_thread_safe
    Function for Bison to take tokens
    ==================
    */    
    static int yylex_file_thread_safe( YYSTYPE* lvalp, void* pParam )
    {
        ecsGrammarInterface_t*     pGrammarInterface = ( ecsGrammarInterface_t* )pParam;
        return ( int )pGrammarInterface->GetNextToken( lvalp );	
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
%token TOKEN_STRING

/* Keywords */
%token TOKEN_MODULE
%token TOKEN_USING
%token TOKEN_COMPONENT
%token TOKEN_DEFAULTS
%token TOKEN_SYSTEM
%token TOKEN_CPP_CODE
%token TOKEN_SYSTEM_STAGE
%token TOKEN_SYSTEM_STAGE_ONSTART
%token TOKEN_SYSTEM_STAGE_ONLOAD
%token TOKEN_SYSTEM_STAGE_POSTLOAD
%token TOKEN_SYSTEM_STAGE_PREUPDATE
%token TOKEN_SYSTEM_STAGE_ONUPDATE
%token TOKEN_SYSTEM_STAGE_ONVALIDATE
%token TOKEN_SYSTEM_STAGE_POSTUPDATE
%token TOKEN_SYSTEM_STAGE_PRESTORE
%token TOKEN_SYSTEM_STAGE_ONSTORE
%token TOKEN_SYSTEM_READ
%token TOKEN_SYSTEM_WRITE
%token TOKEN_SYSTEM_INCLUDE
%token TOKEN_SYSTEM_EXCLUDE

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
    | module_body system                                                { $<pContext>$ = $<pContext>2; }
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
    : metadata TOKEN_COMPONENT TOKEN_IDENT                              { g_pFileParser->StartComponent( $<pContext>3, $<token>3.data() ); $<pContext>$ = $<pContext>3; }
    ;

component_body
    : component_body component_constructor                              {}
    | component_body component_field                                    {}
    | component_body semicolon                                          {}
    | /* empty */
    ;

component_constructor
    : TOKEN_DEFAULTS ':' component_constructor_fields ';'               {}
    ;

component_constructor_fields
    : component_constructor_field ',' component_constructor_fields      {}
    | component_constructor_field                                       {}
    ;

component_constructor_field
    : TOKEN_IDENT '(' TOKEN_CPP_CODE ')'                                { g_pFileParser->SetComponentDefaultFieldValue( $<pContext>1, $<pContext>3, $<token>1.data(), $<token>3.data() ); }
    ;

component_field
    : metadata TOKEN_IDENT TOKEN_IDENT ';'                              { g_pFileParser->AddComponentField( $<pContext>3, $<pContext>2, $<token>3.data(), $<token>2.data() ); }
    ;

////////////////////////////////
// System
////////////////////////////////

system
    : system_header '{' system_body '}'                                 { g_pFileParser->EndDefinition( g_pFileContext->GetCurrentTokenLine(), $<pContext>1, $<pContext>4  ); $<pContext>$ = $<pContext>4; }
    ;

system_header
    : metadata TOKEN_SYSTEM TOKEN_IDENT                                 { g_pFileParser->StartSystem( $<pContext>3, $<token>3.data() ); $<pContext>$ = $<pContext>3; }
    ;

system_body
    : system_body system_stage semicolon                                {}
    | system_body system_field semicolon                                {}
    | system_body system_filter semicolon                               {}
    | system_body semicolon                                             {}
    | /* empty */
    ;

system_stage
    : TOKEN_SYSTEM_STAGE ':' TOKEN_SYSTEM_STAGE_ONSTART                 { g_pFileParser->SetSystemStage( $<pContext>3, ECS_SYSTEM_STAGE_ONSTART ); }
    | TOKEN_SYSTEM_STAGE ':' TOKEN_SYSTEM_STAGE_ONLOAD                  { g_pFileParser->SetSystemStage( $<pContext>3, ECS_SYSTEM_STAGE_ONLOAD ); }
    | TOKEN_SYSTEM_STAGE ':' TOKEN_SYSTEM_STAGE_POSTLOAD                { g_pFileParser->SetSystemStage( $<pContext>3, ECS_SYSTEM_STAGE_POSTLOAD ); }
    | TOKEN_SYSTEM_STAGE ':' TOKEN_SYSTEM_STAGE_PREUPDATE               { g_pFileParser->SetSystemStage( $<pContext>3, ECS_SYSTEM_STAGE_PREUPDATE ); }
    | TOKEN_SYSTEM_STAGE ':' TOKEN_SYSTEM_STAGE_ONUPDATE                { g_pFileParser->SetSystemStage( $<pContext>3, ECS_SYSTEM_STAGE_ONUPDATE ); }
    | TOKEN_SYSTEM_STAGE ':' TOKEN_SYSTEM_STAGE_ONVALIDATE              { g_pFileParser->SetSystemStage( $<pContext>3, ECS_SYSTEM_STAGE_ONVALIDATE ); }
    | TOKEN_SYSTEM_STAGE ':' TOKEN_SYSTEM_STAGE_POSTUPDATE              { g_pFileParser->SetSystemStage( $<pContext>3, ECS_SYSTEM_STAGE_POSTUPDATE ); }
    | TOKEN_SYSTEM_STAGE ':' TOKEN_SYSTEM_STAGE_PRESTORE                { g_pFileParser->SetSystemStage( $<pContext>3, ECS_SYSTEM_STAGE_PRESTORE ); }
    | TOKEN_SYSTEM_STAGE ':' TOKEN_SYSTEM_STAGE_ONSTORE                 { g_pFileParser->SetSystemStage( $<pContext>3, ECS_SYSTEM_STAGE_ONSTORE ); }
    ;

system_field
    : TOKEN_SYSTEM_READ ':' TOKEN_IDENT TOKEN_IDENT                     { g_pFileParser->AddSystemField( $<pContext>4, $<pContext>3, $<token>4.data(), $<token>3.data(), ECS_FIELD_ACCESS_TYPE_READ ); }
    | TOKEN_SYSTEM_WRITE ':' TOKEN_IDENT TOKEN_IDENT                    { g_pFileParser->AddSystemField( $<pContext>4, $<pContext>3, $<token>4.data(), $<token>3.data(), ECS_FIELD_ACCESS_TYPE_WRITE ); }
    ;

system_filter
    : TOKEN_SYSTEM_INCLUDE ':' TOKEN_IDENT                              { g_pFileParser->AddSystemFilter( $<pContext>3, $<token>3.data(), ECS_SYSTEM_FILTER_TYPE_INCLUDE ); }
    | TOKEN_SYSTEM_EXCLUDE ':' TOKEN_IDENT                              { g_pFileParser->AddSystemFilter( $<pContext>3, $<token>3.data(), ECS_SYSTEM_FILTER_TYPE_EXCLUDE ); }
    ;

////////////////////////////////
// Metadata
////////////////////////////////

metadata
    : metadata '[' metadata_values ']'                                  {}
    | /* empty */
    ;

metadata_values
    : metadata_value ',' metadata_values                                {}
    | metadata_value                                                    {}
    ;

metadata_value
    : TOKEN_IDENT                                                       { g_pFileParser->AddMetadata( $<pContext>1, NULL, $<token>1.data(), NULL ); }
    | TOKEN_IDENT '=' TOKEN_IDENT                                       { g_pFileParser->AddMetadata( $<pContext>1, $<pContext>3, $<token>1.data(), $<token>3.data() ); }
    | TOKEN_IDENT '=' TOKEN_STRING                                      { g_pFileParser->AddMetadata( $<pContext>1, $<pContext>3, $<token>1.data(), $<token>3.data() ); }
    ;

////////////////////////////////
// TERMINALS
////////////////////////////////

semicolon
    : ';'
    ;

%%

int EcsCode_GrammarFile( CParserTokenStream& tokens, CEcsFileParser& fileParser )
{
    ecsGrammarInterface_t      grammarInterface( tokens, fileParser );
    return yyparse( &grammarInterface );
}