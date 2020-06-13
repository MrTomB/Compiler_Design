%{
    
    #include "lyutils.h"
    #include "astree.h"
    #include "assert.h"
    
    %}

%debug
%defines
%error-verbose
%token-table
%verbose

%token TOK_VOID TOK_BOOL TOK_CHAR TOK_INT TOK_STRING
%token TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_FALSE TOK_TRUE TOK_NULL TOK_NEW TOK_ARRAY
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON
%token TOK_BAD_IDENT TOK_BAD_CHAR TOK_BAD_STRING

%token TOK_FUNCTION TOK_PARAMLIST TOK_BLOCK TOK_DECLID TOK_PROTOTYPE
%token TOK_FIELDDECL TOK_TYPE TOK_BASETYPE 
%token TOK_BINOP TOK_UNOP TOK_VARIABLE
%token TOK_CONSTANT

%token TOK_CALL TOK_IFELSE TOK_INITDECL TOK_RETURNVOID TOK_INDEX
%token TOK_POS TOK_NEG TOK_NEWARRAY TOK_NEWSTRING TOK_TYPEID TOK_FIELD TOK_VARDECL
%token TOK_ROOT

%right  TOK_IF 'then' TOK_ELSE
%right  '='
%left   TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%left   '+' '-'
%left   '*' '/' '%'
%right  TOK_POS TOK_NEG '!' TOK_ORD TOK_CHR
%left   '[' '.'
%nonassoc TOK_NEW

%start start

%%

start   : program            { yyparse_astree = $1; }
        ;

program : program structdef  { $$ = adopt1 ($1, $2); }
        | program function   { $$ = adopt1 ($1, $2); }
        | program statement  { $$ = adopt1 ($1, $2); }
        | program error '}'  { $$ = $1; }
        | program error ';'  { $$ = $1; }
        |                    { $$ = new_parseroot (); }
        ;

structdef   : TOK_STRUCT TOK_IDENT '{' members '}' { 
                               free_ast2($3, $5);
                               $$ = adopt2($1, $2, $4); }
            | TOK_STRUCT TOK_IDENT '{' '}' {
                               free_ast2($3, $4);
                               $$ = adopt1($1, $2); }
            ;

members     : members decl ';' { free_ast($3); $$ = adopt1($1, $2); }
            | decl ';'            { free_ast($2); $$ = $1; }
            ;

parameters  : parameters ',' decl  { $$ = adopt1($1, $3);
                                    free_ast($2); }
            | decl                { $$ = $1; }
            ;

decl        : type TOK_IDENT      { astree* field_decl = 
                        new astree(TOK_FIELDDECL, 0, 0, 0, "FIELDDECL");
                                    $$ = adopt2(field_decl, $1, $2); }
            ;

type        : basetype TOK_ARRAY  { 
                   astree* type = new astree(TOK_TYPE, 0, 0, 0, "TYPE");
                                    adopt1($2, $1);
                                    $$ = adopt1(type, $2); }
            | basetype            { 
                   astree* type = new astree(TOK_TYPE, 0, 0, 0, "TYPE");
                                    $$ = adopt1(type, $1); }
            ;

basetype    : TOK_VOID            { 
       astree* basetype = new astree(TOK_BASETYPE, 0, 0, 0, "BASETYPE");
                                    $$ = adopt1(basetype, $1); }
            | TOK_BOOL            { 
       astree* basetype = new astree(TOK_BASETYPE, 0, 0, 0, "BASETYPE");
                                    $$ = adopt1(basetype, $1); }
            | TOK_CHAR            { 
       astree* basetype = new astree(TOK_BASETYPE, 0, 0, 0, "BASETYPE");
                                    $$ = adopt1(basetype, $1); }
            | TOK_INT             { 
       astree* basetype = new astree(TOK_BASETYPE, 0, 0, 0, "BASETYPE");
                                    $$ = adopt1(basetype, $1); }
            | TOK_STRING          {
       astree* basetype = new astree(TOK_BASETYPE, 0, 0, 0, "BASETYPE");
                                    $$ = adopt1(basetype, $1); }
            | TOK_IDENT           {
       astree* basetype = new astree(TOK_BASETYPE, 0, 0, 0, "BASETYPE");
                                    change_symbol($1, TOK_TYPEID);
                                    $$ = adopt1(basetype, $1); }
            ;

