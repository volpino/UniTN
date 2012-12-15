/* parser */
%token <int> INT
%token PLUS MINUS TIMES DIV
%token UNARY_MIN
%token LPAR, RPAR
%token EOL
%left PLUS MINUS  /* lower precedence */
%left TIMES DIV /* higher precedence */
%nonassoc UNARY_MIN

%start main
%type <int> main

%%

main:
  expr EOL                      { $1 }
;

expr:
    INT                         { $1 }
  | expr PLUS expr              { $1 + $3 }
  | expr MINUS expr             { $1 - $3 }
  | expr TIMES expr             { $1 * $3 }
  | expr DIV expr               { $1 / $3 }
  | MINUS expr %prec UNARY_MIN  { -$2 }
  | LPAR expr RPAR              { $2 }
;
