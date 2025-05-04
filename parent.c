/* parent.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <locale.h>
#include <errno.h>
#include <ctype.h> 
#define MAX_ENV_VARS   128   // максимальное число переменных в env-файле
#define MAX_CHILD_NAME 16    // длина имени child_XX
#define MAX_LINE       256   // длина буфера чтения строк
extern char **environ;


// Запуск дочернего процесса
static void execChild(int num, const char *env_file, char **envp) {
    
    char *child_dir = getenv("CHILD_PATH");
    if (!child_dir) {
        fprintf(stderr, "CHILD_PATH environment variable not set\n");
        return;
    }

    char child_path[128];
    snprintf(child_path, sizeof(child_path), "%s/child", child_dir);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) { 
        char child_name[16];
        snprintf(child_name, sizeof(child_name), "child_%02d", num);
        
        char *argv[3] = {child_name, NULL, NULL}; 
        if (env_file) {
            argv[1] = (char *)env_file; 
        }
        execve(child_path, argv, envp);
        perror("execve failed");
        exit(EXIT_FAILURE);
    } 
    else { 
        wait(NULL);
    }
}

int main(int argc, char *argv[]) {
    // if (argc < 2) {
    //     fprintf(stderr, "Usage: %s <env_file>\n", argv[0]);
    //     exit(EXIT_FAILURE);
    // }


  
                
    int child_count = 0;
    printf("\nCommands:\n+ - new child with custom env\n* - new child with vars\n& - new child with parent env\nq - quit\n");

    int ch;
    while ((ch = getchar()) != 'q') {
        if (ch == '\n') continue;
        
        switch (ch) {
            case '+':
                execChild(child_count++, argv[1], NULL);
                break;
            case '*':
                execChild(child_count++, NULL, NULL);
                break;
            case '&':
                execChild(child_count++, NULL, environ);
                break;
            default:
                printf("Unknown command: %c\n", ch);
        }
    }

    
    return EXIT_SUCCESS;
}