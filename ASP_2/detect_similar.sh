#!/usr/bin/env bash

filename='/mnt/c/Users/hudan/Desktop/ASP_2/md5List.txt'
owner="$(whoami)"
md5dir="MD5_SUM"
SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]:-$0}"; )" &> /dev/null && pwd 2> /dev/null; )";
# SCRIPT_PATH="dirname ${BASH_SOURCE[0]:-$0}";
fileStoreDir="$SCRIPT_DIR/$md5dir"
# fileStoreDir="$(pwd)/$md5dir"
# echo "Who Am I ?: $owner"
hashValues=("$@")
# echo "Hashed Val: ${hashValues[0]}"

if [[ ! -e $filename ]]; then
# File doesn't exist
    echo "File doesn't exist: $filename"
    exit 1
fi

if [[ ! -d $fileStoreDir ]]; then
# create dir
    mkdir $fileStoreDir
fi

lines=`cat $filename`
duplicateDataArray=()
duplicateFileNamesArray=()
COUNTER=0
fileName=""
originalName=""
duplicateName=false
renameCounter=1


for line in $lines; do
    if [[ $line == *"${hashValues[0]}" ]]; then
        COUNTER=$[$COUNTER +1]
        if [[ $COUNTER -ge 1 ]]; then
            duplicateDataArray+=("$line")
            duplicateFileNamesArray+=("$(echo $line | cut -d ":" -f 1)") # format of line: OriginalPath/Filename -> Filename
            fullFilePath="$(echo $line | cut -d ":" -f 1)" # file name
            fileName="${fullFilePath##*/}"  # extract file name by last value of /
            # echo "File Name: ${fileName##*/}"
            if [[ $fileName == "$originalName" ]]; then
                duplicateName=true # duplicate name found
                if [[ ! -d "${md5dir}" ]]; then # check if dir exists or not
                    # echo "The dir doesn't exist: ${md5dir}"
                    mkdir -p "${md5dir}"
                fi
            else
                originalName=$fileName # set as original file name
            fi
        fi
    fi
done
finalDuplicateFileCount=$[$COUNTER -1]
# echo "Duplicate files count: $finalDuplicateFileCount"


# echo "Original file name: $originalName"
if $duplicateName
then 
    for file in ${duplicateFileNamesArray[@]}; do
    filepath=${file%.*}
    fileName=${filepath##*/}
    extension=${file##*.}
    newFileName="$fileStoreDir/$fileName$renameCounter.$extension"
    mv -i -- "$file" "$newFileName"
    renameCounter=$[$renameCounter +1]
    done
    # echo "Duplicate file name found"
fi


for i in "${duplicateDataArray[@]}"; do
    md5ContentArray+=("$(echo $i | cut -d ":" -f 1)->$(echo $i | cut -d ":" -f 3)")
done


# echo "duplicateFileNamesArray: ${duplicateFileNamesArray[@]}"
if [ $finalDuplicateFileCount -gt 0 ]; then
    file=${duplicateFileNamesArray[0]}
    filepath=${file%.*}
    fileName=${filepath##*/}
    extension=${file##*.}
    # echo "Duplicate files found"
    echo "[MD5 sum of the content]"
    ( IFS=$'\n'; echo "${md5ContentArray[*]}" )
    # echo "Duplicate files: ${md5ContentArray[@]}"
    echo "Number of identical files to $fileName.$extension: $finalDuplicateFileCount"
else
    echo "Number of identical files to $originalName: 0"
fi

