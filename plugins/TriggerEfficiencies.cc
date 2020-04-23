// system include files
#include <memory>
#include <cmath>
#include <TH1.h>
#include <TH2.h>
#include <TLorentzVector.h>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"

#include "PUHLT/PUmitigationatHLT/interface/CommonVariablesStructure.h"

using namespace edm;
using namespace std;
using namespace reco;
using namespace pat;

class matchedJet {
    public:
        pat::Jet recoJet;
        reco::Jet hltJet;
};

class TriggerEfficiencies : public EDAnalyzer {

	public:
		explicit TriggerEfficiencies(const ParameterSet&);
      		static void fillDescriptions(ConfigurationDescriptions & descriptions);
		~TriggerEfficiencies() {}

	private:
		virtual void analyze(const Event&, const EventSetup&) override;
      		virtual void beginJob() override;

	HLTConfigProvider hltConfig;
	EDGetTokenT<TriggerResults> triggerBits_;
	//EDGetTokenT<PFJetCollection> hltObject_;
	EDGetTokenT<trigger::TriggerFilterObjectWithRefs> hltObject_;
	EDGetTokenT<std::vector<pat::Jet>> recoJetToken_;
	string baseTrigger_;
    vector<string> triggerPass_;
    vector<int> triggerOverlap_;
    vector<int> triggerOverlapBase_;
	double recojetPt_;
	bool AK8jets_;
	bool DEBUG_;
	int triggerBit;

	Service<TFileService> fs_;
	map< string, TH1D* > histos1D_;
	map< string, TH2D* > histos2D_;
};

TriggerEfficiencies::TriggerEfficiencies(const ParameterSet& iConfig):
	triggerBits_(consumes<TriggerResults>(iConfig.getParameter<InputTag>("bits"))),
	hltObject_(consumes<trigger::TriggerFilterObjectWithRefs>(iConfig.getParameter<InputTag>("hltObject"))),
	//hltObject_(consumes<reco::PFJetCollection>(iConfig.getParameter<edm::InputTag> ( "hltObject" ))),
	recoJetToken_(consumes<std::vector<pat::Jet>>(iConfig.getParameter<InputTag>("recoJets")))
{
	baseTrigger_ = iConfig.getParameter<string>("baseTrigger");
	triggerPass_ = iConfig.getParameter<vector<string>>("triggerPass");
	triggerOverlap_ = iConfig.getParameter<vector<int>>("triggerOverlap");
	triggerOverlapBase_ = iConfig.getParameter<vector<int>>("triggerOverlapBase");
	recojetPt_ = iConfig.getParameter<double>("recojetPt");
	AK8jets_ = iConfig.getParameter<bool>("AK8jets");
	DEBUG_ = iConfig.getParameter<bool>("DEBUG");
}

