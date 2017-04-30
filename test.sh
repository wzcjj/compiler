#!/bin/bash

for((i=1;i<=23;i++));
do
    bin/parser testcase/2.${i}.c testout/2.${i}.txt
done
