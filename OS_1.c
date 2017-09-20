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
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
int get_args(char* args, char** dest){
    int n, i, j, l;


    // start from first word;
    for (j = 0; j < strlen(args) - 1; ++j){
        if (args[j] != ' '){
            l = j;
            break;
        }
    }

    n = 1;
    for (j = l; j < strlen(args) - 1; ++j){
        if (args[j] == ' '){
            n++;
        }
    }
    
    
    dest[0] = args + l;

    for (l; args[l] != '\0'; l++){
        if (args[l] == ' ' && args[l + 1] != ' '){
            dest[n] = args + l + 1;
        }
        for (i = 0; dest[n][i] != '\0'; i++){
            if (dest[n][i] == ' '){
                dest[j][i] = '\0';
                break;
            }
        }
    
    }

    dest[n++] = NULL;
    
    printf("%s\n", dest[0]);
 //   return n;
    
}

int main(int argc, char *argv[]){
   
    pid_t pid;
    int i, j, count;
    int mypipe[2];
    int pipe_ret;
    char buff_p[100], buff_c[100];
    char** args;

    pipe_ret = pipe(mypipe);

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
        read(mypipe[0], buff_c, sizeof(buff_c)/sizeof(char));
        args = malloc((100)*sizeof(char*));
        get_args(buff_c, args); 
        execv(args[0], args);
    }
    else{
        //Parent;
        printf("Input command with arguments:\n");
        close(mypipe[0]);
        scanf("%s", buff_p);
        write(mypipe[1], buff_p, sizeof(buff_p)/sizeof(char));
        get_args(buff_p, args);
        wait(0);
    }

    return 0;
    
}