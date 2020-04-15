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

In this version, we are reRUnning HLT pahts with (mainly) the python config file: [reRunHLT_MC_cfg.py](test/reRunHLT_MC_cfg.py)
