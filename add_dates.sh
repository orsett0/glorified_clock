#!/usr/bin/env bash

# Copyright (C) 2023 Alessio Orsini <alessiorsini.ao@proton.me>
# SPDX-License-Identifier: GPL-3.0-or-later
# 
# A copy of the GNU General Public License is available in the 
# LICENSE file, in the project root directory.

# Provide a file dates.lst containing a date and description for each line, in the format:
# YYYY-MM-DD DST Space-separated string containing description
# where DST assumes the values 0 or 1

touch res.lst
while read line; do 
    d=$(echo $line | cut -d' ' -f1)
    sec=$(date -d$d "+%s")
    dst=$(echo $line | cut -d' ' -f2)
    if [[ "$dst" == "1" ]]; then 
        sec=$(($sec+7200))
    elif [[ "$dst" == "0" ]]; then 
        sec=$(($sec+3600))
    fi
    echo $sec $(echo $line | cut -d' ' -f3-) >> res.lst
done < dates.lst

while read line; do 
    sec=$(echo $line | cut -d' ' -f1) 
    echo $sec,
done < res.lst

while read line; do 
    desc=$(echo $line | cut -d' ' -f2-)
    echo \"$desc\",
done < res.lst

rm res.lst