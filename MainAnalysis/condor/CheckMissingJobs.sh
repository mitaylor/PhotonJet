#!/usr/bin/env bash

# confs=$(ls */*.conf | awk -F / '{print $2}' | sed 's/conf/root/g' | sed 's/^/\/data\/submit\/mitay\/photons\/condor\//')
ls */*.conf | awk -F / '{print $2}' | sed 's/conf/root/g' | sed 's/^/\/data\/submit\/mitay\/photons\/condor\//' > temp.txt

cp /tmp/x509up_u168456 x509up_u168456

xargs -I % bash -c '[[ ! -e $1 ]] && echo "$1"' _ % < temp.txt > missing.txt

rm temp.txt

# for conf in ${confs}; do
#     # echo $conf
#     bash -c '[[ ! -e ${conf} ]] && echo "${conf}"'
# done