void TriggerEfficiencies::analyze(const Event& iEvent, const EventSetup& iSetup) {

	Handle<TriggerResults> triggerBits;
	iEvent.getByToken(triggerBits_, triggerBits);
	const TriggerNames &names = iEvent.triggerNames(*triggerBits);

	bool changedConfig = false;
	if (!hltConfig.init(iEvent.getRun(), iSetup, "HLT2", changedConfig)) {
		cout << "Initialization of HLTConfigProvider failed!!" << endl;
		return;
	}
	if (changedConfig){
		std::cout << "the curent menu is " << hltConfig.tableName()  << std::endl;
		triggerBit = -1;
		for (size_t j = 0; j < hltConfig.triggerNames().size(); j++) {
			std::cout << TString(hltConfig.triggerNames()[j]) << std::endl;
			if (TString(hltConfig.triggerNames()[j]).Contains(baseTrigger_)) triggerBit = j;
		}
		std::cout << triggerBit << std::endl;
		if (triggerBit == -1) cout << "HLT path not found" << endl;

	}

	bool basedtriggerPass = checkTriggerBitsMiniAOD( names, triggerBits, baseTrigger_ );
	bool triggerPass = checkORListOfTriggerBitsMiniAOD( names, triggerBits, triggerPass_ );
    LogWarning("trigger") << basedtriggerPass << " " << triggerPass;

    if (triggerPass) {


//    edm::Handle<trigger::TriggerEvent> trigEvent;
//	iEvent.getByToken(triggerObjectsToken ,trigEvent);
//	edm::InputTag trigEventTag("hltTriggerSummaryAOD","","HLT");
//	std::string filterName("hltPFHT900Jet30");
//	trigger::size_type filterIndex = trigEvent->filterIndex( edm::InputTag(filterName,"",trigEventTag.process()));
//	//trigger::size_type filterIndex = trigEvent->filterIndex( filterName ); // edm::InputTag(filterName,"",trigEventTag.process()));
//	LogWarning("test") << "trigger filterIndex " << filterIndex << " " <<  trigEvent->sizeFilters() ;
//	if( filterIndex < trigEvent->sizeFilters() ){
//
//		const trigger::Keys& trigKeys = trigEvent->filterKeys(filterIndex);
//		const trigger::TriggerObjectCollection & trigObjColl(trigEvent->getObjects());
//		//now loop of the trigger objects passing filter
//		for( trigger::Keys::const_iterator keyIt=trigKeys.begin(); keyIt!=trigKeys.end(); ++keyIt ){
//			const trigger::TriggerObject& obj = trigObjColl[*keyIt];
//			LogWarning("trigger obj") << obj.pt() << " " << obj.eta();
//		}
//	}
        /*edm::Handle<reco::PFJetCollection> htHLT;
        iEvent.getByToken(hltObject_, htHLT);
        for(const reco::PFJet &iht : *htHLT){
            LogWarning("HT") << iht.pt();
        }*/
//	edm::Handle<trigger::TriggerFilterObjectWithRefs> hltObject;
//	iEvent.getByToken(hltObject_,hltObject);	
//	std::vector<reco::METRef> jetRefVec;
//	//jetRefVec.clear();
//	hltObject->getObjects( trigger::TriggerMET, jetRefVec);
//
//    if ((!jetRefVec.empty())) {
//
//	//for(auto iCalo : jetRefVec) {  
//    for (unsigned i = 0; i < jetRefVec.size(); ++i) {
//        for (unsigned j = 0; j < jetRefVec[i]->size(); ++i) {
//    //for ( auto const& hltJet : jetRefVec ) {
//        //const reco::MET& test = jetRefVec[i];
//		//LogWarning("HT") << test.pt();
//		//LogWarning("HT") << hltJet.Pt(); 
//		LogWarning("HT") << (*jetRefVec[i])[j].size(); 
//	}
//    }
//        Handle<trigger::TriggerFilterObjectWithRefs> hltObject;
//        iEvent.getByToken(hltObject_, hltObject);
//
//        ///// HLT OBJECTS
//        double hltHT = 0;
//        int numHLTJets = 0;
//        double hltHTpt10 = 0;
//        int numHLTJetspt10 = 0;
//        
//        for ( auto const& hltJet : *hltObject ) {
//
//            // first cleaning since we can not go lower in pt anyway
//            if( hltJet.pt() < 5 ) continue;
//            if( TMath::Abs(hltJet.eta()) > 2.5 ) continue;
//            if (DEBUG_) LogWarning("trigger jet") << "\tTrigger object Pt:  pt " << hltJet.pt()  << ", eta " << hltJet.eta() << ", phi " << hltJet.phi() << ", mass " << hltJet.mass();
//
//            // plotting for the basic case
//            hltHT += hltJet.pt();
//            numHLTJets+=1;
//            histos1D_[ "hltJetPt" ]->Fill( hltJet.pt() );
//            histos1D_[ "hltJetEta" ]->Fill( hltJet.eta() );
//
//            /* dont remove it since it can help for later	
//                if (DEBUG_) LogWarning("trigger mass") << "\tTrigger object Mass:  pt " << obj.pt() << ", eta " << obj.eta() << ", phi " << obj.phi() << ", mass " << obj.mass(); 
//                hltMass = obj.mass();
//                histos1D_[ "hltJetMass" ]->Fill( hltMass );
//            }*/
//            if( hltJet.pt() < recojetPt_ ) continue;
//            hltHTpt10 += hltJet.pt();
//            numHLTJetspt10+=1;
//            histos1D_[ "hltJetPt_pt10" ]->Fill( hltJet.pt() );
//            histos1D_[ "hltJetEta_pt10" ]->Fill( hltJet.eta() );
//
//        }
//
//        //histos2D_[ "hltJetPtvsMass" ]->Fill( hltPt, hltMass );
//        histos1D_[ "hltJetHT" ]->Fill( hltHT );
//        histos1D_[ "hltnumJets" ]->Fill( numHLTJets );
//        histos1D_[ "hltJetHT_pt10" ]->Fill( hltHTpt10 );
//        histos1D_[ "hltnumJets_pt10" ]->Fill( numHLTJetspt10 );
//	    
//    /////////////////////////////////////////////////////////////////////////////////
//    /// This is for recoJets
//    double HT = 0;
//    int k = 0;
//        //Handle<PFJetCollection> recojets;
//        Handle<std::vector<pat::Jet>> recojets;
//        iEvent.getByToken(recoJetToken_, recojets);
//    vector<matchedJet> matchJets;
//    for (const auto &recojet : *recojets) {
//
//        if( recojet.pt() < recojetPt_ ) continue;
//        if( TMath::Abs( recojet.eta()) > 2.5 ) continue;
//        if (DEBUG_) LogWarning("recojets") << recojet.pt();
//        //bool ifGenJet = recojet.genJet();
//        //const reco::GenJet* genjet = recojet.genJet();
//        //if(ifGenJet) LogWarning("genjets") << genjet->pt();
//        double dummyMin= 9999;
//        int minInd = -1;
//        for (size_t i=0; i< hltObject->size(); i++) {
//            double deltaRrecohlt = deltaR( (*hltObject)[i], recojet );
//            if ( deltaRrecohlt < dummyMin ) { 
//                dummyMin = deltaRrecohlt;
//                minInd = i;
//            }
//        }
//        if (dummyMin<.3){
//            matchedJet tmpJet;
//            tmpJet.recoJet = recojet;
//            tmpJet.hltJet = (*hltObject)[minInd];
//            matchJets.push_back(tmpJet);
//        }
//
//        HT += recojet.pt();
//
//        if (++k==1){
//            histos1D_[ "jet1Pt" ]->Fill( recojet.pt() );
//        }
//
//    }
//    histos1D_[ "HT" ]->Fill( HT );
//    
//    histos1D_[ "HTDenom" ]->Fill( HT );
//    histos1D_[ "HTPassing" ]->Fill( HT );
//    if ( hltHT > 700 ) histos1D_[ "HTPassingHT700" ]->Fill( HT );
//    if ( hltHT > 800 ) histos1D_[ "HTPassingHT800" ]->Fill( HT );
//    if ( hltHT > 850 ) histos1D_[ "HTPassingHT850" ]->Fill( HT );
//    if ( hltHT > 900 ) histos1D_[ "HTPassingHT900" ]->Fill( HT );
//    if ( hltHT > 950 ) histos1D_[ "HTPassingHT950" ]->Fill( HT );
//    if ( hltHT > 1000 ) histos1D_[ "HTPassingHT1000" ]->Fill( HT );
//    if ( hltHT > 1050 ) histos1D_[ "HTPassingHT1050" ]->Fill( HT );
//
//    /// for HT with pt >10
//    if ( hltHTpt10 > 700 ) histos1D_[ "HTPassingHT700pt10" ]->Fill( HT );
//    if ( hltHTpt10 > 800 ) histos1D_[ "HTPassingHT800pt10" ]->Fill( HT );
//    if ( hltHTpt10 > 850 ) histos1D_[ "HTPassingHT850pt10" ]->Fill( HT );
//    if ( hltHTpt10 > 900 ) histos1D_[ "HTPassingHT900pt10" ]->Fill( HT );
//    if ( hltHTpt10 > 950 ) histos1D_[ "HTPassingHT950pt10" ]->Fill( HT );
//    if ( hltHTpt10 > 1000 ) histos1D_[ "HTPassingHT1000pt10" ]->Fill( HT );
//    if ( hltHTpt10 > 1050 ) histos1D_[ "HTPassingHT1050pt10" ]->Fill( HT );
//    /////////////////////////////////////////////////////////////////////////////////
//
//    
//    // Fro efficiency
//    int m = 0;
//    double mhltHT = 0;
//    double mrecoHT = 0;
//    double mgenHT = 0;
//    for (const auto &mjet: matchJets) {
//
//        bool ifGenJet = mjet.recoJet.genJet();
//        //bool ifhltJet = mjet.hltJet;
//        if ( ifGenJet ) {
//            mgenHT += mjet.recoJet.genJet()->pt();
//            mhltHT += mjet.hltJet.pt();
//            mrecoHT += mjet.recoJet.pt();
//            //LogWarning("test") << mjet.recoJet.pt() << " " << mjet.recoJet.genJet()->pt() << " " << mjet.hltJet.pt();
//            if ((m++)==1){
//                histos1D_[ "genjet1Pt" ]->Fill( mjet.recoJet.genJet()->pt() );
//                histos1D_[ "recojet1Pt" ]->Fill( mjet.recoJet.pt() );
//                histos1D_[ "hltjet1Pt" ]->Fill( mjet.hltJet.pt() );
//                
//                double resGenhlt = mjet.hltJet.pt() / mjet.recoJet.genJet()->pt();
//                double resGenreco = mjet.recoJet.pt() / mjet.recoJet.genJet()->pt();
//                histos1D_[ "genrecojet1Ptreso" ]->Fill( resGenreco );
//                histos2D_[ "genrecojet1PtresovsgenPt" ]->Fill( mjet.recoJet.genJet()->pt(), resGenreco );
//                histos2D_[ "genrecojet1PtresovsrecoPt" ]->Fill( mjet.recoJet.pt(), resGenreco );
//                histos1D_[ "genhltjet1Ptreso" ]->Fill( resGenhlt );
//                histos2D_[ "genhltjet1PtresovsgenPt" ]->Fill( mjet.recoJet.genJet()->pt(), resGenhlt );
//                histos2D_[ "genhltjet1PtresovsrecoPt" ]->Fill( mjet.hltJet.pt(), resGenhlt );
//            }
//
//            histos1D_[ "genjetHT" ]->Fill( mgenHT );
//            histos1D_[ "recojetHT" ]->Fill( mrecoHT );
//            histos1D_[ "hltjetHT" ]->Fill( mhltHT );
//
//            double resGenhltHT = mhltHT/mgenHT;
//            double resGenrecoHT = mrecoHT/mgenHT;
//            histos1D_[ "genrecojet1HTreso" ]->Fill( resGenrecoHT );
//            histos2D_[ "genrecojet1HTresovsgenHT" ]->Fill( mgenHT, resGenrecoHT );
//            histos2D_[ "genrecojet1HTresovsrecoHT" ]->Fill( mrecoHT, resGenrecoHT );
//            histos1D_[ "genhltjet1HTreso" ]->Fill( resGenhltHT );
//            histos2D_[ "genhltjet1HTresovsgenHT" ]->Fill( mgenHT, resGenhltHT );
//            histos2D_[ "genhltjet1HTresovsrecoHT" ]->Fill( mrecoHT, resGenhltHT );
//        }
//
//    }

    }
}

