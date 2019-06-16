#!/bin/sh

#================================================================
#   Copyright (C) 2019 Lynxi Ltd. All rights reserved.
#   
#   @name          ：testDir.sh
#   @author        ：Afly
#   @date          ：2019.06.15
#   @description   ：
#
#================================================================

dir=$1

if [ -e $dir ]; then
    echo File exists;
else
    echo Does not exist;
fi


final=${dir%*/}
echo ${final}
echo ${dir}
expr substr "$dir" 1 2

data="name,sex rollno location"

oldIFS=$IFS
#IFS=,
for item in $(cat $dir)
#for item in $data
do 
    echo Item: $item
done
IFS=$oldIFS

for ip in 192.168.1.{1..255} ;
do 
    (
        ping $ip -c2 &> /dev/null ;
        if [ $? -eq 0 ];
        then
            echo $ip i alive
        fi
        
    )&
done
wait
