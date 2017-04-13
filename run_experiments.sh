#!/bin/bash

# set up the struck configuration options for all sequences
config_file="config.txt"
sed --in-place 's/#\?quietMode.*$/quietMode = 1/' ${config_file}
sed --in-place 's/#\?debugMode.*$/debugMode = 0/' ${config_file}
sed --in-place 's|#\?sequenceBasePath.*$|sequenceBasePath = /home/brendan/Videos/struck_data|' ${config_file}
sed --in-place 's/#\?searchRadius.*$/searchRadius = 30/' ${config_file}
sed --in-place 's/#\?svmC.*$/svmC = 100.0/' ${config_file}
sed --in-place 's/#\?svmBudgetSize.*$/svmBudgetSize = 100/' ${config_file}

sequences=("coke11" "david" "faceocc" "faceocc2" "girl" "sylv" "tiger1" "tiger2")
for s in ${sequences[@]}
do
    echo "---------------------------------------------------"
    echo "tracking ${s}..."
    sed --in-place 's/#\?resultsPath.*$/resultsPath = '${s}'.csv/' ${config_file}
    sed --in-place 's/#\?sequenceName.*$/sequenceName = '${s}'/' ${config_file}
    ./struck config.txt
    ./analyze ${s}
    echo
done
