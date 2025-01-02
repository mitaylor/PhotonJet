#!/bin/sh

input=${1}
output=${2}

WorkDir=`pwd`

export X509_USER_PROXY=${WorkDir}/x509up_u168456

xrdcp ${input} ${output}