#!/bin/bash

if [ "$#" -gt 0 ]
then
    echo "There's Beans"
    echo "$#"
    echo "$?"
fi

if [ "$1" = "cool" ]
then
    echo "Cool Beans"
fi

