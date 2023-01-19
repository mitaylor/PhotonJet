#!/usr/bin/env bash

confs=$(ls */*.conf | awk -F / '{print $2}' | sed 's/conf/root/g' | sed 's/^/\/data\/submit\/mitay\/photons\/condor\//')

cp /tmp/x509up_u168456 x509up_u168456

for conf in ${confs}; do
    bash -c '[[ ! -e ${conf} ]] && echo "${conf}"'
done