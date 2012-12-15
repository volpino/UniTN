(* lexer *)

(* header section*)
{
  open Calc_parser (* the type token is in the module parser *)
  exception Eof
}

(* definitions section *)
let white_space = [' ' '\t']

(* rules section *)
rule token = parse
    white_space       { token lexbuf }    (* skip the white spaces *)
  | ['\n']            { EOL }
  | ['0'-'9']+ as lxm { INT(int_of_string lxm ) }
  | '+'               { PLUS }
  | '-'               { MINUS }
  | '*'               { TIMES }
  | '/'               { DIV }
  | '('               { LPAR }
  | ')'               { RPAR }
  | eof               { raise Eof }
