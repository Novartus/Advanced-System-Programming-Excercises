#!/usr/bin/env bash
cd $1
filename='/mnt/c/Users/hudan/Desktop/ASP_2/md5List.txt'

if [[ ! -e $filename ]]; then
# Create file if it doesn't exist
    touch $filename
else 
# Empty the file if it exists
    : > $filename
fi

for i in $(find $dir -type f);
do
    md5=`md5sum ${i} | awk '{ print $1 }'`
    owner=$(stat -c '%G' ${i})
    filePath=$(realpath ${i})
    printValue="$filePath:$owner:$md5"
    echo "$printValue" >> $filename
done;
