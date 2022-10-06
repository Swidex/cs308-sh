#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>
#include <assert.h>

char *prompt;

char** tokenize(char* a_str, const char a_delim)
{
	char** res			= 0;
	size_t cnt			= 0;
	char* tmp			= a_str;
	char* last_delim	= 0;
	char delim[2];
	delim[0] = a_delim;
	delim[1] = 0;
	
	// count number of arguments
	while (*tmp){
		if (a_delim == *tmp) {
			cnt++;
			last_delim = tmp;
		}
		tmp++;
	}

	cnt += last_delim < (a_str + strlen(a_str) - 1);
	cnt++;
	res = malloc(sizeof(char*) * cnt);

	if (res) {
		size_t idx	= 0;
		char* token = strtok(a_str, delim);

		while (token) {
			assert(idx < cnt);
			*(res + idx++) = strdup(token);
			token = strtok(0, delim);
		}
		assert(idx == cnt - 1);
		*(res + idx) = 0;
	}
	return res;
}

int processTokens(char** tokens)
{
	int status;
	int p = waitpid(-1, &status, WNOHANG);
	if (p > 0) printf("child(%d) exited with status %d\n",p, WEXITSTATUS(status));

	if (strcmp(*(tokens), "exit") == 0) {
		// terminate process
		return 1;

	} else if (strcmp(*(tokens), "pid") == 0) {
		// print process id
		printf("%d\n", getpid());

	} else if (strcmp(*(tokens), "ppid") == 0) {
		// print parent process id
		printf("%d\n", getppid());

	} else if (strcmp(*(tokens), "pwd") == 0) {
		// print current working directory
		char* buf;
		if ( ( buf = getcwd(NULL, 0)) == NULL )
		{
			perror("Failed to get current directory.\n");
		} else {
			printf("%s\n", buf);
			free(buf);
		}

	} else if (strcmp(*(tokens), "cd") == 0) {
		// change directory (defaults to $HOME if no arguments specified)
		if (*(tokens + 1) == NULL) {
			if (chdir(getenv("HOME")) != 0) perror("Failed to change to home path.\n");
		} else {
			if (chdir(*(tokens + 1)) != 0) perror("Failed to change path.\n");
		}

	} else if (strcmp(*(tokens), "") == 0 ) {
		// do nothing

	} else {
		/* Execute file */
		// set to background if flagged
		int stat;
		int i, vargc, bg = 0;
		for (vargc = 0; *(tokens+vargc); vargc++);
		if (strcmp(*(tokens+vargc-1),"&") == 0) bg = 1;

		int pid = fork();
		if (pid == 0) {
			/* Child Process */
			pid = getpid();
			
			/* Announce execution */
			printf("child(%d) executing (", pid);
			for (i = 0; *(tokens+i); i++) printf(" %s",*(tokens+i));
			if (bg) {
				printf(" ) in background.\n");
			} else { printf(" )\n"); }

			char *vargs[vargc+1];
			for (i = 0; i <= vargc; i++) {
				vargs[i] = *(tokens+i);
			}
			vargs[vargc] = NULL;
			if (strcmp(vargs[vargc - 1], "&") == 0) vargs[vargc - 1] = NULL;

			execvp(vargs[0], vargs);
			err(1, "execvp");
		} else {
			if (bg) {
				sleep(1);
				return 0;
			} else {
				waitpid(pid, &stat, 0);
				printf("child(%d) exited with status %d\n",pid, WEXITSTATUS(stat));
			}
		}
	}
	return 0;
}

int main (int argc, char* argv[])
{
	/* Set prompt flag */
	if (argc > 1 && strcmp(argv[1], "-p") == 0) {
		prompt = argv[2];
	} else {
		prompt = "308sh> ";
	}

	/* Main Loop */
	while (1)
	{
		printf("%s",prompt); // print prompt

		/* Receive user input from stdin */
		char **tokens;
		char *uinput;
		int bg = 0;
		fgets(uinput, 64, stdin);
		uinput[strcspn(uinput, "\n")] = 0; // clear newline
		
		tokens = tokenize(uinput, ' ');
		if (tokens) {
			int ex = processTokens(tokens);
			/* Free tokens from memory */
			int i;
			for (i = 0; *(tokens + i); i++) {
				free(*(tokens + i));
			}
			free(tokens);
			if (ex) break;
		}
	}

	return 0;
}

