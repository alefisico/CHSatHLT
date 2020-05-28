initialPyConfig=HLT_PUatHLT_111pre5.py

cp $initialPyConfig hlt_tmp2.py
PUMethods=( 'CHS' 'PUPPI' 'SK' )

python path_maker.py -i hlt_tmp2.py -p HLT_PFJetNoThreshold_v1 -c "hltPrePFJetNoThreshold.offset=cms.uint32( 0 )"  "hltSinglePFJetNoThreshold.MinPt=cms.double( 500.0 )" -r "Pt500" -o hlt_tmp1.py
python path_maker.py -i hlt_tmp1.py -p HLT_PFJetNoThreshold_v1 -c "hltPrePFJetNoThreshold.offset=cms.uint32( 0 )"  "hltSinglePFJetNoThreshold.MinPt=cms.double( 450.0 )" -r "Pt450" -o hlt_tmp2.py
python path_maker.py -i hlt_tmp2.py -p HLT_PFJetNoThreshold_v1 -c "hltPrePFJetNoThreshold.offset=cms.uint32( 0 )"  "hltSinglePFJetNoThreshold.MinPt=cms.double( 400.0 )" -r "Pt400" -o hlt_tmp1.py
python path_maker.py -i hlt_tmp1.py -p HLT_PFJetNoThreshold_v1 -c "hltPrePFJetNoThreshold.offset=cms.uint32( 0 )"  "hltSinglePFJetNoThreshold.MinPt=cms.double( 350.0 )" -r "Pt350" -o hlt_tmp2.py

for HT in 1050 1000 950 900 850; do
     python path_maker.py -i hlt_tmp2.py -p HLT_PFHTNoThreshold_v1 -c "hltPrePFHTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHTJetNoThreshold.minPtJetHt=cms.double( 10.0 )" "hltPFHT0JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt10HT${HT}" -o hlt_tmp1.py
     python path_maker.py -i hlt_tmp1.py -p HLT_PFHTNoThreshold_v1 -c "hltPrePFHTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHTJetNoThreshold.minPtJetHt=cms.double( 20.0 )" "hltPFHT0JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt20HT${HT}" -o hlt_tmp2.py
     python path_maker.py -i hlt_tmp2.py -p HLT_PFHTNoThreshold_v1 -c "hltPrePFHTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHTJetNoThreshold.minPtJetHt=cms.double( 30.0 )" "hltPFHT0JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt30HT${HT}" -o hlt_tmp1.py
     python path_maker.py -i hlt_tmp1.py -p HLT_PFHTNoThreshold_v1 -c "hltPrePFHTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHTJetNoThreshold.minPtJetHt=cms.double( 40.0 )" "hltPFHT0JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt40HT${HT}" -o hlt_tmp2.py
     python path_maker.py -i hlt_tmp2.py -p HLT_PFHTNoThreshold_v1 -c "hltPrePFHTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHTJetNoThreshold.minPtJetHt=cms.double( 50.0 )" "hltPFHT0JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt50HT${HT}" -o hlt_tmp1.py

     python path_maker.py -i hlt_tmp1.py -p HLT_PFHTNoThreshold_v1 -c "hltPrePFHTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHTJetNoThreshold.minPtJetHt=cms.double( 10.0 )" "hltPFHTJetNoThreshold.maxEtaJetHt=cms.double( 5.0 )" "hltPFHT0JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt10Eta5HT${HT}" -o hlt_tmp2.py
     python path_maker.py -i hlt_tmp2.py -p HLT_PFHTNoThreshold_v1 -c "hltPrePFHTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHTJetNoThreshold.minPtJetHt=cms.double( 20.0 )" "hltPFHTJetNoThreshold.maxEtaJetHt=cms.double( 5.0 )" "hltPFHT0JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt20Eta5HT${HT}" -o hlt_tmp1.py
     python path_maker.py -i hlt_tmp1.py -p HLT_PFHTNoThreshold_v1 -c "hltPrePFHTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHTJetNoThreshold.minPtJetHt=cms.double( 30.0 )" "hltPFHTJetNoThreshold.maxEtaJetHt=cms.double( 5.0 )" "hltPFHT0JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt30Eta5HT${HT}" -o hlt_tmp2.py
     python path_maker.py -i hlt_tmp2.py -p HLT_PFHTNoThreshold_v1 -c "hltPrePFHTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHTJetNoThreshold.minPtJetHt=cms.double( 40.0 )" "hltPFHTJetNoThreshold.maxEtaJetHt=cms.double( 5.0 )" "hltPFHT0JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt40Eta5HT${HT}" -o hlt_tmp1.py
     python path_maker.py -i hlt_tmp1.py -p HLT_PFHTNoThreshold_v1 -c "hltPrePFHTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHTJetNoThreshold.minPtJetHt=cms.double( 50.0 )" "hltPFHTJetNoThreshold.maxEtaJetHt=cms.double( 5.0 )" "hltPFHT0JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt50Eta5HT${HT}" -o hlt_tmp2.py

done

