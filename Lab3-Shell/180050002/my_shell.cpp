#include  <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
*
*/

pid_t parent;
bool ign;//background;

void sigquit_handler (int sig) {
    assert(sig == SIGQUIT);
    pid_t self = getpid();
    if (parent != self) _exit(0);
}

void sigint_handler (int sig) {
    assert(sig == SIGINT);
    ign = true;
    //background = true;
    killpg(-1*parent, SIGQUIT);

}

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

vector<char**> split(const string& line, string delim)
{
   vector<char**> command_tokens;
   char** tokens;
		string s = line;
    auto start = 0U;
    auto end = s.find(delim);
    while (end != string::npos)
    {
    	string s1 = s.substr(start, end - start);
    	int n1 = s1.length();
    	char char_arr[n1+1];
    	strcpy(char_arr, s1.c_str());
    	tokens = tokenize(char_arr);
    	command_tokens.push_back(tokens);
        //cout << s.substr(start, end - start) << endl;
        start = end + delim.length();
        end = s.find(delim, start);
    }
string s1 = s.substr(start, end);
    	int n1 = s1.length();
    	char char_arr[n1+1];
    	strcpy(char_arr, s1.c_str());
    	tokens = tokenize(char_arr);
    	command_tokens.push_back(tokens);
    	return command_tokens;
    }


int main(int argc, char* argv[]) {
	//signal(SIGCHLD, SIG_IGN);
	signal(SIGQUIT, sigquit_handler);
	signal(SIGINT, sigint_handler);
	parent = getpid();
	pid_t background_children_pgid = 0;
	pid_t chld;
    //background = false;
    //vector<pid_t> background_chld(64,NULL);//change
    int background_completed = 0;
	char  line[MAX_INPUT_SIZE]; 
	char  **tokens;              
	int i;
    int status;
	FILE* fp;
	if(argc == 2) {
		fp = fopen(argv[1],"r");
		if(fp == NULL) {
			printf("File doesn't exists.");
			return -1;
		}

	}

	while(1) {
		
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		if(argc == 2) { // batch mode
			int temp = background_completed;
			for(int k=0;k<temp;k++){
	        if(waitpid(-1,NULL,WNOHANG) > 0 && background_completed>0){
	        	cout<<"Shell: Background process finished"<<endl;
	        	background_completed--;
	        }
	    }
			if(fgets(line, sizeof(line), fp) == NULL) { // file reading finished
				break;	
			}
			line[strlen(line) - 1] = '\0';
		
		} else { // interactive mode
			//while (wait(NULL) > 0);
			int temp = background_completed;
			for(int k=0;k<temp;k++){
	        if(waitpid(-1,NULL,WNOHANG) > 0 && background_completed>0){
	        	cout<<"Shell: Background process finished"<<endl;
	        	background_completed--;
	        }
	    }
			printf("$ ");
			scanf("%[^\n]", line);
			getchar();
		
		}
		ign = false;
		//background = false;
		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);

		if(tokens[0]==NULL){
			continue;
		}
		else{

		vector<char**> command_tokens;
		bool b1=false,b2=false,b3=false;
		for(int x=0;tokens[x]!=NULL;x++){
			if(strcmp(tokens[x],"&&&")==0){
				//cout<<"hi";
				b1=true;
				command_tokens = split(line,"&&&");
				break;
			}
			else if(strcmp(tokens[x],"&&")==0){
				b2=true;
				command_tokens = split(line,"&&");
				break;
			}
			else if(strcmp(tokens[x],"&")==0){
				b3 = true;
				background_completed++;
				command_tokens = split(line,"&");
				break;
			}
		}

        if(strcmp(tokens[0],"cd")!=0&&strcmp(tokens[0],"exit")!=0&&!b2&&!b1){
        chld = fork();
        if(chld==0){
        	if(!b3){
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
    else{
    	//cout<<getpid()<<endl;
        if(execvp(command_tokens[0][0],command_tokens[0])<0){
        printf("command execution failed\n");
        for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);
        _exit(1);
    }
    } 
    }
    else{
    	if(b3){
    // 		for(int k=0;k<64;k++){
    // 			if(background_chld[k]==NULL){
    // 		background_chld[k]=chld;//change
    // 		break;
    // 	}
    // }

    	if(background_children_pgid==0){
    		background_children_pgid = chld;
    	}
    	setpgid(chld,background_children_pgid);
    	if(getpgid(chld)==parent){
    		background_children_pgid = chld;
    	}
    	setpgid(chld,background_children_pgid);
        //cout<<chld<<" "<<getpgid(chld)<<" "<<background_children_pgid<<endl;
    	}
    	else{
    		//cout<<"waiting\n";
    		waitpid(chld,NULL,0);
    	}
    }
    
}
else if(strcmp(tokens[0],"exit")==0){
	for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);
	pid_t pid = getpid();
	kill(-1*background_children_pgid,SIGINT);
	kill(-1*pid,SIGINT);
			for(int k=0;k<64;k++){
	        if(waitpid(-1,NULL,0) > 0){
	        	cout<<"Shell: Background process finished"<<endl;
	        }
	    }
	command_tokens.resize(0);
	command_tokens.shrink_to_fit();
	_exit(0);
}
else if(strcmp(tokens[0],"cd")==0){
	int ret = chdir(tokens[1]);
    if(ret!=0){printf("Shell: Incorrect command\n");}
}
// if(b3){
// 	background = true;
// }
if(b1){
	vector<pid_t> children;
	for (int l=0; l<command_tokens.size(); l++) {
		if(strcmp(command_tokens[l][0],"cd")==0){
	int ret = chdir(command_tokens[l][1]);
    if(ret!=0){printf("Shell: Incorrect command\n");}
}

else{
   pid_t slave = fork();
   children.push_back(slave);
   if (slave==0) /* only execute this if child */
   {
     if(execvp(command_tokens[l][0],command_tokens[l])<0){
        printf("command execution failed\n");
        for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);
        _exit(1);
    }
   }
  }
 }

 for(int l=0;l<command_tokens.size();l++){
 	waitpid(children[l],NULL,0);
 }
}

if(b2){
for(int l=0; l<command_tokens.size()&&!ign; l++){
if(strcmp(command_tokens[l][0],"cd")==0){
	int ret = chdir(command_tokens[l][1]);
    if(ret!=0){printf("Shell: Incorrect command\n");}
}

else{
    pid_t slave = fork();
    if (slave==0) /* only execute this if child */
        {
        if(execvp(command_tokens[l][0],command_tokens[l])<0){
        printf("command execution failed\n");
        for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);
        _exit(1);
    }  
        }
        waitpid(slave,NULL,0);
  }  /* only the parent waits */
 }
}

// if(!background){
// 	cout<<"waiting...\n";
// 		waitpid(chld, NULL, 0);
// }
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

