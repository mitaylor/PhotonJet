#!/usr/bin/env bash

folder=${1}
config_fragment=${2}
output_tag=${3}

# split the input files so there are no more than 30 files per job
find ${folder} -type f > ${output_tag}
split -l 30 -d -a 3 ${output_tag} ${output_tag}
files=`ls "${output_tag}?*"`

# create all of the config files
for file in ${files}; do
    sed -i '1 i\ std::vector<std::string> files =' ${file}
    sed -i 's/$/ \\/' ${file} # add \ after every line
    sed -i '${s/\\$//;p;x}' ${file} # delete the final \

    echo "$(cat config_fragment.txt)" >> ${file}.py # add the rest of the configuration file
done

find . -type f -iname "${output_tag}???" > "${output_tag}.list"
sed '=' "${output_tag}.list" | sed 'N; s/\n/, /'

echo "$(cat config_fragment.txt)"