#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wordexp.h>

#define MAX_ARGS 10

int main (int argc, char* argv[])
{
	/*
	Accept prompt option or
	set to default "308sh> "
	*/

	char *prompt;
	if (argc > 1 && strcmp(argv[1], "-p") == 0) {
		prompt = argv[2];
	} else {
		prompt = "308sh> ";
	}

	/*
	Input loop
	*/
	char *cmd;

	while (1)
	{
		cmd = NULL;
		// Accept user input
		char *uinput;
		printf("%s",prompt); // print prompt
		fgets(uinput, 64, stdin);

		// Parse command and arguments
		if (uinput[0] != '\n') // don't cut if empty line
			uinput[strcspn(uinput, "\n")] = 0; // remove trailing newline for ease of parsing.
		
		cmd = strtok(uinput, " ");

		// Process user input
		if (strcmp(cmd, "exit") == 0) {
			// terminate process
			return 0;

		} else if (strcmp(cmd, "pid") == 0) {
			// print process id
			printf("%d\n", getpid());

		} else if (strcmp(cmd, "ppid") == 0) {
			// print parent process id
			printf("%d\n", getppid());

		} else if (strcmp(cmd, "pwd") == 0) {
			// print current working directory
			char* buf;
			if ( ( buf = getcwd(NULL, 0)) == NULL )
			{
				perror("Failed to get current directory.\n");
			} else {
				printf("%s\n", buf);
				free(buf);
			}

		} else if (strcmp(cmd, "cd") == 0) {
			// change directory (defaults to $HOME if no arguments specified)
			cmd = strtok(NULL, " "); // grab next argument
			if (cmd == NULL) {
				if (chdir(getenv("HOME")) != 0) perror("Failed to change to home path.\n");
			} else {
				if (chdir(cmd) != 0) perror("Failed to change path.\n");
			}

		} else if (strcmp(cmd, "\n") == 0 ) {
			// do nothing

		} else {
			// execute file
			if (cmd == NULL) {
				perror("No path specified.\n");
				continue;
			}

			// seperate arguments
			int i = 0;
			int stat;
			char *arg_list[MAX_ARGS];

			while (uinput != NULL) {
				arg_list[i++] = uinput;
				uinput = strtok(NULL," ");
			}
			arg_list[i++] = NULL;

			int pid = fork();
			if (pid == 0) {
				execvp(arg_list[0], arg_list);
				err(1, "execvp");
			} else {
				waitpid(pid, &stat, 0);
				fprintf(stderr, "child(%d) exited with %d\n",pid, WEXITSTATUS(stat));
			}
		}
	}

	return 0;
}

