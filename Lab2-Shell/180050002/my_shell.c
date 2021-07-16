#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include<ctype.h>
#include<sys/wait.h>

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


int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;
    int status;
	FILE* fp;
	if(argc == 2) {
		fp = fopen(argv[1],"r");
		if(fp == NULL) {
			printf("File doesn't exists.\n");
			return -1;
		}
	}

	while(1) {			
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		if(argc == 2) { // batch mode
			wait(NULL);
			if(fgets(line, sizeof(line), fp) == NULL) { // file reading finished
				break;	
			}
			line[strlen(line) - 1] = '\0';
		} else { // interactive mode
			wait(NULL);
			printf("$ ");
			scanf("%[^\n]", line);
			getchar();
		}
		//printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */
// 		int rc = fork();
// 		if(rc==0){
 //        char *myargs[4];
	// myargs[0] = strdup("echo");
	// myargs[1] = strdup("hello");
	// myargs[2] = strdup("world");
	// myargs[3] = NULL;
 	// execvp(myargs[0],myargs);
// }
//else if(rc>0){
		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);

		if(tokens[0]==NULL){
			continue;
		}
		else{
       //do whatever you want with the commands, here we just print them

		// for(i=0;tokens[i]!=NULL;i++){
		// 	printf("found token %s (remove this debug output later)\n", tokens[i]);
		// }

        if(strcmp(tokens[0],"cd")!=0){
        int rc = fork();
        if(rc==0){
        if(execvp(tokens[0],tokens)<0){
        printf("command execution failed\n");
        for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);
        _exit(1);
        return 1;
    }
    }
    
}
else{
	int ret = chdir(tokens[1]);
    if(ret!=0){printf("Shell: Incorrect command\n");}
}

		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	//}
}
}
	return 0;
}
