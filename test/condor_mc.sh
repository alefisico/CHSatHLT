#!/usr/bin/env sh
export X509_USER_PROXY=/afs/cern.ch/user/a/algomez/x509up_u15148   ###### Dont forget to modify this line

source /cvmfs/cms.cern.ch/cmsset_default.sh
export SCRAM_ARCH=slc6_amd64_gcc630
cd $PWD
eval `scram runtime -sh`

cmsRun reRunHLT_MC_withAnalyzer_cfg.py
