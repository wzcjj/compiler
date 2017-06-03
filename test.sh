#!/bin/bash

for((i=1;i<=4;i++));
do
    bin/parser testcase/3.${i}.c testout/3.${i}.txt
done
