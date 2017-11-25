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
char* tmp[100] = {NULL};  
int n;
// functon get_args 
// parsing a string on words with spaces as separators.
int get_args(char* lol, /*char* first_arg,*/ char** dest){
    int n, i, j, l;
    char* args;
    l = 0;
    // start from first word;
    args = malloc(100*sizeof(char));
    strcpy(args, lol);
    for (j = 0; j < strlen(args) - 1; ++j){
        if (args[j] != ' '){
            l = j;
            break;
        }
    }
    
    n = 0;
    //strcpy(dest[n], "/bin/");
    //strcat(dest[n], (args + l));
    dest[n] = malloc(100*sizeof(char));
    dest[n] = args + l;
    n++;
    // strcat(first_arg, "/bin/");
    // strcat(first_arg, args + l);
    // for (i = 0; first_arg[i] != '\0'; ++i){
    //     if (first_arg[i] == ' ')
    //         first_arg[i] = '\0';
    // }

    for (l; args[l] != '\0'; ++l){
        if (args[l] == ' ' && args[l + 1] != ' '){
            dest[n] = malloc(100*sizeof(char));
            strcpy(dest[n], args + l + 1);
            
            n++;
            
        }
    
    }

    for (j = 0; j < n; ++j){
        for (i = 0; dest[j][i] != '\0'; ++i){
            if (dest[j][i] == ' ' || dest[j][i] == '\n')
                dest[j][i] = '\0';
        }
    }

    dest[n] = NULL; 
    return n;
    
}


void handle(int sig){
    int i; 
    int len = 0;
    FILE* file;
    char line[256];
    count++;
    if (count >= 3){
        printf("I've got three interruption!\n%s", tmp[0]);
        for (i=1; tmp[i] != NULL; ++i){
            if (tmp[i][0] != '-'){
                printf("\nFile %d:\n %s\n", i+1, tmp[i]);
                file = fopen(tmp[i], "r");
                while (fgets(line, sizeof(line), file)){
                    len++;
                }
                fclose(file);
                printf("%d\n", len);
                len = 0;
            }
        }
        exit(count);
    }
}

int main(int argc, char *argv[]){
   
    pid_t pid;
    struct sigaction new_act, old_act;
    int n;
    int i, j, amount;
    int mypipe[2];
    int pipe_ret;
    char buff_p[100];
    char* buff_c[100] = {NULL};

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
        sleep(2);
        close(mypipe[1]);
        read(mypipe[0], &n, sizeof(n));
        
        for (i = 0; i < n; ++i){
            buff_c[i] = malloc(100*sizeof(char));
        }
        
        for (i = 0; i < n; ++i){
            read(mypipe[0], buff_c[i], 100);
        }
        
        
        close(mypipe[0]);
        
        execv(buff_c[0], buff_c);
    }
    else{
        //Parent;

        printf("Input command with arguments:\n");
        fgets(buff_p, 100, stdin);
        n = get_args(buff_p, tmp);
        
        strcpy(buff_p, "/bin/");
        strcat(buff_p, tmp[0]);
        strcpy(tmp[0], buff_p);
        
        printf("number of keys and files: %d\n", n);
        puts(buff_p);
        puts(tmp[0]);

        close(mypipe[0]);
        
        write(mypipe[1], &n, sizeof(n));
        
        for (i = 0; i < n; ++i){
            write(mypipe[1], tmp[i], 100);
        }
        
        close(mypipe[1]);
        wait(0);
        
        for (;;){
            pause();
        }
     }
    
    return 0;
    
}