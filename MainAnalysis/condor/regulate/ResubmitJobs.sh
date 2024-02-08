#!/usr/bin/env bash

tags=`ls */*.list | sed 's/\/.*$//'`

for tag in ${tags}; do
    if [ -f ${tag}/${tag}.txt ]; then
        indices=`cat ${tag}/${tag}.txt`

        for index in ${indices}; do
            echo `grep "^${index}," ${tag}/${tag}.list` >> ${tag}/${tag}_temp.list
        done

        cd ${tag}
        mv ${tag}_temp.list ${tag}.list
        condor_submit SubmitCondor.condor
        cd ..
    fi
done