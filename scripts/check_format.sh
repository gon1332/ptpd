#!/bin/sh

commit=$1

cmd="git diff -U0 --no-color $commit -- '*.c' '*.h' | clang-format-diff -p1"
diff=$(eval "$cmd")
if [ $? -ne 0 ]
then
  echo "!!! Failed to check the patch for formatting issues"
  exit 1
fi

if [ -z "$diff" ]
then
  echo "Well done"
  exit 0
else
  echo "!!! Formatting inconsistencies"
  echo "!!! Please, format the code"
  echo
  echo "$diff"
  exit 1
fi
