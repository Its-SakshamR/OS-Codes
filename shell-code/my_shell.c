#include  <stdio.h>
#include  <signal.h>
#include  <sys/types.h>
#include  <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

pid_t foregroundProcess = -1;

// Signal handler for Ctrl+C (SIGINT)
void sigintHandler(int signo) {
    if (foregroundProcess > 0) {
        // Send Ctrl+C to the foreground process
        kill(foregroundProcess, SIGINT);
        foregroundProcess = -1; // Reset the foreground process ID
    }
}

int getTokensLength(char **tokens) {
    // Iterate through the array until a NULL pointer is encountered
    int length = 0;
    while (tokens[length] != NULL) {
        length++;
    }
    return length;
}


int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;

	while(1){
		signal(SIGINT,sigintHandler);
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();

		//printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */
		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);

		if(tokens[0] && !(strcmp(tokens[0],"exit"))){
			kill(-9,-1);
			for(i=0;tokens[i]!=NULL;i++){
				free(tokens[i]);
			}
			free(tokens);
			return 0;
		}

		for(int i = 0 ; i < 64 ; i = i + 1){
			int ruk_ja = waitpid(-1,NULL,WNOHANG);
			if(ruk_ja>0){
				printf("Shell: Backgroound Process Finished\n");
			}
		}
   
       //do whatever you want with the commands, here we just print them
	    int status;
		int len = getTokensLength(tokens);
		if(tokens[0]){
			pid_t rc = fork();
			if(!rc){
				//printf("%s\n", tokens[len-1]);
				setpgid(0,0);
				if(strcmp(tokens[0],"cd")){
					if(!strcmp(tokens[len-1],"&")){
						tokens[len-1] = '\0';
					}
					int co = 1; // Count of number of subprocesses created

					int prev = 0; // Index of the first token after the last seen && or &&&

					for (int i = 0; i < MAX_NUM_TOKENS; i++)
					{

						if (tokens[i] == NULL)
						{

							break;
						}

						if (!strcmp(tokens[i], "&&"))
						{

							tokens[i] = NULL; // Null terminate the array, effectively splitting it
							if (fork() == 0)
							{

								execvp(*(tokens + prev), tokens + prev); // Exec statement
								printf("Shell: Incorrect Command : %s\n", *(tokens + prev));
								exit(0);
							}
							else
							{

								wait(NULL);   // Since sequential, we are waiting
								prev = i + 1; // Update the value of prev
							}
							continue;
						}

						if (!strcmp(tokens[i], "&&&"))
						{

							// Similar to &&, except we increment the number of processes since we will
							// wait only after all processes are created.
							tokens[i] = NULL;
							co++;
							if (fork() == 0)
							{

								execvp(*(tokens + prev), tokens + prev);
								printf("Shell: Incorrect Command : %s\n", *(tokens + prev));
								exit(0);
							}
							else
							{

								prev = i + 1;
							}
							continue;
						}
					}
					// Executing the final argument (since it does not end with a && or &&&)
					if (fork() == 0)
					{
						execvp(*(tokens + prev), tokens + prev);
						// Fork copies the heap memory as well, so this is memory safe
						printf("Shell: Incorrect Command : %s\n", tokens[0]);
						exit(0);
					}
					else
					{

						for (int i = 0; i < co; i++)
						{

							wait(NULL);
						}
						// Must have as many wait statements as the processes we created
						// For && we were reaping instantly, for &&& we incremented the co counter
					}
						// execvp(tokens[0],tokens);
						// printf("Error: No command named as %s\n", tokens[0]);
					}

				exit(EXIT_SUCCESS);
			}

			else if(rc>0){
				if(strcmp(tokens[len-1],"&")){
					foregroundProcess = rc;
					int t20_wc = wait(NULL);
					foregroundProcess = -1;
				}
				// if(!x){
				// 	kill(getpid(),9);
				// }

				//int t20_wc = wait(NULL);

				if(!strcmp(tokens[0],"cd")){
					int x = chdir(tokens[1]);

					if(x){
						printf("Shell: Incorrect Command\n");
					}
				}
			}

		}
       
		// Freeing the allocated memory	

		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

		usleep(10000);

	}
	return 0;
}
