
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "command.h"
#include <ostream>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>

void childTerminated(pid_t pid)
{
	auto now = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(now);

	struct tm *timeinfo = localtime(&end_time);

	char buffer[80];
	strftime(buffer, 80, "%d-%m-%Y %H:%M", timeinfo);
	std::ofstream logfile("logfile.log", std::ios_base::app);

	logfile << buffer<< " Child process " << pid << " terminated" << std::endl;
	logfile.close();
}

SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **)malloc(_numberOfAvailableArguments * sizeof(char *));
}

void SimpleCommand::insertArgument(char *argument)
{
	if (_numberOfAvailableArguments == _numberOfArguments + 1)
	{
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **)realloc(_arguments, _numberOfAvailableArguments * sizeof(char *));
	}

	_arguments[_numberOfArguments] = argument;

	//printf("inserting new argument in coomand.c file: %s\n", _arguments[_numberOfArguments]);

	// Add NULL argument at the end
	_arguments[_numberOfArguments + 1] = NULL;

	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc(_numberOfSimpleCommands * sizeof(SimpleCommand *));

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void Command::insertSimpleCommand(SimpleCommand *simpleCommand)
{
	if (_numberOfAvailableSimpleCommands == _numberOfSimpleCommands)
	{
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **)realloc(_simpleCommands,
													_numberOfAvailableSimpleCommands * sizeof(SimpleCommand *));
	}
	_simpleCommands[_numberOfSimpleCommands] = simpleCommand;
	_numberOfSimpleCommands++;
}

void Command::clear()
{

	//printf("in clear\n");
	for (int i = 0; i < _numberOfSimpleCommands; i++)
	{
		for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++)
		{
			free(_simpleCommands[i]->_arguments[j]);
		}

		free(_simpleCommands[i]->_arguments);
		free(_simpleCommands[i]);
	}

	if (_outFile)
	{
		free(_outFile);
	}

	if (_inputFile)
	{
		free(_inputFile);
	}

	if (_errFile)
	{
		free(_errFile);
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");

	for (int i = 0; i < _numberOfSimpleCommands; i++)
	{
		printf("  %-3d ", i);
		for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++)
		{
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[j]);
		}
	}

	printf("\n\n");
	printf("  Output       Input        Error        Background\n");
	printf("  ------------ ------------ ------------ ------------\n");
	printf("  %-12s %-12s %-12s %-12s\n", _outFile ? _outFile : "default",
		   _inputFile ? _inputFile : "default", _errFile ? _errFile : "default",
		   _background ? "YES" : "NO");
	printf("\n\n");
}

void Command::handlePipes()
{
	int defaultin = dup(0);
	int defaultout = dup(1);
	int previousPipe[2];
	pid_t lastChild;

	for (int i = 0; i < _numberOfSimpleCommands; i++)
	{
		int currentPipe[2];
		if (pipe(currentPipe) == -1)
		{
			perror("pipe creation error");
			exit(2);
		}
		pid_t pid = fork();
		if (pid == -1)
		{
			perror("fork error");
			exit(2);
		}
		else if (pid == 0)
		{
			if (i == 0)
			{ // first Pipe ->default input , put output in current pipe
				dup2(defaultin, 0);
				close(currentPipe[0]);
				dup2(currentPipe[1], 1);
				close(defaultin);
			}

			else if (i < _numberOfSimpleCommands - 1)
			{ // mid pipes-> input from previous Pipe, output in current Pipe
				close(currentPipe[0]);
				close(previousPipe[1]);
				dup2(previousPipe[0], 0);
				dup2(currentPipe[1], 1);
				close(currentPipe[1]);
				close(previousPipe[0]);
			}
			else
			{ // Last pipe -> input from previous Pipe, output to terminal or file
				lastChild = pid;
				close(currentPipe[0]);
				close(currentPipe[1]);
				close(previousPipe[1]);
				dup2(previousPipe[0], 0);
				dup2(defaultout, 1);
				close(defaultout);
			}
			char path[20] = "/bin/";
			strcat(path, _simpleCommands[i]->_arguments[0]);
			execvp(path, _simpleCommands[i]->_arguments);
			perror("Command Failed-->no command with this name\n");
			exit(2);
		}
		else
		{
			if (i > 0)
			{
				close(previousPipe[0]);
				close(previousPipe[1]);
			}
			previousPipe[0] = currentPipe[0];
			previousPipe[1] = currentPipe[1];
			if (i == _numberOfSimpleCommands - 1)
			{
				close(currentPipe[0]);
				close(currentPipe[1]);
				dup2(defaultin, 0);
				dup2(defaultout, 1);
				close(defaultin);
				close(defaultout);
			}
			// Wait only for the last child process
			waitpid(lastChild, NULL, 0);
			childTerminated(pid);
			//logChildTermination(logFile, lastChild);
		}
	}
}

void Command::execute()
{
	
	// Don't do anything if there are no simple commands
	if (_numberOfSimpleCommands == 0)
	{
		prompt();
		return;
	}
	// There is Pipes
	else if (_numberOfSimpleCommands == 1)
	{	
		pid_t pid;
		pid = fork();
		if (pid < 0)
		{
			fprintf(stderr, "Fork Failed");
			return;
		}

		else if (pid == 0)
		{
			char path[20] = "/bin/";
			strcat(path, _simpleCommands[0]->_arguments[0]);
			execvp(path, _simpleCommands[0]->_arguments);
			perror("Command Failed-->no command with named %s\n");
			exit(2);
		}

		else
		{
		 
			wait(NULL);
			childTerminated(pid);
			//waitpid( pid, 0, 0 );
			
		}
	}
	else
	{
		this->handlePipes();
	}
	// Clear to prepare for next command
	clear();
	// Print new prompt
	prompt();
	//}
}

// Shell implementation

void Command::prompt()
{
	printf("myshell>");
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand *Command::_currentSimpleCommand;

int yyparse(void);

int main()
{
	signal(SIGINT, SIG_IGN);
		
	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}
