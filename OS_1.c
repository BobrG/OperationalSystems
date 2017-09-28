/*
 Вариант №2
 Написать программу, осуществляющую выполнение команды ОС UNIX ls таким образом, чтобы данные на стандартный ввод команды ls (ключи, имена файлов) 
 в параллельном процессе потомке поступали из стандарт­ного вывода основного процесса (где они вводились бы по запросу) че­рез межпроцессный канал.
 Предусмотреть в программе возможность неод­нократного прерывания от клавиатуры.
 При поступлении трех сиг­налов прерывания дополнительно вывести количество строк для каждого файла, указанного в аргументах.
 В остальных случаях просто продол­жать выполнение программы.
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <setjmp.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

sigjmp_buf obl;
volatile sig_atomic_t count = 0;
static char* args[100] = {NULL};

// functon get_args 
// parsing a string on words with spaces as separators.
void get_args(char* args, char* first_arg, char** dest){
    int n, i, j, l;

    l = 0;
    // start from first word;
    for (j = 0; j < strlen(args) - 1; ++j){
        if (args[j] != ' '){
            l = j;
            break;
        }
    }

    n = 0;
    dest[n] = args + l;
    n++;
    strcat(first_arg, "/bin/");
    strcat(first_arg, args + l);
    for (i = 0; first_arg[i] != '\0'; ++i){
        if (first_arg[i] == ' ')
            first_arg[i] = '\0';
    }

    for (l; args[l] != '\0'; ++l){
        if (args[l] == ' ' && args[l + 1] != ' '){
            dest[n] = args + l + 1;
            n++;
            
        }
    
    }

    for (j = 0; j < n; ++j){
        for (i = 0; dest[j][i] != '\0'; ++i){
            if (dest[j][i] == ' ' || dest[j][i] == '\n')
                dest[j][i] = '\0';
        }
    }

    
}

//not ready yet
void handle(int sig){
    int i;
    FILE* file;
    char line[256];
    count ++;
    if (count >= 3){
        for (i=1; args[i] != NULL; ++i){
            if (args[i][0] != '-'){
                printf("\nFile %d:\n %s\n", i+1, args[i]);
                file = fopen(args[i], "r");
                while (fgets(line, sizeof(line), file)){
                    printf("%s\n", line);
                }
                fclose(file);
            }
        }
        exit(count);
    }
    siglongjmp (obl, 1);    /* returns to last setjmp */
}

int main(int argc, char *argv[]){
   
    pid_t pid;
    struct sigaction new_act, old_act;
    int i, j, amount;
    int mypipe[2];
    int pipe_ret;
    char buff_p[100], buff_c[100];
    char* first_arg;

    pipe_ret = pipe(mypipe);


    // sa.sa_flags = SA_SIGINFO;
    // new_act.sa_sigaction = handle;
    new_act.sa_handler = handle;

    if (pipe_ret == -1){
        perror("Creating a pipe ERROR\n");
        exit(1);
    }
    
    if (sigaction(SIGINT, &new_act, NULL) < 0) {
        perror ("Sigaction ERROR\n");
        exit(2);
    }

    pid = fork();
    if (pid < 0){
        perror("Forking ERROR");
        exit(2);
    }
    else if (pid == 0){
        //Child;
        if (sigaction(SIGINT, &new_act, NULL) < 0) {
            perror ("Sigaction ERROR\n");
            exit(3);
        }
        close(mypipe[1]);
        read(mypipe[0], buff_c, 100);
        close(mypipe[0]);
        first_arg =  malloc(100*sizeof(char));
        
        get_args(buff_c, first_arg, args);
        //printf("%f\n", sizeof(args));
        sigsetjmp(obl,1);
        sleep(3);
        execv(first_arg, args);
    }
    else{
        //Parent;
        sigsetjmp(obl,1);	
        printf("Input command with arguments:\n");
        close(mypipe[0]);
        fgets(buff_p, 100, stdin);
        sigsetjmp(obl,1);
        write(mypipe[1], buff_p, strlen(buff_p));
        close(mypipe[1]);
        wait(0);

    }
    //sigsetjmp(obl,1);
    //printf("lol");
    
    return 0;
    
}