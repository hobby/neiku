#!/bin/bash 
#
# test script for progressbar;
# 

source ../../sh/progressbar.sh

if [ -n "$debug" ]
then
     set -x
fi


i=0;
> data.txt
while ((i++<100))
do
    echo "100000" >> data.txt
done

i=0
total=`cat data.txt | wc -l`

progressbar_init test_progressbar;
while read line
do
    # do something
    usleep $line;
    ret=$((i%2))
 
    # show log 
    progressbar_clean;
    echo task-$i end;
 
    # show progress bar
    ((i++));
    progressbar_show $i $total $ret;
done < data.txt
progressbar_close;
