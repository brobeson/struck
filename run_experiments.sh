#!/bin/bash

if [[ $# -eq 1 ]]
then
    random_seed=$1
else
    random_seed=0
fi

# color codes for script output
error_color='\033[1;31m'
success_color='\033[0;32m'
no_color='\033[0m'

# set up the struck configuration options for all sequences
config_file="config.txt"
sed --in-place 's/#\?quietMode.*$/quietMode = 1/' ${config_file}
sed --in-place 's/#\?debugMode.*$/debugMode = 0/' ${config_file}
sed --in-place 's|#\?sequenceBasePath.*$|sequenceBasePath = /home/brendan/Videos/struck_data|' ${config_file}
sed --in-place 's/#\?searchRadius.*$/searchRadius = 30/' ${config_file}
sed --in-place 's/#\?svmC.*$/svmC = 100.0/' ${config_file}
sed --in-place 's/#\?svmBudgetSize.*$/svmBudgetSize = 100/' ${config_file}
sed --in-place 's/#\?seed =.*$/seed = '${random_seed}'/' ${config_file}

sequences=("coke11" "david" "faceocc" "faceocc2" "girl" "sylv" "tiger1" "tiger2")
for s in ${sequences[@]}
do
    echo "---------------------------------------------------"
    echo "tracking ${s}..."
    sed --in-place 's/#\?resultsPath.*$/resultsPath = '${s}'.csv/' ${config_file}
    sed --in-place 's/#\?sequenceName.*$/sequenceName = '${s}'/' ${config_file}
    ./struck config.txt
    if [[ ! $? -eq 0 ]]
    then
        >&2 echo -e "${error_color}error: struck failed to complete sequence ${s}, skipping analysis"
    else
        ./analyze ${s}
    fi
    echo
done
