#!/bin/bash

if [[ $# -eq 0 ]] ; then
    echo 'First argument, name of sample, is needed. Have a good day :)'
else

    myproxy=/afs/cern.ch/user/a/algomez/x509up_u15148  ################## MODIFY THIS LINE WITH YOUR PROXY
    sample="QCDSample"

    #sample=$1

    condorFile=${sample}_condorJob
    echo '''
universe    =  vanilla
#arguments   =  $(myfile) $(ProcId)
executable  =  '${PWD}'/condorlogs/'${condorFile}'.sh
log         =  '${PWD}'/condorlogs/log_'${condorFile}'_$(ClusterId).log
error       =  '${PWD}'/condorlogs/log_'${condorFile}'_$(ClusterId)-$(ProcId).err
output      =  '${PWD}'/condorlogs/log_'${condorFile}'_$(ClusterId)-$(ProcId).out
#initialdir  =  '${PWD}'/
getenv      =  True
+JobFlavour = "testmatch"
queue
#queue myfile from '${sample}'.txt

    ''' > condorlogs/${condorFile}.sub

    echo '''#!/bin/bash
export SCRAM_ARCH=slc6_amd64_gcc700
export X509_USER_PROXY='${myproxy}'

cd '${CMSSW_BASE}'/src
eval `scramv1 runtime -sh`

echo ${1} ${2} ${3}
cd -
echo "Running: cmsRun '${PWD}'/reRunHLT_MC_withAnalyzer_cfg.py"
cmsRun '${PWD}'/reRunHLT_MC_withAnalyzer_cfg.py

    ''' > condorlogs/${condorFile}.sh

    condor_submit condorlogs/${condorFile}.sub

fi
