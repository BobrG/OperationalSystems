# function for handling interupruption;
# after three interuprtions it outputs amount of strings in files which were
# inputed for ls;
handler()
{
    if [ $flag != 0 ] #if statement, [ expr ] <=> test expr
    then
        count=`expr $count + 1` # Everything you type between backticks is evaluated 
                                # (executed) by the shell before the main command :)
        echo $count
        echo "try once more!"
    else
        echo "input options!"
    fi

    

    if [ $count -gt 3 ]
    then
        
        s=`echo "$options"|sed -e 's/-[a-z]*//g'` # sed - stream editor for filtering
                                                    # and transforming text;
                                                    # -e - add the script to the commands to be executed;
        # s - stands for substitute; 
        # g - stands for global;
        echo $s
        if [ $s != $options ]
        then
            wc -l $s #  print newline, word, and byte counts for each file, -l - means count newlines;
        else
            echo "no files were inputed!"
        fi
        exit 0 # exit cause normal process termination;
    fi
}


trap handler SIGINT # trap - a function built into the shell that responds
                    # to hardware signals and other events. It defines and activates handlers to be run when the shell
                    # receives signals or other special conditions;

count=0
flag=0
options=''
echo "input options" # "expr" are required for echo to understand expresion as union;
read options # get keys and names of files for ls from keyboard;
flag=1
echo $options|xargs ls  # xargs - convert input from standard input into arguments to a command;

while :
do 
    sleep 3
done

