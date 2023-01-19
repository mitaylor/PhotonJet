#!/usr/bin/env bash

# confs=$(ls */*.conf | awk -F / '{print $2}' | sed 's/conf/root/g' | sed 's/^/\/data\/submit\/mitay\/photons\/condor\//')
ls */*.conf | awk -F / '{print $2}' | sed 's/conf/root/g' | sed 's/^/\/data\/submit\/mitay\/photons\/condor\//' > temp.txt
xargs -I % bash -c '[[ ! -e $1 ]] && echo "$1"' _ % < temp.txt > missing.txt
rm temp.txt

numbers=$(cat missing.txt | awk -F / '{print $7}' | awk -F . '{print $1}' | awk -F _ '{print $NF}')
folders=$(cat missing.txt | awk -F / '{print $7}' | awk -F . '{print $1}' | sed -r 's/(.*)\_.*/\1/')

cp /tmp/x509up_u168456 x509up_u168456

for folder in $folders; do
    echo $folder
done

# for conf in ${confs}; do
#     # echo $conf
#     bash -c '[[ ! -e ${conf} ]] && echo "${conf}"'
# done