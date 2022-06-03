#!/usr/bin/env bash

owner="$(whoami)"
md5dir="MD5_SUM"
SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]:-$0}"; )" &> /dev/null && pwd 2> /dev/null; )"; # Get script directory
fileStoreDir="$SCRIPT_DIR/$md5dir" # Create directory to store duplicate files
filename="$SCRIPT_DIR/md5List.txt"
hashValues=("$@")

if [[ ! -e $filename ]]; then
# File doesn't exist
    echo "File doesn't exist: $filename"
    exit 1
fi

if [[ ! -d $fileStoreDir ]]; then
# create dir if it doesn't exist
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
            if [[ $fileName == "$originalName" ]]; then
                duplicateName=true # duplicate name found
            else
                originalName=$fileName # set as original file name
            fi
        fi
    fi
done
finalDuplicateFileCount=$[$COUNTER -1]

if [[ $finalDuplicateFileCount -gt 0 ]]; then
    # Logic for renaming duplicate files
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
    else
        # Logic for moving duplicate files to fileStoreDir
        for file in ${duplicateFileNamesArray[@]}; do
            filepath=${file%.*}
            fileName=${filepath##*/}
            extension=${file##*.}
            newFileName="$fileStoreDir/$fileName.$extension"
            mv -i -- "$file" "$newFileName"
        done
    fi
fi

echo ""
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
    # echo "[MD5 sum of the content]"
    ( IFS=$'\n'; echo "${md5ContentArray[*]}" )
    # echo "Duplicate files: ${md5ContentArray[@]}"
    echo "Number of identical files to $fileName.$extension: $finalDuplicateFileCount"
else
    echo "Number of identical files to $originalName: 0"
fi
