#!/bin/bash

while [ 1 == 1 ]; do
  clear
  echo " "
  echo "                                                                           SPACE INVADER"
  echo " "
  echo "s -> start game"
  echo "e -> exit game"
  echo " "
  echo -n "> "
  read input
  if [ $input == "e" ] ; then
    break
  elif [ $input == "s" ]; then
    ./rungame
  fi
done