function    : type TOK_IDENT '(' parameters ')' block  { 
                                    change_symbol($5, TOK_FUNCTION);
                                    change_symbol($3, TOK_PARAMLIST);
                                    adopt1($3, $4);
                                    adopt1($5, $1);
                                    adopt1($5, $2);
                                    adopt1($5, $3);
                                    $$ = adopt1($5, $6); }
            | type TOK_IDENT '(' ')' block            { 
                                    change_symbol($4, TOK_FUNCTION);
                                    change_symbol($3, TOK_PARAMLIST);
                                    adopt1($4, $1);
                                    adopt1($4, $2);
                                    adopt1($4, $3);
                                    $$ = adopt1($4, $5); }
            ;

block       : '{' zomstatement '}' { 
                                    free_ast($3);
                                    $$ = adopt1sym($1, $2, TOK_BLOCK); }
            | '{' '}'              { 
                                    change_symbol($1, TOK_BLOCK);
                                    free_ast($2);
                                    $$ = $1; }
            | ';'                  { $$ = $1; }
            ;

zomstatement: zomstatement statement { $$ = adopt1($1, $2) }
            | statement            { $$ = $1; }
            ;

statement   : block                { $$ = $1; }
            | vardecl              { $$ = $1; }
            | while                { $$ = $1; }
            | ifelse               { $$ = $1; }
            | return               { $$ = $1; }
            | expr ';'             { free_ast($2); $$ = $1; }
            ;

vardecl     : type TOK_IDENT '=' expr ';' { 
                                     change_symbol($2, TOK_DECLID);
                                     change_symbol($3, TOK_VARDECL);
                                     adopt1($3, $1);
                                     adopt1($3, $2);
                                     adopt1($3, $4);
                                     free_ast($5);
                                     $$ = $3; }
            ;

while       : TOK_WHILE '(' expr ')' statement  { 
                                     $$ = adopt2($1, $3, $5);
                                     free_ast2($2, $4); }
            ;

ifelse      : TOK_IF '(' expr ')' statement %prec 'then' { 
                                     $$ = adopt2($1, $3, $5);
                                     free_ast2($2, $4); }
            | TOK_IF '(' expr ')' statement TOK_ELSE statement { 
                                     change_symbol($1, TOK_IFELSE);
                                     adopt2($1, $3, $5);
                                     $$ = adopt1($1, $7);
                                     free_ast2($2, $4);
                                     free_ast($6); }
            ;

return      : TOK_RETURN ';'   {     change_symbol($1, TOK_RETURNVOID);
                                     $$ = $1;
                                     free_ast($2); }
            | TOK_RETURN expr ';'  { $$ = adopt1($1, $2);
                                     free_ast($3); }
            ;

zomexprwc   : zomexprwc ',' expr   { $$ = adopt1($1, $3);
                                     free_ast($2); }
            | expr                 { $$ = $1; }
            ;

expr        : unop                 { $$ = $1; }
            | binop                { $$ = $1; }
            | allocator            { $$ = $1; }
            | call                 { $$ = $1; }
            | '(' expr ')'         { $$ = $2;
                                     free_ast2($1, $3); }
            | variable             { $$ = $1; }
            | constant             { $$ = $1; }
            ;

unop        : TOK_POS expr         { 
                   astree* unop = new astree(TOK_UNOP, 0, 0, 0, "UNOP");
                                     adopt1($1, $2);
                                     $$ = adopt1(unop, $1); }
            | TOK_NEG expr         {
                   astree* unop = new astree(TOK_UNOP, 0, 0, 0, "UNOP");
                                     adopt1($1, $2);
                                     $$ = adopt1(unop, $1); }
            | '!' expr             {
                   astree* unop = new astree(TOK_UNOP, 0, 0, 0, "UNOP");
                                     adopt1($1, $2);
                                     $$ = adopt1(unop, $1); }
            | TOK_ORD expr         {
                   astree* unop = new astree(TOK_UNOP, 0, 0, 0, "UNOP");
                                     adopt1($1, $2);
                                     $$ = adopt1(unop, $1); }
            | TOK_CHR expr         {
                   astree* unop = new astree(TOK_UNOP, 0, 0, 0, "UNOP");
                                     adopt1($1, $2);
                                     $$ = adopt1(unop, $1); }
            ;

