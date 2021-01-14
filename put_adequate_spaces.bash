#!/bin/bash

sed -i 's/while(/while (/' $1
sed -i 's/if(/if (/' $1
sed -i 's/return(/return (/' $1
sed -i 's/return;/return ;/' $1
sed -i 's/break;/break ;/' $1
