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
#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"

#include "PUHLT/PUmitigationatHLT/interface/CommonVariablesStructure.h"

using namespace edm;
using namespace std;
using namespace reco;
using namespace pat;

class TriggerEfficienciesfromMenu : public EDAnalyzer {

	public:
		explicit TriggerEfficienciesfromMenu(const ParameterSet&);
      		static void fillDescriptions(ConfigurationDescriptions & descriptions);
		~TriggerEfficienciesfromMenu() {}

	private:
		virtual void analyze(const Event&, const EventSetup&) override;
      		virtual void beginJob() override;

	EDGetTokenT<TriggerResults> triggerBits_;
	EDGetTokenT<JetCollection> recojetToken_;
	string baseTrigger_;
    vector<string> triggerPass_;
    vector<int> triggerOverlap_;
    vector<int> triggerOverlapBase_;
	double recojetPt_;
	double recojetEta_;
	bool AK8jets_;
	bool DEBUG_;
	HLTConfigProvider hltConfig;
	int triggerBit;

	Service<TFileService> fs_;
	map< string, TH1D* > histos1D_;
	map< string, TH2D* > histos2D_;
};

TriggerEfficienciesfromMenu::TriggerEfficienciesfromMenu(const ParameterSet& iConfig):
	triggerBits_(consumes<TriggerResults>(iConfig.getParameter<InputTag>("bits"))),
	recojetToken_(consumes<JetCollection>(iConfig.getParameter<InputTag>("recojets")))
{
	baseTrigger_ = iConfig.getParameter<string>("baseTrigger");
	triggerPass_ = iConfig.getParameter<vector<string>>("triggerPass");
	triggerOverlap_ = iConfig.getParameter<vector<int>>("triggerOverlap");
	triggerOverlapBase_ = iConfig.getParameter<vector<int>>("triggerOverlapBase");
	recojetPt_ = iConfig.getParameter<double>("recojetPt");
	recojetEta_ = iConfig.getParameter<double>("recojetEta");
	AK8jets_ = iConfig.getParameter<bool>("AK8jets");
	DEBUG_ = iConfig.getParameter<bool>("DEBUG");
}

void TriggerEfficienciesfromMenu::analyze(const Event& iEvent, const EventSetup& iSetup) {

    // Accesing the menus
	Handle<TriggerResults> triggerBits;
	iEvent.getByToken(triggerBits_, triggerBits);
	const TriggerNames &names = iEvent.triggerNames(*triggerBits);

    if (DEBUG_){
        //just checking if HLT2 menu was run
        bool changedConfig = false;
        if (!hltConfig.init(iEvent.getRun(), iSetup, "HLT2", changedConfig)) {
            cout << "Initialization of HLTConfigProvider failed!!" << endl;
            return;
        }
        // Printing the menu
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
    }

    // Checking if triggers were fired
	bool baseTrigger = checkTriggerBitsMiniAOD( names, triggerBits, baseTrigger_ );
	bool ORTriggers = checkORListOfTriggerBitsMiniAOD( names, triggerBits, triggerPass_ );
	if (DEBUG_) LogWarning("trigger fired") << "Based " << baseTrigger << " OR " << ORTriggers;
	if ( TString(baseTrigger_).Contains("empty") ) baseTrigger = true;


    if ( baseTrigger || ORTriggers ) { // checking if both triggers passed

        /////////////////////////////////////////////////////////////////////////////////
        /// This is for recojets
        Handle<JetCollection> recojets;
        iEvent.getByToken(recojetToken_, recojets);

        double recoHT = 0;
        int numRecoJets = 0;
        for (const pat::Jet &recojet : *recojets) {

            if( recojet.pt() < recojetPt_ ) continue;
            if( TMath::Abs(recojet.eta()) > recojetEta_ ) continue;
            if (DEBUG_) LogWarning("recojets") << recojet.pt();
            recoHT += recojet.pt();

            if (++numRecoJets==1){
                histos1D_[ "recoLeadJetPt" ]->Fill( recojet.pt() );
                histos1D_[ "recoLeadJetEta" ]->Fill( recojet.eta() );

                if ( baseTrigger ) {
                    histos1D_[ "recoLeadJetPtDenom" ]->Fill( recojet.pt() );
                    histos1D_[ "recoLeadJetEtaDenom" ]->Fill( recojet.eta() );
                    if ( ORTriggers ) {
                        histos1D_[ "recoLeadJetPtPassing" ]->Fill( recojet.pt() );
                        histos1D_[ "recoLeadJetEtaPassing" ]->Fill( recojet.eta() );
                    }
                }
            }

        }
        histos1D_[ "recoHT" ]->Fill( recoHT );
        histos1D_[ "numRecoJets" ]->Fill( numRecoJets );
        
        if ( baseTrigger ) {
            histos1D_[ "recoHTDenom" ]->Fill( recoHT );
            if ( ORTriggers ) histos1D_[ "recoHTPassing" ]->Fill( recoHT );
        }
    }
	////////////////////////////////////////////////////////////////////////////////////////////////////

    /*///// checking overlap, dont remove it, it will help later
    if ( triggerOverlapBase_[0] != triggerOverlap_[0] ) {

        bool overlapBase = 0;
        vector<bool> vectorOverlapBase;
        for (size_t b = 0; b < triggerOverlapBase_.size(); b++) {
            vectorOverlapBase.push_back( triggersFired[ triggerOverlapBase_[b] ] );	
            //LogWarning("base") <<  triggerOverlapBase_[b] << " " << triggersFired[triggerOverlapBase_[b]];
        }
        overlapBase = any_of(vectorOverlapBase.begin(), vectorOverlapBase.end(), [](bool v) { return v; }); 
        //LogWarning("baseAll") <<  overlapBase;

        bool overlap = 0;
        vector<bool> vectorOverlap;
        for (size_t b = 0; b < triggerOverlap_.size(); b++) {
            vectorOverlap.push_back( triggersFired[ triggerOverlap_[b] ] );	
            //LogWarning("no base") <<  triggerOverlap_[b] << " " << triggersFired[triggerOverlap_[b]];
        }
        overlap = any_of(vectorOverlap.begin(), vectorOverlap.end(), [](bool v) { return v; }); 
        //LogWarning("no baseAll") <<  overlap;
        
        if ( overlapBase && overlap ) {
            histos2D_[ "jet1SDMassvsHT11" ]->Fill( jet1SoftdropMass, HT );
            histos2D_[ "jet1SDMassvsPt11" ]->Fill( jet1SoftdropMass, jet1Pt );
        } else if (!overlapBase && overlap ) {
            histos2D_[ "jet1SDMassvsHT01" ]->Fill( jet1SoftdropMass, HT );
            histos2D_[ "jet1SDMassvsPt01" ]->Fill( jet1SoftdropMass, jet1Pt );
        } else if (overlapBase && !overlap ) {
            histos2D_[ "jet1SDMassvsHT10" ]->Fill( jet1SoftdropMass, HT );
            histos2D_[ "jet1SDMassvsPt10" ]->Fill( jet1SoftdropMass, jet1Pt );
        } else { 
            histos2D_[ "jet1SDMassvsHT00" ]->Fill( jet1SoftdropMass, HT );
            histos2D_[ "jet1SDMassvsPt00" ]->Fill( jet1SoftdropMass, jet1Pt );
        }
    }*/
}

