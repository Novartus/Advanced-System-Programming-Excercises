#/bin/bash
echo “Enter dir”
read dir
 
cd $dir
 
for i in $(find $dir -type f);
do
    md5=`md5sum ${i} | awk '{ print $1 }'`
    owner=$(stat -c '%G' ${i})
    filePath=$(realpath ${i})
    echo "$filePath:$owner:$md5"; 
done;