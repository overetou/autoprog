#!/bin/bash

sed -i 's/while(/while (/' filename
sed -i 's/if(/if (/' filename
sed -i 's/return(/return (/' filename
sed -i 's/return;/return ;/' filename
sed -i 's/break;/break ;/' filename