/* =====================================================================================
 *
 *       Filename:  mini_shell_main.c
 *        Created:  Wednesday 08 March 2017 05:28:05  IST
 *         Author:  KEWAL UKUNDE , kewalukunde@gmail.com
 *   Organization:  Emertxe technology Banglore
 *
 * =====================================================================================
 */

#include "mini_shell_functions.c"

int main()
{
    /* defining handlers */
    signal(SIGTSTP,handler2);
    //signal(SIGINT,handler1);

    char *buff,*buf;
    char *MY_PS1 = calloc(30,sizeof(char));
    char *cmd = calloc(300,sizeof(char));
    cmd_cpy = cmd;
    strcpy(MY_PS1,"Kewal_bash");

    system("clear");
    /* printing info of shell */
    printf("====================================================================================");
    printf("=============================================================\n");
    printf("\t\t\t\t\t\t\tWELCOME TO MINI_SHELL VERSION\n");
    printf("\t\t\t\t\t\t\t   AUTHOR : KEWAL UKUNDE\n");
    printf("====================================================================================");
    printf("=============================================================\n\n");

    /* loop for taking commands */
    do
    {
        buff =(char*)get_current_dir_name();
        printf("%s:~%s$ ",MY_PS1,buff);

        __fpurge(stdin);
        /* scaning command */
        scanf("%[^\n]",cmd);

        /* various special commands */
        if (!strncmp(cmd,"PS1=",4))
        {
            buf = strtok(cmd,"=");
            strcpy(MY_PS1,strtok(NULL,"="));
        }
        else if (!strcmp(cmd,"echo $$"))
            printf("%d\n",getpid());
        else if (!strcmp(cmd,"echo $SHELL"))
            printf("%s\n",PATH);
        else if (!strcmp(cmd,"echo $?"))
            printf("%d\n",status);
        else if (!strncmp(cmd,"cd",2))
            change_dir(cmd);
        else if (!strcmp(cmd,"jobs"))
            print_jobs(job_head);
        else if (!strcmp(cmd,"my_fg"))
            forgrnd(&job_head);
        else if (!strcmp(cmd,"exit"))
            break;
        else
            command(cmd);

        /* freeing memory */
        memset(cmd,0,300);
        free(buff);
    }while(1);

    return 0;
}
