
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
%token	<string_val> ARG
%token	<string_val> CD
%token 	NOTOKEN GREAT GREAT2 NEWLINE LESS AND  
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
#include <unistd.h>
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
		//printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	}
	| NEWLINE 
	| command_and_args AND NEWLINE{
		printf("& was inserted\n");
		printf("Yacc: Execute command\n");
		Command::_currentCommand.execute();
	}
	| command_and_args error NEWLINE{ 
		Command::_currentCommand.clear();
		yyerrok; 
	}
	| CD WORD NEWLINE{
		char ss[100];
		printf("your directory was %s\n",getcwd(ss,100));
		chdir($2);
		printf("current directory is %s\n",getcwd(ss,100));
		Command::_currentCommand.prompt();
	}
	| CD NEWLINE{
	char ss[100];
		printf("your directory was %s\n",getcwd(ss,100));
		chdir("/home");
		printf("current directory is %s\n",getcwd(ss,100));
		Command::_currentCommand.prompt();
	
	}
	|EXIT{
	printf("Bye\n");
	return 0;
	}
	| error NEWLINE {
	Command::_currentCommand.clear();
	 yyerrok;}
	;

command_and_args:
	command_word arg_list {

		Command::_currentCommand.insertSimpleCommand( Command::_currentSimpleCommand );
	}
	| command_and_args PIPE command_word arg_list {
       //printf("Yacc: You inserted PIPE Operator \n");
       Command::_currentCommand.insertSimpleCommand( Command::_currentSimpleCommand ); // Insert the new simple command into the new command 
	}
	;
arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
              // printf("   Yacc: insert argument \"%s\"\n", $1);
	       Command::_currentSimpleCommand->insertArgument( $1 );\
	}
	|ARG{
		//printf("   Yacc: insert argument \"%s\"\n", $1);
	       Command::_currentSimpleCommand->insertArgument( $1 );\
	}
	//|
	;
command_word:
	WORD {
           //printf("   Yacc: insert command \"%s\"\n", $1);
	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;

iomodifier_opt:
	GREAT WORD {
		//printf("   Yacc: insert output 3 \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
	}
	|GREAT2 WORD {
	//	printf("   Yacc: insert output 3 \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentSimpleCommand->_append=true;
		;
	}
	|LESS WORD {
	//printf("   Yacc: insert input 3 \"%s\"\n", $2);
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
