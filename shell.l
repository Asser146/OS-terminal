
/*
 *
 * CS-413 Spring 98
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include "y.tab.h"
%}

%%
"|"	{
		return PIPE;	
	}

"exit" 	{
	return EXIT;
}

\n 	{
		return NEWLINE;
	}

[ \t] 	{
		/* Discard spaces and tabs */
     	}	
">>" 	{
		return GREAT2;
	}

">" 	{
		return GREAT;
	}
	
"<" 	{
		return LESS;
	}
	
"&"	{
		return AND;
	}

-[a-zA-Z0-9]+ {	printf("HELLOO");
			yylval.string_val = strdup(yytext);
			return ARG;
			
		}
[^ \t\n\>\<\|\&\-]+ {
		/* Assume that file names have only alpha chars */
		
		yylval.string_val = strdup(yytext);
		
		return WORD;
	}


'.'	{
		/* Invalid character in input */
		return NOTOKEN;
	}

%%
	
