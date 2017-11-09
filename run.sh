#!/bin/bash
TIMEFORMAT=%R
> $3
for i in {1..19}
do
  (time ./bin/bitonic.o $1 $2) 2>> $3
done
