#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "colors.h"
#include "header.h"

#define MAX_NAME_SIZE 100
#define MAX_DIR_SIZE 100
#define MAX_COMMAND_SIZE 200


char* host_name;
char* user_name;
char* home_dir;
char command[MAX_COMMAND_SIZE];
char* curr_dir;
char* prev_dir;

char* getHostname()
{
    char* host = (char*)malloc((MAX_NAME_SIZE+1)*sizeof(char));
    gethostname(host,MAX_NAME_SIZE); //syscall
    return host;
}

char* getUsername()
{
    char *user = (char*)malloc((MAX_NAME_SIZE+1)*sizeof(char));
    getlogin_r(user,MAX_NAME_SIZE);
    return user;
}

char* getCurrentDirectory()
{
    char* home_dir = (char*)malloc((MAX_DIR_SIZE+1)*sizeof(char));
    home_dir=get_current_dir_name();
    return home_dir;
}

void initializeShell()
{
    printf("%s",CLR);
    host_name = getHostname();
    user_name = getUsername();
    home_dir = getCurrentDirectory();
    curr_dir = (char*)malloc((MAX_DIR_SIZE+1)*sizeof(char));
    prev_dir = (char*)malloc((MAX_DIR_SIZE+1)*sizeof(char));
    strcpy(curr_dir,home_dir);
}

void printPrompt()
{
    
    printf("%s",FBLD);
    printf("%s%s@%s%s:%s",KGRN,user_name,host_name,KWHT,KBLU);
    if(strlen(curr_dir)==0)
    {
        printf("/");
    }
    else if (strncmp(curr_dir, home_dir, strlen(home_dir)) == 0)
    {
        printf("~%s", curr_dir + strlen(home_dir));
    }
    else
    {
        printf("%s",curr_dir);
    }

    printf("%s",RESET);
    printf("%s> ",KWHT);
}

int isPrefix(const char *a, const char *b)
{
    int lenA = strlen(a);
    int lenB = strlen(b);

    if (lenA > lenB) {
        return -1;
    }
    if (strncmp(a, b, lenA) == 0){
        return lenA;
    }
    else{
        return -1;
    }
}

int countArgs(char *str)
{
    int count = 0;
    bool inWord = false;

    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n') {
            if (inWord)
            {
                ++count; 
                inWord = false;
            }
        }
        else
        {
            inWord = true;
        }
    }
    if (inWord) {
        ++count;
    }
    return count;
}

int directoryExists(char* path)
{
    DIR* dir = opendir(path);
    if (dir)
    {
        return 1;
        closedir(dir);
    }
    else if (ENOENT == errno)
    {
        return 0;
    }
    else
    {
        return -1;
    }

}

void echo(char* args)
{
    if(args==NULL)
    {
        return;
    }
    printf("%s",KWHT);

    int n = strlen(args);
    short quotes = 0;
    
    for(int i=0;i<n;i++)
    {
        if(args[i]=='\"'){
            quotes = 1-quotes;
            continue;
        }

        if(args[i]=='\\' && i!=n-1)
        {
            if (args[i+1]=='n'){
                printf("\n");
                i++;
                continue;
            }
            else if (args[i+1]=='t'){
                printf("\t");
                i++;
                continue;
            }
        }

        if(quotes){
            printf("%c",args[i]);
        }
        else{
            if(args[i]==' ' || args[i]=='\t')
            {
                printf(" ");
                int j = i+1;
                while(j<n && (args[j]==' ' || args[j]=='\t'))
                    j++;
                i=j-1;
            }
            else{
                printf("%c",args[i]);
            }
        }
    }
    printf("\n");
}

void pwd()
{
    if(strlen(curr_dir)==0)
    {
        printf("/\n");
        return;
    }
    printf("%s\n",curr_dir);
}