binop       : expr '+' expr        {
                astree* binop = new astree(TOK_BINOP, 0, 0, 0, "BINOP");
                                     adopt2($2, $1, $3);
                                     $$ = adopt1(binop, $2); }
            | expr '-' expr        {
                astree* binop = new astree(TOK_BINOP, 0, 0, 0, "BINOP");
                                     adopt2($2, $1, $3);
                                     $$ = adopt1(binop, $2); }
            | expr '*' expr        {
                astree* binop = new astree(TOK_BINOP, 0, 0, 0, "BINOP");
                                     adopt2($2, $1, $3);
                                     $$ = adopt1(binop, $2); }
            | expr '/' expr        {
                astree* binop = new astree(TOK_BINOP, 0, 0, 0, "BINOP");
                                     adopt2($2, $1, $3);
                                     $$ = adopt1(binop, $2); }
            | expr '%' expr        { 
                astree* binop = new astree(TOK_BINOP, 0, 0, 0, "BINOP");
                                     adopt2($2, $1, $3);
                                     $$ = adopt1(binop, $2); }
            | expr '=' expr        {
                astree* binop = new astree(TOK_BINOP, 0, 0, 0, "BINOP");
                                     adopt2($2, $1, $3);
                                     $$ = adopt1(binop, $2); }
            | expr TOK_EQ expr     {
                astree* binop = new astree(TOK_BINOP, 0, 0, 0, "BINOP");
                                     adopt2($2, $1, $3);
                                     $$ = adopt1(binop, $2); }
            | expr TOK_NE expr     {
                astree* binop = new astree(TOK_BINOP, 0, 0, 0, "BINOP");
                                     adopt2($2, $1, $3);
                                     $$ = adopt1(binop, $2); }
            | expr TOK_GT expr     {
                astree* binop = new astree(TOK_BINOP, 0, 0, 0, "BINOP");
                                     adopt2($2, $1, $3);
                                     $$ = adopt1(binop, $2); }
            | expr TOK_LT expr     {
                astree* binop = new astree(TOK_BINOP, 0, 0, 0, "BINOP");
                                     adopt2($2, $1, $3);
                                     $$ = adopt1(binop, $2); }
            | expr TOK_GE expr     {
                astree* binop = new astree(TOK_BINOP, 0, 0, 0, "BINOP");
                                     adopt2($2, $1, $3);
                                     $$ = adopt1(binop, $2); }
            | expr TOK_LE expr     {
                astree* binop = new astree(TOK_BINOP, 0, 0, 0, "BINOP");
                                     adopt2($2, $1, $3);
                                     $$ = adopt1(binop, $2); }
            ;

allocator   : TOK_NEW TOK_NEWSTRING '(' expr ')'  { 
                                     $$ = adopt2($1, $2, $4);
                                     free_ast2($3, $5); }
            | TOK_NEW TOK_IDENT '(' ')'        {
                                     $$ = adopt1($1, $2);
                                     free_ast2($3, $4); }
            | TOK_NEW basetype '[' expr ']'    {
                                     change_symbol($1, TOK_NEWARRAY);
                                     $$ = adopt2($1, $2, $4);
                                     free_ast2($3, $5); }
            ;

call        : TOK_IDENT '(' zomexprwc ')'      {
                   astree* call = new astree(TOK_CALL, 0, 0, 0, "CALL");
                                     adopt1(call, $3);
                                     free_ast2($2, $4);
                                     $$ = adopt1(call, $1); }
            | TOK_IDENT '(' ')'                {
                   astree* call = new astree(TOK_CALL, 0, 0, 0, "CALL");
                                     $$ = adopt1(call, $1);
                                     free_ast2($2, $3); }
            ;

variable    : TOK_IDENT            {
            astree* var = new astree(TOK_VARIABLE, 0, 0, 0, "VARIABLE");
                                     $$ = adopt1(var, $1); }
            | expr '[' expr ']'    {
            astree* var = new astree(TOK_VARIABLE, 0, 0, 0, "VARIABLE");
                                     change_symbol($2, TOK_INDEX);
                                     adopt2($2, $1, $3);
                                     $$ = adopt1(var, $2);
                                     free_ast($4); }
            | expr '.' TOK_IDENT   { 
            astree* var = new astree(TOK_VARIABLE, 0, 0, 0, "VARIABLE");
                                     change_symbol($3, TOK_FIELD);
                                     adopt2($2, $1, $3);
                                     $$ = adopt1(var, $2); }
            ;

constant    : TOK_INTCON           { $$ = $1; }
            | TOK_CHARCON          { $$ = $1; }
            | TOK_STRINGCON        { $$ = $1; }
            | TOK_FALSE            { $$ = $1; }
            | TOK_TRUE             { $$ = $1; }
            | TOK_NULL             { $$ = $1; }
            ;

%%

const char *get_yytname (int symbol) {
    return yytname [YYTRANSLATE (symbol)];
}


bool is_defined_token (int symbol) {
    return YYTRANSLATE (symbol) > YYUNDEFTOK;
}

/*static void* yycalloc (size_t size) {
 void* result = calloc (1, size);
 assert (result != NULL);
 return result;
 }*/

