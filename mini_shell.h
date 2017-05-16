#define NOT_FOUND -1
#define FOUND 1
#define _GNU_SOURCE             
#include <fcntl.h>              
#include <unistd.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/* job structure */
typedef struct job
{
    int pid;
    char *proc;
    struct job *next;
}jobs;

/* commands structure */
typedef struct node
{
    char *data;
    struct node *next;
}node;

int flag,status;
char *cmd_cpy;

jobs *job_head = NULL;
int CUR_PID;
int job_num;

/* path for mini shell */
char *PATH = "/home/emertxe/kewal/linux_internal/project/mini_shell";

void handler1(int arg)
{
    fflush(stdin);
    fflush(stdout);
    signal(SIGINT,SIG_IGN);
}

void handler2(int arg)
{
    printf("[%d]+           %s\n",job_num,cmd_cpy);
    insert_job(&job_head,CUR_PID,cmd_cpy);
    job_num++;
}

int commnad(char*);
int insert(node **head,char *data);
int print_list(node *head);
node *create_node(char *data);

int insert_job(jobs **head,int pid,char*);
int print_jobs(jobs *head);
jobs *create_job(int pid,char*);
int forgrnd(jobs **head);

