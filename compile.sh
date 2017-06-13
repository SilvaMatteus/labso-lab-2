#!/bin/bash

if [ -f myPsTree ]; then
    echo "Removing old myPsTree executable ..."
    rm myPsTree
fi

if [ -f topzera ]; then
    echo "Removing old topzera executable ..."
    rm topzera
fi

echo "Compiling topzera and myPstree..."

echo "..."
g++ myPsTree.cpp proc_common.cpp -o myPsTree
echo "..."
g++ topzera.cpp proc_common.cpp -o topzera -lpthread -lncurses
echo "Done!"
