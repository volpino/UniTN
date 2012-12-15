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
  | PLUS num_list               { List.fold_left ( + ) 0 $2 }
  | MINUS num_list              { List.fold_left ( - ) 0 $2 }
  | TIMES num_list              { List.fold_left ( * ) 1 $2 }
  | DIV num_list                { let f x = match x with
                                    h::t -> List.fold_left ( / ) h t
                                  | [] -> 0
                                  in f $2 }
  | MINUS expr %prec UNARY_MIN  { -$2 }
  | LPAR expr RPAR              { $2 }
;

num_list:
    expr num_list                { $1::$2}
  | expr                         { $1::[] }
;