void TriggerEfficienciesfromMenu::beginJob() {

    /////////////////////////////////////////////////////////////
	histos1D_[ "recoLeadJetPt" ] = fs_->make< TH1D >( "recoLeadJetPt", "recoLeadJetPt", 1000, 0., 1000. );
	histos1D_[ "recoLeadJetEta" ] = fs_->make< TH1D >( "recoLeadJetEta", "recoLeadJetEta", 100, -5., 5. );
	histos1D_[ "recoHT" ] = fs_->make< TH1D >( "recoHT", "recoHT", 100, 0., 2000. );
	histos1D_[ "numRecoJets" ] = fs_->make< TH1D >( "numRecoJets", "numRecoJets", 20, 0., 20. );


	histos1D_[ "recoLeadJetPtDenom" ] = fs_->make< TH1D >( "recoLeadJetPtDenom", "recoLeadJetPtDenom", 1000, 0., 1000. );
	histos1D_[ "recoLeadJetPtPassing" ] = fs_->make< TH1D >( "recoLeadJetPtPassing", "recoLeadJetPtPassing", 1000, 0., 1000. );
	histos1D_[ "recoLeadJetEtaDenom" ] = fs_->make< TH1D >( "recoLeadJetEtaDenom", "recoLeadJetEtaDenom", 100, -5., 5. );
	histos1D_[ "recoLeadJetEtaPassing" ] = fs_->make< TH1D >( "recoLeadJetEtaPassing", "recoLeadJetEtaPassing", 100, -5., 5. );
	histos1D_[ "recoHTDenom" ] = fs_->make< TH1D >( "recoHTDenom", "recoHTDenom", 2000, 0., 2000. );
	histos1D_[ "recoHTPassing" ] = fs_->make< TH1D >( "recoHTPassing", "recoHTPassing", 2000, 0., 2000. );


	///// Sumw2 all the histos
	for( auto const& histo : histos1D_ ) histos1D_[ histo.first ]->Sumw2();
	for( auto const& histo : histos2D_ ) histos2D_[ histo.first ]->Sumw2();
}

void TriggerEfficienciesfromMenu::fillDescriptions(ConfigurationDescriptions & descriptions) {

	ParameterSetDescription desc;
	desc.add<InputTag>("bits", 	InputTag("TriggerResults", "", "HLT2"));
	desc.add<string>("baseTrigger", 	"HLT_PFHT800");
	desc.add<double>("recojetEta", 	2.5);
	desc.add<double>("recojetPt", 	10);
	desc.add<bool>("AK8jets", 	true);
	desc.add<bool>("DEBUG", 	false);
	desc.add<InputTag>("recojets", 	InputTag("slimmedJets"));
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
DEFINE_FWK_MODULE(TriggerEfficienciesfromMenu);
