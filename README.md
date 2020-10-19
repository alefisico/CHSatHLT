# PileUp Mitigation at HLT

### To set initial environment

This is different in every branch. Please check that the name of the branch corresponds to the CMSSW release.

```
cmsrel CMSSW_11_2_0_pre5
cd CMSSW_11_2_0_pre5/src
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

The output of crab usually contains several files like `reRunHLTwithAnalyzer_MC_*.root`. You can use the `hadd` command from root to merge the files. For instance, if the files are located in an EOS area like `/eos/home-a/algomez/PUatHLT/QCD_Pt-15to3000_TuneCP5_Flat_14TeV_pythia8/crab_QCD_Pt-15to3000_TuneCP5_Flat_14TeV_pythia8_TriggerStudies_v01/200430_101648/0000/`:
```
hadd QCD_Pt-15to3000_TuneCP5_Flat_14TeV_pythia8.root  /eos/home-a/algomez/PUatHLT/PUatHLT/QCD_Pt-15to3000_TuneCP5_Flat_14TeV_pythia8/crab_QCD_Pt-15to3000_TuneCP5_Flat_14TeV_pythia8_TriggerStudies_v01/200430_101648/0000/reRunHLT*root
```
The file `QCD_Pt-15to3000_TuneCP5_Flat_14TeV_pythia8.root` can be used to further analyze the histograms created in the previous step.

#### How to create python files to rerun HLT

```bash
hltGetConfiguration --offline --full --mc  /users/algomez/PUatHLT --setup /dev/CMSSW_11_0_0/GRun --input /store/mc/Run3Winter20DRPremixMiniAOD/QCD_Pt_170to300_TuneCP5_14TeV_pythia8/GEN-SIM-RAW/110X_mcRun3_2021_realistic_v6-v2/40000/A623EE66-618D-FC43-B4FC-6C4029CD68FB.root --unprescale --process HLT2 --globaltag 110X_mcRun3_2021_realistic_v6 --no-output  > HLT_PUatHLT_111pre5.py
```


#### Create file to run several paths

Once you have a test menu like `/users/algomez/PUatHLT` with different paths, and you want to check different thresholds for each path, first dump the main python configuration as:
```bash
hltGetConfiguration --offline --full --mc /users/algomez/PUatHLT --setup /dev/CMSSW_11_2_0/GRun --input /store/mc/Run3Winter20DRPremixMiniAOD/QCD_Pt_600oInf_TuneCP5_14TeV_pythia8/GEN-SIM-RAW/110X_mcRun3_2021_realistic_v6-v2/10000/6F329F1F-1DD4-AE42-AF5F-1C76166B5DA5.root --unprescale --process HLT2 --globaltag 110X_mcRun3_2021_realistic_v6 --no-output > HLT_PUatHLT_1120pre5.py
```
This step creates a python config file for MC, settings for `CMSSW_11_2_0`, and input file `/store/mc/Run3Winter20DRPremixMiniAOD/QCD_Pt_600oInf_TuneCP5_14TeV_pythia8/GEN-SIM-RAW/110X_mcRun3_2021_realistic_v6-v2/10000/6F329F1F-1DD4-AE42-AF5F-1C76166B5DA5.root --unprescale --process HLT2 --globaltag 110X_mcRun3_2021_realistic_v6`, without any output. You can test that works by:
```
cmsRun HLT_PUatHLT_1120pre5.py
```
Next, there is a script ([`path_maker.py`](test/path_maker.py)) that clones the chosen path and changes thresholds. An example of how to run this module is:
```
python path_maker.py -i HLT_PUatHLT_1120pre5.py -p HLT_PFJet550_v11 -c "hltPrePFJet550.offset=cms.uint32( 0 )"  "hltSinglePFJet550.MinPt=cms.double( 500.0 )" -r "Pt500" -o hlt_tmp1.py
```
where `-i` is the input python config file, `-p` is the path for cloning,  `-c` represents the modules that you want to change the thresholds, `-r` is the suffix for the new path, and `-o` is the output python config file. There is  a *very important*  thing to remember in `-c` every time that a trigger is clone, you __must__ change the prescale module. (The prescale module starts with `hltPre` followed by the name of the HLT path). In other words you should always include something like `"hltPreNAMEOFHLTPATH.offset=cms.uint32( 0 )" ` before changing any other module of the trigger. 
If you are in doubt of a name of the module, or the name of the parameter chosen, you can always open the original python config file and look for the name of the module and the parameters. 

Because usually you need to test several thresholds at the time, you can create a bash script to run several of these tests. An example is found in [createPaths.sh](test/createPaths.sh).

Once you change the modules that you need, the python config file will NOT produce any output file and it will run only on `GEN-SIM-RAW` files. Because we need to analyze the performance of these triggers with offline objects (stored in `MINIAOD` files), first we need to include manually [these files](https://github.com/alefisico/PUmitigationatHLT/blob/112X/test/reRunHLT_MC_withAnalyzer_cfg.py#L6-L39). Then we need to also copy manually lines like [these ones](https://github.com/alefisico/PUmitigationatHLT/blob/112X/test/reRunHLT_MC_withAnalyzer_cfg.py#L17489-L17577) to analyze the newly cloned paths. 

After done this, *always* test that your job run local in your terminal (aka, with cmsRun) and once that works you can submit a higher stats job with condor or crab.


#### Draw histograms 

A [DrawHistograms.py](test/DrawHistograms.py) python script can be use to create histograms. Currently it is very specific for the output of the reRunHLT step above. The script assumes that the input file is located under `test/Rootfiles/`, and creates two types of histograms: mean response and comparison of different histograms. To run it:
```
python DrawHistograms.py -v v01 -p simple
```
`-p` can be `simple` for a simple comparison of different selections, or `meanRes` to compare different responses. `-v` is to include a version, just for bookkepping. The output plots are located under `test/Plots/`, to open them you could use `eog`, or `display` within lxplus, or copy them local to your machine with `scp`.