void TriggerEfficiencies::beginJob() {

	histos1D_[ "hltJetPt" ] = fs_->make< TH1D >( "hltJetPt", "hltJetPt", 2000, 0., 2000. );
	//histos1D_[ "hltJetMass" ] = fs_->make< TH1D >( "hltJetMass", "hltJetMass", 2000, 0., 2000. );
	histos1D_[ "hltJetEta" ] = fs_->make< TH1D >( "hltJetEta", "hltJetEta", 100, -5, 5 );
	histos1D_[ "hltJetHT" ] = fs_->make< TH1D >( "hltJetHT", "hltJetHT", 5000, 0., 5000. );
	histos1D_[ "hltnumJets" ] = fs_->make< TH1D >( "hltnumJets", "hltnumJets", 20, 0., 20. );

	histos1D_[ "hltJetPt_pt10" ] = fs_->make< TH1D >( "hltJetPt_pt10", "hltJetPt_pt10", 2000, 0., 2000. );
	histos1D_[ "hltJetEta_pt10" ] = fs_->make< TH1D >( "hltJetEta_pt10", "hltJetEta_pt10", 100, -5, 5 );
	histos1D_[ "hltJetHT_pt10" ] = fs_->make< TH1D >( "hltJetHT_pt10", "hltJetHT_pt10", 5000, 0., 5000. );
	histos1D_[ "hltnumJets_pt10" ] = fs_->make< TH1D >( "hltnumJets_pt10", "hltnumJets_pt10", 20, 0., 20. );

	//histos2D_[ "hltJetPtvsMass" ] = fs_->make< TH2D >( "hltJetPtvsMass", "hltJetPtvsMass", 2000, 0., 2000., 2000, 0., 2000. );

	histos1D_[ "jet1Pt" ] = fs_->make< TH1D >( "jet1Pt", "jet1Pt", 1000, 0., 1000. );
	histos1D_[ "HT" ] = fs_->make< TH1D >( "HT", "HT", 100, 0., 2000. );


	histos1D_[ "HTDenom" ] = fs_->make< TH1D >( "HTDenom", "HTDenom", 2000, 0., 2000. );
	histos1D_[ "HTPassing" ] = fs_->make< TH1D >( "HTPassing", "HTPassing", 2000, 0., 2000. );
	histos1D_[ "HTPassingHT700" ] = fs_->make< TH1D >( "HTPassingHT700", "HTPassingHT700", 2000, 0., 2000. );
	histos1D_[ "HTPassingHT800" ] = fs_->make< TH1D >( "HTPassingHT800", "HTPassingHT800", 2000, 0., 2000. );
	histos1D_[ "HTPassingHT850" ] = fs_->make< TH1D >( "HTPassingHT850", "HTPassingHT850", 2000, 0., 2000. );
	histos1D_[ "HTPassingHT900" ] = fs_->make< TH1D >( "HTPassingHT900", "HTPassingHT900", 2000, 0., 2000. );
	histos1D_[ "HTPassingHT950" ] = fs_->make< TH1D >( "HTPassingHT950", "HTPassingHT950", 2000, 0., 2000. );
	histos1D_[ "HTPassingHT1000" ] = fs_->make< TH1D >( "HTPassingHT1000", "HTPassingHT1000", 2000, 0., 2000. );
	histos1D_[ "HTPassingHT1050" ] = fs_->make< TH1D >( "HTPassingHT1050", "HTPassingHT1050", 2000, 0., 2000. );

	histos1D_[ "HTPassingHT700pt10" ] = fs_->make< TH1D >( "HTPassingHT700pt10", "HTPassingHT700pt10", 2000, 0., 2000. );
	histos1D_[ "HTPassingHT800pt10" ] = fs_->make< TH1D >( "HTPassingHT800pt10", "HTPassingHT800pt10", 2000, 0., 2000. );
	histos1D_[ "HTPassingHT850pt10" ] = fs_->make< TH1D >( "HTPassingHT850pt10", "HTPassingHT850pt10", 2000, 0., 2000. );
	histos1D_[ "HTPassingHT900pt10" ] = fs_->make< TH1D >( "HTPassingHT900pt10", "HTPassingHT900pt10", 2000, 0., 2000. );
	histos1D_[ "HTPassingHT950pt10" ] = fs_->make< TH1D >( "HTPassingHT950pt10", "HTPassingHT950pt10", 2000, 0., 2000. );
	histos1D_[ "HTPassingHT1000pt10" ] = fs_->make< TH1D >( "HTPassingHT1000pt10", "HTPassingHT1000pt10", 2000, 0., 2000. );
	histos1D_[ "HTPassingHT1050pt10" ] = fs_->make< TH1D >( "HTPassingHT1050pt10", "HTPassingHT1050pt10", 2000, 0., 2000. );

	histos1D_[ "genjet1Pt" ] = fs_->make< TH1D >( "genjet1Pt", "genjet1Pt", 2000, 0., 2000. );
	histos1D_[ "recojet1Pt" ] = fs_->make< TH1D >( "recojet1Pt", "recojet1Pt", 2000, 0., 2000. );
	histos1D_[ "hltjet1Pt" ] = fs_->make< TH1D >( "hltjet1Pt", "hltjet1Pt", 2000, 0., 2000. );
	histos1D_[ "genjetHT" ] = fs_->make< TH1D >( "genjetHT", "genjetHT", 2000, 0., 2000. );
	histos1D_[ "recojetHT" ] = fs_->make< TH1D >( "recojetHT", "recojetHT", 2000, 0., 2000. );
	histos1D_[ "hltjetHT" ] = fs_->make< TH1D >( "hltjetHT", "hltjetHT", 2000, 0., 2000. );
	histos1D_[ "genrecojet1Ptreso" ] = fs_->make< TH1D >( "genrecojet1Ptreso", "genrecojet1Ptreso", 40, 0., 10. );
	histos2D_[ "genrecojet1PtresovsrecoPt" ] = fs_->make< TH2D >( "genrecojet1PtresovsrecoPt", "genrecojet1PtresovsrecoPt", 2000, 0., 2000., 40, 0., 10. );
	histos2D_[ "genrecojet1PtresovsgenPt" ] = fs_->make< TH2D >( "genrecojet1PtresovsgenPt", "genrecojet1PtresovsgenPt", 2000, 0., 2000., 40, 0., 10. );
	histos1D_[ "genhltjet1Ptreso" ] = fs_->make< TH1D >( "genhltjet1Ptreso", "genhltjet1Ptreso", 40, 0., 10. );
	histos2D_[ "genhltjet1PtresovsrecoPt" ] = fs_->make< TH2D >( "genhltjet1PtresovsrecoPt", "genhltjet1PtresovsrecoPt", 2000, 0., 2000., 40, 0., 10. );
	histos2D_[ "genhltjet1PtresovsgenPt" ] = fs_->make< TH2D >( "genhltjet1PtresovsgenPt", "genhltjet1PtresovsgenPt", 2000, 0., 2000., 40, 0., 10. );
	histos1D_[ "genrecojet1HTreso" ] = fs_->make< TH1D >( "genrecojet1HTreso", "genrecojet1HTreso", 40, 0., 10. );
	histos2D_[ "genrecojet1HTresovsrecoHT" ] = fs_->make< TH2D >( "genrecojet1HTresovsrecoHT", "genrecojet1HTresovsrecoHT", 2000, 0., 2000., 40, 0., 10. );
	histos2D_[ "genrecojet1HTresovsgenHT" ] = fs_->make< TH2D >( "genrecojet1HTresovsgenHT", "genrecojet1HTresovsgenHT", 2000, 0., 2000., 40, 0., 10. );
	histos1D_[ "genhltjet1HTreso" ] = fs_->make< TH1D >( "genhltjet1HTreso", "genhltjet1HTreso", 40, 0., 10. );
	histos2D_[ "genhltjet1HTresovsrecoHT" ] = fs_->make< TH2D >( "genhltjet1HTresovsrecoHT", "genhltjet1HTresovsrecoHT", 2000, 0., 2000., 40, 0., 10. );
	histos2D_[ "genhltjet1HTresovsgenHT" ] = fs_->make< TH2D >( "genhltjet1HTresovsgenHT", "genhltjet1HTresovsgenHT", 2000, 0., 2000., 40, 0., 10. );

	///// Sumw2 all the histos
	for( auto const& histo : histos1D_ ) histos1D_[ histo.first ]->Sumw2();
	for( auto const& histo : histos2D_ ) histos2D_[ histo.first ]->Sumw2();
}