void cd(char* args)
{
    char slash[] = "/";
    if(args==NULL){
        strcpy(prev_dir,curr_dir);
        strcpy(curr_dir,home_dir);
        return;
    }
    if(countArgs(args)>1){
        printf("cd: too many arguments.\n");
        return;
    }

    if (strcmp(args,"-")==0)
    {
        char* temp = (char*)malloc((MAX_DIR_SIZE+1)*sizeof(char));
        printf("%s\n",prev_dir);
        strcpy(temp, curr_dir);
        strcpy(curr_dir,prev_dir);
        strcpy(prev_dir,temp);
        return;
    }
    else if(strcmp(args,".")==0)
    {
        return;
    }
    else if (strcmp(args,"..")==0)
    {
        strcpy(prev_dir,curr_dir);
        if (strcmp(curr_dir,slash)!=0)
        {
            char *lastSlash = strrchr(curr_dir, '/');
            if (lastSlash != NULL)
            {
                *lastSlash = '\0';
            }
        }
        return;
    }
    else if (strcmp(args,"~")==0)
    {
        strcpy(prev_dir,curr_dir);
        strcpy(curr_dir,"");
        return;
    }
    char path[MAX_DIR_SIZE];
    strcpy(path, curr_dir); 
    strcat(path,slash);
    strcat(path, args); 
    if(directoryExists(path))
    {
        strcpy(prev_dir,curr_dir);
        strcpy(curr_dir,path);
    }
    else
    {
        printf("%s : No such directory\n",path);
    }
}

char* getLastCommand()
{
    FILE* fp = fopen("./history.txt","r");
    if(fp==NULL)
    {
        perror("Error opening File!\n");
        return NULL;
    }

    char line[256];
    char* lastLine = (char*)malloc(sizeof(char)*(MAX_COMMAND_SIZE+1));

    while (fgets(line, sizeof(line), fp) != NULL) {
        strcpy(lastLine, line);
    }

    fclose(fp);
    return lastLine;
}

void writeHistory(char* command)
{   

    if(strcmp(getLastCommand(),command)==0)
    {
        return;
    }
    FILE* fp = fopen("./history.txt", "r");
    if (fp == NULL) {
        perror("Error Opening File!\n");
        return;
    }
    
    int count_line = 0;
    int ch;
    
    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n') {
            count_line++;
        }
    }
    fclose(fp);
    if(count_line<20)
    {
        fp = fopen("./history.txt","a");
        if(fp==NULL)
        {
            perror("Error opening file!\n");
            return;
        }
        fprintf(fp,"%s",command);
        fclose(fp);
    }
    else
    {
        FILE* fp = fopen("./history.txt","r");
        if(fp==NULL)
        {
            perror("Error Opening File!\n");
            return;
        }
        char line[1000]; 
        char *commands[19]; 
        int i = 0;

        while (fgets(line, sizeof(line), fp) != NULL) {
            if (i > 0)
            {
                commands[i - 1] = malloc(strlen(line) + 1);
                strcpy(commands[i - 1], line);
            }
            i++;
        }
        fclose(fp);
        fp = fopen("./history.txt","w");
        if(fp==NULL){
            perror("Error opening file!\n");
            return;
        }
        for(int x=0;x<19;x++)
        {
            fprintf(fp,"%s",commands[x]);
        }
        fprintf(fp,"%s",command);
        fclose(fp);
    }

}

void printHistory()
{
    FILE* fp = fopen("./history.txt","r");
    if(fp==NULL)
    {
        perror("Cannot open file : ERROR!\n");
            return;
    }

    int count = 0;
    char line[MAX_COMMAND_SIZE];
    char command_history[20][MAX_COMMAND_SIZE];

    while (fgets(line, sizeof(line), fp) != NULL && count < 20)
    {
        line[strcspn(line, "\n")] = '\0';
        strcpy(command_history[count], line);
        count++;
    }

    fclose(fp);
    int n=count;
    if(n<=10)
    {
        for (int i=0;i<n;i++)
        {
            printf("%s\n",command_history[i]);
        }
    }
    else
    {
        for(int i=n-10;i<n;i++)
        {
            printf("%s\n",command_history[i]);
        }
    }

}


int main()
{
    initializeShell();
    while(1)
    {
        printPrompt();
        fgets(command, MAX_COMMAND_SIZE, stdin);
        char *cmd;
        char *args;
        writeHistory(command);

        cmd = strtok(command, " \n");
        if (cmd==NULL)
            continue;
        args = strtok(NULL, "\n");
                
        if(strcmp(cmd,"echo")==0){
            echo(args);
        }
        else if (strcmp(cmd,"pwd")==0){
            pwd();
        }
        else if (strcmp(cmd,"cd")==0){
            cd(args);   
        }
        else if (strcmp(cmd,"history")==0){
            printHistory();
        }
        else if (strcmp(cmd,"exit")==0){
            break;
        }
        else
        {
            printf("no such command.\n");
        }
    }
    return 0;
}
