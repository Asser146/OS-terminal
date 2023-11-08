
/*
 * CS-413 Spring 98
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%token	<string_val> WORD

%token 	NOTOKEN GREAT GREAT2 NEWLINE LESS
%token PIPE
%token EXIT
%union	{
		char   *string_val;
	}

%{
extern "C" 
{
	int yylex();
	void yyerror (char const *s);
}
#define yylex yylex
#include <stdio.h>
#include "command.h"
%}

%%

goal:	
	commands
	;
	  
commands: 
	command
	| commands command 
	;

command: simple_command
        ;

simple_command:	
	command_and_args iomodifier_opt NEWLINE {
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	}
	| NEWLINE 
	| error NEWLINE { yyerrok; }
	;


command_and_args:
	command_word arg_list {
		printf("on top\n");
		Command::_currentCommand.insertSimpleCommand( Command::_currentSimpleCommand );
	}
	| command_and_args PIPE command_word arg_list {
       printf("Yacc: You inserted PIPE Operator \n");
       Command::_currentCommand.insertSimpleCommand( Command::_currentSimpleCommand ); // Insert the new simple command into the new command 
	}
	;
arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
               printf("   Yacc: insert argument \"%s\"\n", $1);
	       Command::_currentSimpleCommand->insertArgument( $1 );\
	}
	;
command_word:
	WORD {
            printf("   Yacc: insert command \"%s\"\n", $1);
	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	|EXIT{
	printf("exit was inserted\n");
	return 0;
	}
	;

iomodifier_opt:
	GREAT WORD {
		printf("   Yacc: insert output 3 \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
	}
	|GREAT2 WORD {
		printf("   Yacc: insert output 3 \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
	}
	|LESS WORD {
	printf("   Yacc: insert output 3 \"%s\"\n", $2);
		Command::_currentCommand._inputFile = $2;
	}
	| /* can be empty */ 
	;
	
%%

void
yyerror(const char * s)
{
	fprintf(stderr,"%s", s);
}

#if 0
main()
{
	yyparse();
}
#endif
