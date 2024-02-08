#!/usr/bin/env bash

tags=`ls */*.list | sed 's/\/.*$//'`

for tag in ${tags}; do
    echo ${tag}
    if [ -f ${tag}/${tag}.txt ]; then
        cd ${tag}
        mv ${tag}.list ${tag}_old.list

        indices=`cat ${tag}.txt`

        for index in ${indices}; do
            echo `grep "^${index}," ${tag}_old.list` >> ${tag}.list
            rm ${tag}.*.${index}.*
        done

        condor_submit SubmitCondor.condor
        cd ..
    fi
done