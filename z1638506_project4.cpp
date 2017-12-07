/******************************************************************************
NAME    : Chris Skiles
ZID     : z1638506
ASSIGN  : 10/10/16
PURPOSE : create a mini shell within UNIX which can pipe output from one
		  command to another.
*******************************************************************************/

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <cstring>

using namespace std;

void executeCommand(char[]);
void executePipeCommand(char[]);

int main() {
	char command[1024];
	int pid, status;

	cout << "480shell: "; 		//prompt user for command
	cin.getline(command, 1024);	//get command
	while(strcmp(command, "quit") != 0 && strcmp(command, "q") != 0) {	//finish if user entered quit or q
		if ( (pid = fork()) < 0 ) {
			cerr << "fork error";
			exit(-37);
		}

		else if (pid == 0) {  //child process
			if (strstr(command, "||") == NULL)	//if not a pipe command, execute the command
				executeCommand(command);

			else								//else send to executePipeCommand for setup
				executePipeCommand(command);
		}
		
		else { //parent process
			waitpid(pid, &status, 0);	//wait for previous command to finish before looping
			
			cout << "480shell: ";		//prompt for new command
			cin.getline(command, 1024);
		}
	}
	
	return 0;
}


/*********************************************************************************
FUNCTION: executeCommand

PUPRPOSE: tokenize argument command and use exec to execute the command in UNIX
**********************************************************************************/
void executeCommand(char command[]) {
	int index = 1;
	char *args[30]; 					 //this shell allows a maximum of 29 arguments
	char *token = strtok(command, " ");	 //begin command tokenization
	args[0] = strdup(token);			 

	//tokenize string and store in args
	while ((token = strtok(NULL, " ")) != NULL) {
		args[index] = strdup(token);
		++index;
	}

	//set last element of args beyond meaningful elements to NULL
	args[index] = NULL;

	//execute command in UNIX
	execvp(args[0], args);

	//if everything goes right these lines are never run
	cerr << "command not executed" << endl;
	exit(-37);
}


/************************************************************************
FUNCTION: executePipeCommand

PURPOSE:  utilizing the executeCommand function and pipes, executes the 
		  command before the || substring in command and sends the output 
		  to the second command.
*************************************************************************/
void executePipeCommand(char command[]) {
	int pid;
	int pipefds[2];
	char *cmd1, *cmd2;
	
	//tokenize command by the || substring
	cmd1 = strtok(command, "||");
	cmd2 = strtok(NULL, "||");
	
	//create a pipe
	pipe(pipefds);
	
	
	if ((pid = fork()) < 0) {
		cerr << "Fork error in executePipeCommand" << endl;
		exit(-37);
	}
	
	if (pid == 0) {	//child process
		close(1);			//close stdout
		dup(pipefds[1]);	//replace with write side of pipe
		
		close(pipefds[0]);	//close both ends of the pipe
		close(pipefds[1]);
		
		executeCommand(cmd1);	//execute the first command
	}
	
	else { //parent process
		close(0);			//close stdin
		dup(pipefds[0]);	//replace with read side of pipe
		
		close(pipefds[0]);	//close both ends of the pipe
		close(pipefds[1]);
		
		executeCommand(cmd2);	//execute the second command
	}
}







