/* parent.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_CHILDREN 100

typedef struct {
    pid_t pid;
    int number;
    char name[16];
} ChildInfo;

static ChildInfo children[MAX_CHILDREN];
static int child_count = 0;

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
        snprintf(children[child_count].name, sizeof(children[child_count].name), "child_%02d", num);
        char *argv[3] = {children[child_count].name, NULL, NULL}; 
        if (env_file) {
            argv[1] = (char *)env_file; 
        }
        
        execve(child_path, argv, envp);
        perror("execve failed");
        exit(EXIT_FAILURE);
    } 
    else { 
        if (child_count < MAX_CHILDREN) {
            children[child_count].pid = pid;
            children[child_count].number = num;
            child_count++;
            printf("Created child %s (PID: %d)\n", children[child_count-1].name, pid);
        } else {
            fprintf(stderr, "Maximum children limit reached\n");
        }
    }
}

static void killAllChildren() {
    printf("\nKilling all children:\n");
    for (int i = 0; i < child_count; i++) {
        printf("Killing %s (PID: %d)\n", children[i].name, children[i].pid);
        kill(children[i].pid, SIGTERM);
        waitpid(children[i].pid, NULL, 0);
    }
    child_count = 0;
}

static void printChildrenList() {
    printf("\nActive children (%d):\n", child_count);
    for (int i = 0; i < child_count; i++) {
        printf("%s (PID: %d)\n", children[i].name, children[i].pid);
    }
}


int main(int argc, char *argv[]) {
    printf("\nCommands:\n"
           "+ - new child\n"
           "- - kill last child\n"
           "l - list active children\n"
           "k - kill all children\n"
           "q - quit\n");

    int ch;
    int child_number = 0;
    while ((ch = getchar()) != 'q') {
        if (ch == '\n') continue;
        
        switch (ch) {
            case '+':
                execChild(child_number++, argv[1], NULL);
                break;
            case '-':
                if (child_count > 0) {
                    pid_t pid = children[--child_count].pid;
                    printf("Killing %s (PID: %d)\n", children[child_count].name, pid);
                    kill(pid, SIGTERM);
                    waitpid(pid, NULL, 0);
                }
                break;
            case 'l':
                printChildrenList();
                break;
            case 'k':
                killAllChildren();
                child_number = 0;
                break;
            default:
                printf("Unknown command: %c\n", ch);
        }
    }

    killAllChildren();
    return EXIT_SUCCESS;
}