for PU in ${PUMethods[@]}; do

     python path_maker.py -i hlt_tmp2.py -p HLT_PF${PU}JetNoThreshold_v1 -c "hltPrePF${PU}JetNoThreshold.offset=cms.uint32( 0 )"  "hltSinglePF${PU}JetNoThreshold.MinPt=cms.double( 500.0 )" -r "Pt500" -o hlt_tmp1.py
     python path_maker.py -i hlt_tmp1.py -p HLT_PF${PU}JetNoThreshold_v1 -c "hltPrePF${PU}JetNoThreshold.offset=cms.uint32( 0 )"  "hltSinglePF${PU}JetNoThreshold.MinPt=cms.double( 450.0 )" -r "Pt450" -o hlt_tmp2.py
     python path_maker.py -i hlt_tmp2.py -p HLT_PF${PU}JetNoThreshold_v1 -c "hltPrePF${PU}JetNoThreshold.offset=cms.uint32( 0 )"  "hltSinglePF${PU}JetNoThreshold.MinPt=cms.double( 400.0 )" -r "Pt400" -o hlt_tmp1.py
     python path_maker.py -i hlt_tmp1.py -p HLT_PF${PU}JetNoThreshold_v1 -c "hltPrePF${PU}JetNoThreshold.offset=cms.uint32( 0 )"  "hltSinglePF${PU}JetNoThreshold.MinPt=cms.double( 350.0 )" -r "Pt350" -o hlt_tmp2.py

    for HT in 1050 1000 950 900 850; do
         python path_maker.py -i hlt_tmp2.py -p HLT_PF${PU}HTNoThreshold_v1 -c "hltPrePF${PU}HTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHT${PU}JetNoThreshold.minPtJetHt=cms.double( 10.0 )" "hltPFHT0${PU}JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt10HT${HT}" -o hlt_tmp1.py
         python path_maker.py -i hlt_tmp1.py -p HLT_PF${PU}HTNoThreshold_v1 -c "hltPrePF${PU}HTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHT${PU}JetNoThreshold.minPtJetHt=cms.double( 20.0 )" "hltPFHT0${PU}JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt20HT${HT}" -o hlt_tmp2.py
         python path_maker.py -i hlt_tmp2.py -p HLT_PF${PU}HTNoThreshold_v1 -c "hltPrePF${PU}HTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHT${PU}JetNoThreshold.minPtJetHt=cms.double( 30.0 )" "hltPFHT0${PU}JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt30HT${HT}" -o hlt_tmp1.py
         python path_maker.py -i hlt_tmp1.py -p HLT_PF${PU}HTNoThreshold_v1 -c "hltPrePF${PU}HTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHT${PU}JetNoThreshold.minPtJetHt=cms.double( 40.0 )" "hltPFHT0${PU}JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt40HT${HT}" -o hlt_tmp2.py
         python path_maker.py -i hlt_tmp2.py -p HLT_PF${PU}HTNoThreshold_v1 -c "hltPrePF${PU}HTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHT${PU}JetNoThreshold.minPtJetHt=cms.double( 50.0 )" "hltPFHT0${PU}JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt50HT${HT}" -o hlt_tmp1.py

         python path_maker.py -i hlt_tmp1.py -p HLT_PF${PU}HTNoThreshold_v1 -c "hltPrePF${PU}HTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHT${PU}JetNoThreshold.minPtJetHt=cms.double( 10.0 )" "hltPFHT${PU}JetNoThreshold.maxEtaJetHt=cms.double( 5.0 )" "hltPFHT0${PU}JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt10Eta5HT${HT}" -o hlt_tmp2.py
         python path_maker.py -i hlt_tmp2.py -p HLT_PF${PU}HTNoThreshold_v1 -c "hltPrePF${PU}HTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHT${PU}JetNoThreshold.minPtJetHt=cms.double( 20.0 )" "hltPFHT${PU}JetNoThreshold.maxEtaJetHt=cms.double( 5.0 )" "hltPFHT0${PU}JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt20Eta5HT${HT}" -o hlt_tmp1.py
         python path_maker.py -i hlt_tmp1.py -p HLT_PF${PU}HTNoThreshold_v1 -c "hltPrePF${PU}HTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHT${PU}JetNoThreshold.minPtJetHt=cms.double( 30.0 )" "hltPFHT${PU}JetNoThreshold.maxEtaJetHt=cms.double( 5.0 )" "hltPFHT0${PU}JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt30Eta5HT${HT}" -o hlt_tmp2.py
         python path_maker.py -i hlt_tmp2.py -p HLT_PF${PU}HTNoThreshold_v1 -c "hltPrePF${PU}HTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHT${PU}JetNoThreshold.minPtJetHt=cms.double( 40.0 )" "hltPFHT${PU}JetNoThreshold.maxEtaJetHt=cms.double( 5.0 )" "hltPFHT0${PU}JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt40Eta5HT${HT}" -o hlt_tmp1.py
         python path_maker.py -i hlt_tmp1.py -p HLT_PF${PU}HTNoThreshold_v1 -c "hltPrePF${PU}HTNoThreshold.offset=cms.uint32( 0 )"  "hltPFHT${PU}JetNoThreshold.minPtJetHt=cms.double( 50.0 )" "hltPFHT${PU}JetNoThreshold.maxEtaJetHt=cms.double( 5.0 )" "hltPFHT0${PU}JetNoThreshold.minHt=cms.vdouble( ${HT}.0 )" -r "Pt50Eta5HT${HT}" -o hlt_tmp2.py

     done

done

cp hlt_tmp2.py ${initialPyConfig/.py/_moreThresholds.py}
