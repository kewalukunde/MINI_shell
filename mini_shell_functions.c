#include <stdlib.h>
#include <stdio.h>
#include "mini_shell.h"

/* function to insert job in jobs */
int insert_job(jobs **head,int pid,char* name)
{
    jobs *ptr,*new;
    new = create_job(pid,name);

    if (*head == NULL)
        *head = new;
    else
    {
        ptr = *head;
        while (ptr->next != NULL)
            ptr = ptr->next;
        ptr->next = new;
    }
}

/* create job node */
jobs *create_job(int pid,char* str)
{
    jobs *new = calloc(1,sizeof(jobs));
    if (new != NULL)
    {
        new->pid = pid;
        new->proc = calloc(30,sizeof(char));
        strcpy(new->proc,str);
        new->next = NULL;
    }
    return new;
}

/* print jobs */
int print_jobs(jobs *head)
{
    jobs *ptr;
    int i=1;

    if (head == NULL)
        return NOT_FOUND;
    ptr = head;
    while(ptr != NULL)
    {
        printf("[%d]            %s\n",ptr->pid,ptr->proc);
        ptr = ptr->next;
        i++;
    }
}

int insert(node **head,char *data)
{
    node *ptr,*new;
    new = create_node(data);

    if (*head == NULL)
        *head = new;
    else
    {
        ptr = *head;
        while (ptr->next != NULL)
            ptr = ptr->next;
        ptr->next = new;
    }
}

node *create_node(char* data)
{
    node *new = calloc(1,sizeof(node));
    if (new != NULL)
    {
        new->data = calloc(1,sizeof(data) + 1);
        strcpy(new->data, data);
        new->next = NULL;
    }
    return new;
}


int print_list(node *head)
{
    node *ptr;
    int i=1;

    if (head == NULL)
        return NOT_FOUND;
    ptr = head;
    while(ptr != NULL)
    {
        printf("[%d] = %s\n",i,ptr->data);
        ptr = ptr->next;
        i++;
    }
}

/* taking change directory commands */
int change_dir(char *str)
{
    char *tmp = (char*)get_current_dir_name();
    char *path = calloc(50,sizeof(char));
    strcpy(path,tmp);
    char *tok;

    /* default for cd */
    if (!strcmp(str,"cd")) 
        chdir("/home/emertxe");
    /* for previous directory */
    else if (!strncmp(str,"cd ..",5))
    {
        tok = rindex(path,'/');
        *tok = '\0';
        chdir(path);
    }
    /* nest directory */
    else
    {
        tok = strchr(str,' ');
        tok[0] = '/';
        strcat(path,tok);
        chdir(path);
    }
}

int command(char *cmd)
{
    node *ptr2,*ptr,*head = NULL;
    int pid,fd1[2],fd2[2],i = 0, j = 0;
    char *buf = calloc(20,sizeof(char));

    int sw_p = 0;

    /* loop to separet commands and store to link list */
    while (1) 
    {
        /* waiting for next word */
        if ((cmd[i] == ' ') || (cmd[i] == '\0'))
        {
            insert(&head,buf);

            i++;
            j = 0;
            if (cmd[i] == '\0')
            {
                free(buf);
                break;
            }
            memset(buf,0,20);
        }
        else
        {
            /* if not space store character */
            buf[j] = cmd[i];
            i++;
            j++;
        }
    }

    char **f_cmd = (char **)NULL;
    i = 0;

    /* creating pipes */
    if (pipe(fd1) == -1)
    {
        perror("Pipe fail\n");
        exit(1);
    }

    if (pipe(fd2) == -1)
    {
        perror("Pipe fail\n");
        exit(1);
    }


    /* loop to detect command and do operetion */
    ptr = head;
    while (ptr != NULL)
    {
        if (!strcmp(ptr->data,"|"))
        {
            if (flag == 0)
            {
                flag = 1;
                pid = fork();
                CUR_PID = pid;
                if (pid)
                    pid = wait(NULL);
                else
                {
                    close(fd1[0]);
                    close(fd2[0]);
                    close(fd2[1]);
                    dup2(fd1[1],1);
                    execvp(f_cmd[0],f_cmd);
                }
            }
            else
            {
                pid = fork();
                CUR_PID = pid;
                if (pid)
                {
                    if (sw_p == 0)
                    {
                        close(fd1[1]);
                        close(fd1[0]);
                        close(fd2[1]);
                    }
                    else
                    {
                        close(fd2[1]);
                        close(fd2[0]);
                        close(fd1[1]);
                    }
                    pid = wait(NULL);
                }
                else
                {
                    if (sw_p == 0)
                    {
                        sw_p = 1;
                        close(fd1[1]);
                        close(fd2[0]);
                        dup2(fd2[1],1);
                        dup2(fd1[0],0);
                        execvp(f_cmd[0],f_cmd);
                    }
                    else
                    {
                        sw_p = 0;
                        close(fd2[1]);
                        close(fd1[0]);
                        dup2(fd1[1],1);
                        dup2(fd2[0],0);
                        execvp(f_cmd[0],f_cmd);
                    }
                }
            }
            while (i > 0)
            {
                i--;
                free(f_cmd[i]);
            }
        }
        else
        {
            f_cmd = realloc(f_cmd,sizeof(char*));
            f_cmd[i] = calloc(1,sizeof(ptr->data)+1);
            strcpy(f_cmd[i],ptr->data);
            i++;
        }
        ptr = ptr->next;
    }

    pid = fork();
    CUR_PID = pid;
    if (pid)
    {
        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
        pid = waitpid(-1,&status,WUNTRACED);

        while (i > 0)
        {
            i--;
            free(f_cmd[i]);
        }

        ptr = head;
        while(ptr != NULL)
        {
            ptr2 = ptr;
            free(ptr);
            ptr = ptr2->next;
        }
        return 0;
    }
    else
    {
        if (flag == 0)
        {
            execvp(f_cmd[0],f_cmd);
        }
        else
        {
            flag = 0;
            if (sw_p == 0)
            {
                close(fd2[1]);
                close(fd1[0]);
                close(fd1[1]);
                dup2(fd2[0],0);
            }
            else
            {
                close(fd1[1]);
                close(fd2[0]);
                close(fd2[1]);
                dup2(fd1[0],0);
            }
            execvp(f_cmd[0],f_cmd);
        }
    }
}

/* making resume the process */
int forgrnd(jobs **head)
{
    jobs *ptr,*prv;

    if (*head == NULL)
        return NOT_FOUND;
    ptr = *head;
    while(ptr->next != NULL)
    {
        prv = ptr;
        ptr = ptr->next;
    }
    
    printf("pid = %d\n",ptr->pid);
    kill(ptr->pid,SIGCONT);
    
    if (ptr == *head)
    {
        *head = NULL;
        free(ptr);
    }
    else
    {
        prv->next=NULL;
        free(ptr);
    }
    job_num--;
}
