#!/usr/bin/env bash

tags=`ls */*.list | sed 's/\/.*$//'`

for tag in ${tags}; do
    indices=`cat ${tag}/${tag}.txt`

    for index in ${indices}; do
        echo `grep "^${index}," ${tag}/${tag}.list` >> ${tag}/${tag}_temp.list
    done
done

# cat ../SubmitCondor.condor | sed "s/__MASTER__/${output_tag}/g" > SubmitCondor.condor
# sed -i "s/__CONFIG__/${config_fragment}/g" SubmitCondor.condor

# condor_submit SubmitCondor.condor