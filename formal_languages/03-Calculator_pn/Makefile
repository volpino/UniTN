all: calc_lexer.cmo calc_parser.cmo calc_main.cmo
	ocamlc -o calc $^

calc_lexer.ml : calc_lexer.mll calc_parser.cmi
	ocamllex calc_lexer.mll

calc_parser.ml : calc_parser.mly
	ocamlyacc calc_parser.mly

calc_parser.cmi : calc_parser.ml
	ocamlc -c calc_parser.mli

calc_parser.cmo : calc_parser.ml calc_parser.mli
	ocamlc -c calc_parser.ml

%.cmo : %.ml
	ocamlc -c $<

clean:
	rm -rf calc_parser.mli calc_parser.ml calc_lexer.ml *.cmo *.cmi calc

.PHONY: clean
