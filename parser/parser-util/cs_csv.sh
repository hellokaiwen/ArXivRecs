#!/bin/bash

src="cs.txt"
dest="cs.csv"
header="code,fullname"
echo "$header" > "$dest"

while read code name; do
	name="\"$name\""
	echo "${code},${name}" >> "$dest"
done < "$src"
