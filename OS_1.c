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
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

sigjmp_buf obl;
int count = 0;

int get_args(char* args, char* first_arg, char** dest){
    int n, i, j, l;


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

    
    dest[n] = NULL; 
    n++;
    return n;
    
}

void handle(){
    count ++;
    signal (SIGINT, handle);
    if (count >= 3){
        printf("show file info");
        exit(count);
    }
    siglongjmp (obl, 1);    /* возвращение на последний setjmp */
}

int main(int argc, char *argv[]){
   
    pid_t pid;
    int i, j, count;
    int mypipe[2];
    int pipe_ret;
    char buff_p[100], buff_c[100];
    char** args;
    char* first_arg;


    pipe_ret = pipe(mypipe);

    signal(SIGINT, handle);

    sigsetjmp(obl, 1);
    if (pipe_ret == -1){
        perror("Creating a pipe ERROR");
        exit(1);
    }

    pid = fork();
    if (pid < 0){
        perror("Forking ERROR");
        exit(2);
    }
    else if (pid == 0){
        //Child;
        close(mypipe[1]);
        read(mypipe[0], buff_c, 100);
        close(mypipe[0]);
        args = malloc((100)*sizeof(char*));
        first_arg =  malloc(100*sizeof(char));
        count = get_args(buff_c, first_arg, args);
        for (i = 0; i < count; ++i){
            printf("Args[0]:%s\n",  args[i]);
            
         } 
        // args = realloc(args,(count)*sizeof(char*));
        execv(first_arg, args);
    }
    else{
        //Parent;
        printf("Input command with arguments:\n");
        close(mypipe[0]);
        fgets(buff_p, 100, stdin);
        sigsetjmp(obl, 1);
        write(mypipe[1], buff_p, strlen(buff_p));
        close(mypipe[1]);
        wait(0);

    }

    return 0;
    
}