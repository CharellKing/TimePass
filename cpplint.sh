#!/bin/bash
if [ $# -gt 1 ] #>
then
echo "usage: $0 TIMEPASS-PROJECT-ROOT(/usr/share etc..)"
exit 1
fi

root=`pwd`
if [ $# -eq 1 ]
then
root=${1}
fi

#TimePass's source relative path
paths=("/src/shm" "/src/global")
for path in ${paths[@]}
do
    sources=`find "${root}${path}" -name "*.cpp" -o -name "*.h"`
    for source in ${sources[@]}
    do
        cpplint ${source}
    done
done

#example's main.cpp
cd ${root}/example
source=`find . -name "main.cpp"`
for path in ${source[@]}
do
    cpplint ${path}
done
