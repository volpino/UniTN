(* main program *)

let _ =
	try
		let lexbuf = Lexing.from_channel stdin in
		while true do
			let result = Calc_parser.main Calc_lexer.token lexbuf in
				print_int result; print_newline(); flush stdout
		done
	with Calc_lexer.Eof -> exit 0