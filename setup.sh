#!/bin/bash

# Create the program binary
gcc -pthread bitonic.c -o bin/bitonic.o -lm

# Generate the sample data
for input_size in "1024" "2048" "4096" "8192" "16384"
do
  ruby input.rb $input_size > data/$input_size.txt
done
