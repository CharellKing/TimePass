#!/bin/bash
if [ $# -ne 2 ] #>
then
        echo "usage: $0 [cmd] [times]"
        exit 1
fi



cmd=${1}
times=${2}

for((i=0;i<${times};i++))
do
  	./bin/${cmd}
done
