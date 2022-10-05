#include <stdio.h>
#include <string.h>

int main (int argc, char* argv[])
{
	/*
	Accept prompt option or
	set to default "308sh>"
	*/

	char *prompt;
	if (argc > 1 && strcmp(argv[1], "-p") == 0) {
		prompt = argv[2];
	} else {
		prompt = "308sh>";
	}

	/*
	Shell loop
	*/

	while (1)
	{
		//Accept user input
		char *uinput;
		printf("%s",prompt); // print prompt
		fgets(uinput, 64, stdin);

		//Process user input
		if (strcmp(uinput, "exit\n") == 0) {
			printf("Exiting 308sh.\n");
			return 1;
		} else if (strcmp(uinput, "pid\n") == 0) {
			printf("%d\n", getpid());
		} else {
			printf("Unknown command.\n");
		}
	}

	return 1;
}