void TriggerEfficiencies::fillDescriptions(ConfigurationDescriptions & descriptions) {

	ParameterSetDescription desc;
	desc.add<InputTag>("bits", 	InputTag("TriggerResults", "", "HLT2"));
	desc.add<InputTag>("hltObject", 	InputTag("slimmedPatTrigger"));
	desc.add<string>("baseTrigger", 	"HLT_PFHT800");
	desc.add<double>("recojetPt", 	50);
	desc.add<bool>("AK8jets", 	true);
	desc.add<bool>("DEBUG", 	false);
	desc.add<InputTag>("recoJets", 	InputTag("slimmedJetsAK8"));
	vector<string> HLTPass;
	HLTPass.push_back("HLT_AK8PFHT650_TrimR0p1PT0p03Mass50");
	desc.add<vector<string>>("triggerPass",	HLTPass);
	vector<int> HLTOverlapBase;
	HLTOverlapBase.push_back(0);
	desc.add<vector<int>>("triggerOverlapBase",	HLTOverlapBase);
	vector<int> HLTOverlap;
	HLTOverlap.push_back(0);
	desc.add<vector<int>>("triggerOverlap",	HLTOverlap);
	descriptions.addDefault(desc);
}
      
//define this as a plug-in
DEFINE_FWK_MODULE(TriggerEfficiencies);
