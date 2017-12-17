#!/bin/bash
file_list=`ls /home/pangbo/share/github/dataplane/include/lib/quagga`

for file in $file_list; do
	grep -rn \"${file}\" ./
	grep -rn \"${file}\" ./ | cut -d ':' -f 1 | xargs sed -i "s/\"${file}\"/<lib\/quagga\/${file}>/g"
done
