#!/bin/bash

# NAME:  ALISSA NIEWIADOMSKI
# EMAIL: aniewiadomski@ucla.edu
# UID:   *********

`./lab0 --input="input.txt" --output="output.txt"`

echo "Test that it should exit successfully with input and output arguments"
if [ $? -eq 0 ]
then
    echo "=> PASSED TEST"
else
    echo "=> FAILED TEST"
fi

echo "Test that the input file should be copied to the output file correctly"
`cmp input.txt output.txt` 
if [ $? -eq 0 ]
then 
    echo "=> PASSED TEST"
else 
    echo "=> FAILED TEST"
fi

echo "Test that bogus arguments exit with code 1"
`./lab0 --hi`
if [ $? -eq 1 ]
then
    echo "=> PASSED TEST"
else
    echo "=> FAILED TEST"
fi

echo "Test that an invalid input argument exits with code 2"
`./lab0 --input="dnefile" 2> /dev/null`
if [ $? -eq 2 ]
then 
  echo "=> PASSED TEST"
else 
  echo "=> FAILED TEST"
fi

echo "Test that the catch handles the segfault"
`./lab0 --catch --segfault 2> /dev/null`
if [ $? -eq 4 ]
then 
    echo "=> PASSED TEST"
else 
  echo "=> FAILED TEST"
fi

echo "Test that the dumpcore cancels out the catch"
`./lab0 --catch --dump-core --segfault 2> /dev/null`
if [ $? -eq 139 ]
then
    echo "=> PASSED TEST"
else
    echo "=> FAILED TEST"
fi
