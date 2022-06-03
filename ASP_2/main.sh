#!/usr/bin/env bash

# Check for format of usage
if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ]; then
  echo "Invalid arguments"
  echo "Usage: $0 <file> -d <dir>"
  exit 1
fi

file=$1
dir=""
SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]:-$0}"; )" &> /dev/null && pwd 2> /dev/null; )";

# If file doesn't exist, exit
if [ ! -f "$file" ]; then
  echo "File '$file' does not exist"
  exit 1
fi

if [ "$2" = "-d" ]; then
  if [ -d "$3" ]; then
    # echo "Directory $3 exists"
    dir=$3
    # Check if dir has read access
    if [ ! -r "$dir" ]; then
      echo "Directory $dir does not have read access"
      exit 1
    fi
    # Check if dir has write access
    if [ ! -w "$dir" ]; then
      echo "Directory $dir does not have write access"
      exit 1
    fi
    # Check if script has write access
    if [ ! -w "$0" ]; then
      echo "Script $0 does not have write access"
      exit 1
    fi
    # Check if script dir has read and write access
    if [ ! -r "$SCRIPT_DIR" ]; then
      echo "Script dir $SCRIPT_DIR does not have read access"
      exit 1
    fi
  else
  # If directory doesn't exist, exit
    echo "Directory $3 does not exist"
    exit 1
  fi
fi

sum_md5_script="$(pwd)/sum_md5.sh"
detect_similar_script="$(pwd)/detect_similar.sh"

# Call sum_md5.sh script
source $sum_md5_script $dir
readmd5List=`cat $filename`
echo "$readmd5List"

# Call detect_similar.sh script & pass the md5 hash of file
md5=`md5sum ${file} | awk '{ print $1 }'`
# echo "MD5: $md5"
source $detect_similar_script $md5 
