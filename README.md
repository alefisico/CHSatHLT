# PileUp Mitigation at HLT

### To set initial environment

This is different in every branch. Please check that the name of the branch corresponds to the CMSSW release.

```
cmsrel CMSSW_11_1_0_pre5
cd CMSSW_11_1_0_pre5/src
cmsenv
git cms-addpkg HLTrigger/Configuration          ### From HLT recommendations, not used yet
git clone git@github.com:cms-jet/JetToolbox.git JMEAnalysis/JetToolbox -b jetToolbox_102X_v2
git clone git@github.com:alefisico/PUmitigationatHLT.git -b 111X PUHLT/PUmitigationatHLT
scram b -j 8 
```

### ReRun HLT paths

In this version, we are reRUnning HLT pahts with (mainly) the python config file: [reRunHLT_MC_cfg.py](test/reRunHLT_MC_withAnalyzer_cfg.py).
To test if this file runs local (do not forget to activate your certificate):
```
cd $CMSSW_BASE/PUHLT/PUmitigationatHLT/test/
cmsRun reRunHLT_MC_withAnalyzer_cfg.py
```
The output of this job is called: `reRunHLTwithAnalyzer_MC.root`

This job is very computational expensive, so you will need to send it to crab or condor:
 * For crab:
The script `crab3_reRunHLTwithAnalyzer.py` will send the job to crab. You will need to change some parameters inside the script and then just run it as `python crab3_reRunHLTwithAnalyzer.py`. If you use crab, you need the DAS dataset names in the crab3 file instead of the name of the files.

 * For condor (for smaller jobs):
You need to manually modify the number of events in the `reRunHLT_MC_withAnalyzer_cfg.py` file. Look for `process.maxEvents` and modify the number of events (`-1` is for all).
Then, the script `submitCondorJobs.sh` will make all the job for you. Just modify the proxy in that file, then you run it as `source submitCondorJobs.sh`

#### How to create python files to rerun HLT

```bash
hltGetConfiguration --offline --full --mc  /users/algomez/PUatHLT --setup /dev/CMSSW_11_0_0/GRun --input /store/mc/Run3Winter20DRPremixMiniAOD/QCD_Pt_170to300_TuneCP5_14TeV_pythia8/GEN-SIM-RAW/110X_mcRun3_2021_realistic_v6-v2/40000/A623EE66-618D-FC43-B4FC-6C4029CD68FB.root --unprescale --process HLT2 --globaltag 110X_mcRun3_2021_realistic_v6 --no-output  > HLT_PUatHLT_111pre5.py
```
