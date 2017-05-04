#!/bin/bash

# color codes for script output
error_color='\033[1;31m'
success_color='\033[0;32m'
no_color='\033[0m'

# if a mode & sequence weren't specified on the command line, print how to run the script and
# exit.
if [[ $# -lt 2 ]]
then
    echo "usage: demo.sh MODE SEQUENCE"
    echo "    possible modes are:"
    echo "       original"
    echo "       new"
    echo "    possible sequence names are:"
    echo "       cliffbar"
    echo "       coke11"
    echo "       david"
    echo "       dollar"
    echo "       faceocc"
    echo "       faceocc2"
    echo "       girl"
    echo "       surfer"
    echo "       sylv"
    echo "       tiger1"
    echo "       tiger2"
    echo "       twinings"
    exit 0
fi

# set some configuration parameters based on the mode. if the mode isn't recognized, report an
# error.
if [[ "$1" == "original" ]]
then
    loss="iou"
    manipulator="identity"
    processOld="on"
    optimizeAll="off"
elif [[ "$1" == "new" ]]
then
    loss="distance"
    manipulator="smoothStep"
    processOld="off"
    optimizeAll="on"
else
    >&2 echo -e "${error_color}error: $1 is not a valid mode"
    exit 1
fi

# 'write' the Struck configuration file
config_file="config.txt"
sed --in-place 's/#\?loss.*$/loss = '${loss}'/' ${config_file}
sed --in-place 's/#\?manipulator.*$/manipulator = '${manipulator}'/' ${config_file}
sed --in-place 's/#\?processOld.*$/processOld = '${processOld}'/' ${config_file}
sed --in-place 's/#\?optimizeAll.*$/optimizeAll = '${optimizeAll}'/' ${config_file}
sed --in-place 's/#\?quietMode.*$/quietMode = 0/' ${config_file}
sed --in-place 's/#\?debugMode.*$/debugMode = 0/' ${config_file}
sed --in-place 's|#\?sequenceBasePath.*$|sequenceBasePath = /home/brendan/Videos/struck_data|' ${config_file}
sed --in-place 's/#\?searchRadius.*$/searchRadius = 30/' ${config_file}
sed --in-place 's/#\?svmC.*$/svmC = 100.0/' ${config_file}
sed --in-place 's/#\?svmBudgetSize.*$/svmBudgetSize = 100/' ${config_file}
sed --in-place 's/#\?seed =.*$/seed = 0/' ${config_file}
sed --in-place 's/#\?resultsPath.*$/resultsPath = '${2}'.boxes/' ${config_file}
sed --in-place 's/#\?sequenceName.*$/sequenceName = '${2}'/' ${config_file}
./struck config.txt
