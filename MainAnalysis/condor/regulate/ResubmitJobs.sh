#!/usr/bin/env bash

tags=`ls */*.txt | sed 's/\/.*$//'`

for tag in ${tags}; do
    if [ -f ${tag}/${tag}.txt ]; then
        echo ${tag}
        cd ${tag}
        mv ${tag}.list ${tag}_old.list

        indices=`cat ${tag}.txt`

        for index in ${indices}; do
            echo `grep "^${index}," ${tag}_old.list` >> ${tag}.list
            rm -f ${tag}.*.${index}.*
        done

        condor_submit SubmitCondor.condor
        cd ..
    fi
done