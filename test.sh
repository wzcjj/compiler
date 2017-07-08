#!/bin/bash

for((i=1;i<=2;i++));
do
    bin/parser testcase/4.${i}.c testout/4.${i}.txt
done
