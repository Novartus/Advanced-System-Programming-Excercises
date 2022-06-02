#!/usr/bin/env bash

while getopts ":d:" arg; do
  case $arg in
    d) dir=$OPTARG;;
  esac
done

echo "Dir Passed $dir!"

sum_md5_script="$(pwd)/sum_md5.sh"
detect_similar_script="$(pwd)/detect_similar.sh"

# Call sum_md5.sh script
source $sum_md5_script $dir
readmd5List=`cat $filename`
echo "$readmd5List"

# Call detect_similar.sh script
