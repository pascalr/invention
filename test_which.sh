#!/bin/bash

var="$(bin/which_acm0)"

if [[ $var =~ "fixed" ]]; then
  echo "ACM0 is fixed!!!"
else
  echo "ACM0 is not fixed!!!"
fi
