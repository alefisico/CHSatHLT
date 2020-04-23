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


class TriggerResponses : public EDAnalyzer {

	public:
		explicit TriggerResponses(const ParameterSet&);
      	static void fillDescriptions(ConfigurationDescriptions & descriptions);
		~TriggerResponses() {}

	private:
		virtual void analyze(const Event&, const EventSetup&) override;
      	virtual void beginJob() override;

	EDGetTokenT<TriggerResults> triggerBits_;
	//EDGetTokenT<trigger::TriggerFilterObjectWithRefs> triggerObjects_;
	EDGetTokenT<PFJetCollection> triggerObjects_;
	EDGetTokenT<JetCollection> recojetToken_;
	EDGetTokenT<GenJetCollection> genjetToken_;
	string baseTrigger_;
	double hltjetPt_;
	double recojetPt_;
	double hltjetEta_;
	double recojetEta_;
	bool AK8jets_;
	bool DEBUG_;
	HLTConfigProvider hltConfig;
	int triggerBit;

	Service<TFileService> fs_;
	map< string, TH1D* > histos1D_;
	map< string, TH2D* > histos2D_;
};

TriggerResponses::TriggerResponses(const ParameterSet& iConfig):
	triggerBits_(consumes<TriggerResults>(iConfig.getParameter<InputTag>("bits"))),
	triggerObjects_(consumes<PFJetCollection>(iConfig.getParameter<InputTag>("objects"))),
	recojetToken_(consumes<JetCollection>(iConfig.getParameter<InputTag>("recojets"))),
	genjetToken_(consumes<GenJetCollection>(iConfig.getParameter<InputTag>("genjets")))
{
	baseTrigger_ = iConfig.getParameter<string>("baseTrigger");
	hltjetPt_ = iConfig.getParameter<double>("hltjetPt");
	recojetPt_ = iConfig.getParameter<double>("recojetPt");
	hltjetEta_ = iConfig.getParameter<double>("hltjetEta");
	recojetEta_ = iConfig.getParameter<double>("recojetEta");
	AK8jets_ = iConfig.getParameter<bool>("AK8jets");
	DEBUG_ = iConfig.getParameter<bool>("DEBUG");
}

void TriggerResponses::analyze(const Event& iEvent, const EventSetup& iSetup) {

    // Accesing the menus
	Handle<TriggerResults> triggerBits;
	iEvent.getByToken(triggerBits_, triggerBits);
	const TriggerNames &names = iEvent.triggerNames(*triggerBits);
    // Checking if triggers were fired
	bool baseTrigger = checkTriggerBitsMiniAOD( names, triggerBits, baseTrigger_ );

	///// HLT OBJECTS
	if ( baseTrigger ) { // because not every event has triggerObjects

        Handle<PFJetCollection> triggerObjects;
        iEvent.getByToken(triggerObjects_, triggerObjects);

		double hltHT = 0;
        int numHLTJets = 0;
        
        PFJetCollection triggerJets;
        for ( auto const& triggerJet : *triggerObjects ) {

            // first cleaning since we can not go lower in pt anyway
			if( triggerJet.pt() < hltjetPt_ ) continue;
			if( TMath::Abs(triggerJet.eta()) > hltjetEta_ ) continue;
            if (DEBUG_) LogWarning("trigger jet") << "\tTrigger object Pt:  pt " << triggerJet.pt()  << ", eta " << triggerJet.eta() << ", phi " << triggerJet.phi() << ", mass " << triggerJet.mass();
        
            // plotting the basic distributions
            histos1D_[ "hltJetsPt" ]->Fill( triggerJet.pt() );
            histos1D_[ "hltJetsEta" ]->Fill( triggerJet.eta() );

            if (++numHLTJets==1){
                histos1D_[ "hltLeadJetPt" ]->Fill( triggerJet.pt() );
                histos1D_[ "hltLeadJetEta" ]->Fill( triggerJet.eta() );
            }
            hltHT += triggerJet.pt();
            triggerJets.push_back( triggerJet );

            // Checking matching with genJets
            Handle<GenJetCollection> genJets;
            iEvent.getByToken(genjetToken_, genJets);

            double dummyMin= 9999;
            for ( auto const& genJet : *genJets ) {
                double deltaRrecohlt = deltaR( genJet, triggerJet );
                if ( deltaRrecohlt < dummyMin ) { 
                    dummyMin = deltaRrecohlt;
                }
            }
            if (dummyMin>0.4) {
                histos1D_[ "hltPUJetsPt" ]->Fill( triggerJet.pt() );
                histos1D_[ "hltPUJetsEta" ]->Fill( triggerJet.eta() );
            }
        }

        //histos2D_[ "hltJetPtvsMass" ]->Fill( hltPt, hltMass );
        histos1D_[ "hltnumJets" ]->Fill( numHLTJets );
	    

        /////////////////////////////////////////////////////////////////////////////////
        /// This is for recojets
        Handle<JetCollection> recojets;
        iEvent.getByToken(recojetToken_, recojets);

        double recoHT = 0;
        double genHT = 0;
        int numRecoJets = 0;
        for (unsigned i = 0; i < recojets->size(); ++i) {

            const pat::Jet &recojet = (*recojets)[i];
            if( recojet.pt() < recojetPt_ ) continue;
            if( TMath::Abs(recojet.eta()) > recojetEta_ ) continue;
            if (DEBUG_) LogWarning("recojets") << recojet.pt();
            recoHT += recojet.pt();
            bool ifGenJet = recojet.genJet();

            histos1D_[ "recoJetsPt" ]->Fill( recojet.pt() );
            histos1D_[ "recoJetsEta" ]->Fill( recojet.eta() );
            if (++numRecoJets==1){
                histos1D_[ "recoLeadJetPt" ]->Fill( recojet.pt() );
                histos1D_[ "recoLeadJetEta" ]->Fill( recojet.eta() );
            }

            if ( ifGenJet ) {

                genHT += recojet.genJet()->pt();
                histos1D_[ "genJetsPt" ]->Fill( recojet.genJet()->pt() );
                histos1D_[ "genJetsEta" ]->Fill( recojet.genJet()->eta() );
                if (numRecoJets==1){
                    histos1D_[ "genLeadJetPt" ]->Fill( recojet.genJet()->pt() );
                    histos1D_[ "genLeadJetEta" ]->Fill( recojet.genJet()->eta() );
                }
                
                double responseRecoPt = recojet.pt() / recojet.genJet()->pt();
                histos1D_[ "respRecoJetsPt" ]->Fill( responseRecoPt );
                histos2D_[ "respRecoJetsPtvsGen" ]->Fill( recojet.genJet()->pt(), responseRecoPt );
                histos2D_[ "respRecoJetsPtvsReco" ]->Fill( recojet.pt(), responseRecoPt );
                if (numRecoJets==1){
                    histos1D_[ "respRecoLeadJetPt" ]->Fill( responseRecoPt );
                    histos2D_[ "respRecoLeadJetPtvsGen" ]->Fill( recojet.genJet()->pt(), responseRecoPt );
                    histos2D_[ "respRecoLeadJetPtvsReco" ]->Fill( recojet.pt(), responseRecoPt );
                }

                double responseRecoEta = recojet.eta() / recojet.genJet()->eta();
                histos1D_[ "respRecoJetsEta" ]->Fill( responseRecoEta );
                histos2D_[ "respRecoJetsEtavsGen" ]->Fill( recojet.genJet()->eta(), responseRecoEta );
                histos2D_[ "respRecoJetsEtavsReco" ]->Fill( recojet.eta(), responseRecoEta );
                if (numRecoJets==1){
                    histos1D_[ "respRecoLeadJetEta" ]->Fill( responseRecoEta );
                    histos2D_[ "respRecoLeadJetEtavsGen" ]->Fill( recojet.genJet()->eta(), responseRecoEta );
                    histos2D_[ "respRecoLeadJetEtavsReco" ]->Fill( recojet.eta(), responseRecoEta );
                }

                if (triggerJets.size()>=i){
                
                    double responseHLTPt = (triggerJets)[i].pt() / recojet.genJet()->pt();
                    histos1D_[ "respHLTJetsPt" ]->Fill( responseHLTPt );
                    histos2D_[ "respHLTJetsPtvsGen" ]->Fill( recojet.genJet()->pt(), responseHLTPt );
                    histos2D_[ "respHLTJetsPtvsHLT" ]->Fill( (triggerJets)[i].pt(), responseHLTPt );
                    if (numHLTJets==1){
                        histos1D_[ "respHLTLeadJetPt" ]->Fill( responseHLTPt );
                        histos2D_[ "respHLTLeadJetPtvsGen" ]->Fill( recojet.genJet()->pt(), responseHLTPt );
                        histos2D_[ "respHLTLeadJetPtvsHLT" ]->Fill( (triggerJets)[i].pt(), responseHLTPt );
                    }

                    double responseHLTEta = (triggerJets)[i].eta() / recojet.genJet()->eta();
                    histos1D_[ "respHLTJetsEta" ]->Fill( responseHLTEta );
                    histos2D_[ "respHLTJetsEtavsGen" ]->Fill( recojet.genJet()->eta(), responseHLTEta );
                    histos2D_[ "respHLTJetsEtavsHLT" ]->Fill( (triggerJets)[i].eta(), responseHLTEta );
                    if (numHLTJets==1){ 
                        histos1D_[ "respHLTLeadJetEta" ]->Fill( responseHLTEta );
                        histos2D_[ "respHLTLeadJetEtavsGen" ]->Fill( recojet.genJet()->eta(), responseHLTEta );
                        histos2D_[ "respHLTLeadJetEtavsHLT" ]->Fill( (triggerJets)[i].eta(), responseHLTEta );
                    }
                }

            } else {
                histos1D_[ "recoPUJetsPt" ]->Fill( recojet.pt() );
                histos1D_[ "recoPUJetsEta" ]->Fill( recojet.eta() );
            }
        }

        histos1D_[ "hltHT" ]->Fill( hltHT );
        histos1D_[ "recoHT" ]->Fill( recoHT );
        if (genHT>0) {
            histos1D_[ "genHT" ]->Fill( genHT );

            double responseRecoHT = recoHT / genHT;
            histos1D_[ "respRecoJetsHT" ]->Fill( responseRecoHT );
            histos2D_[ "respRecoJetsHTvsGen" ]->Fill( genHT, responseRecoHT );
            histos2D_[ "respRecoJetsHTvsReco" ]->Fill( recoHT, responseRecoHT );

            double responseHLTHT = hltHT / genHT;
            histos1D_[ "respHLTJetsHT" ]->Fill( responseHLTHT );
            histos2D_[ "respHLTJetsHTvsGen" ]->Fill( genHT, responseHLTHT );
            histos2D_[ "respHLTJetsHTvsHLT" ]->Fill( hltHT, responseHLTHT );

        }
    }
}

void TriggerResponses::beginJob() {

	//histos1D_[ "hltJetMass" ] = fs_->make< TH1D >( "hltJetMass", "hltJetMass", 2000, 0., 2000. );
	histos1D_[ "hltJetsPt" ] = fs_->make< TH1D >( "hltJetsPt", "hltJetsPt", 2000, 0., 2000. );
	histos1D_[ "hltJetsEta" ] = fs_->make< TH1D >( "hltJetsEta", "hltJetsEta", 100, -5, 5 );
	histos1D_[ "hltLeadJetPt" ] = fs_->make< TH1D >( "hltLeadJetPt", "hltLeadJetPt", 2000, 0., 2000. );
	histos1D_[ "hltLeadJetEta" ] = fs_->make< TH1D >( "hltLeadJetEta", "hltLeadJetEta", 100, -5, 5 );
	histos1D_[ "hltHT" ] = fs_->make< TH1D >( "hltHT", "hltHT", 5000, 0., 5000. );
	histos1D_[ "hltnumJets" ] = fs_->make< TH1D >( "hltnumJets", "hltnumJets", 20, 0., 20. );
	histos1D_[ "hltPUJetsPt" ] = fs_->make< TH1D >( "hltPUJetsPt", "hltPUJetsPt", 2000, 0., 2000. );
	histos1D_[ "hltPUJetsEta" ] = fs_->make< TH1D >( "hltPUJetsEta", "hltPUJetsEta", 100, -5, 5 );

	//histos2D_[ "hltJetPtvsMass" ] = fs_->make< TH2D >( "hltJetPtvsMass", "hltJetPtvsMass", 2000, 0., 2000., 2000, 0., 2000. );


    /////////////////////////////////////////////////////////////
	histos1D_[ "recoJetsPt" ] = fs_->make< TH1D >( "recoJetsPt", "recoJetsPt", 1000, 0., 1000. );
	histos1D_[ "recoJetsEta" ] = fs_->make< TH1D >( "recoJetsEta", "recoJetsEta", 100, -5., 5. );
	histos1D_[ "recoLeadJetPt" ] = fs_->make< TH1D >( "recoLeadJetPt", "recoLeadJetPt", 1000, 0., 1000. );
	histos1D_[ "recoLeadJetEta" ] = fs_->make< TH1D >( "recoLeadJetEta", "recoLeadJetEta", 100, -5., 5. );
	histos1D_[ "recoHT" ] = fs_->make< TH1D >( "recoHT", "recoHT", 100, 0., 2000. );
	histos1D_[ "numRecoJets" ] = fs_->make< TH1D >( "numRecoJets", "numRecoJets", 20, 0., 20. );

	histos1D_[ "genJetsPt" ] = fs_->make< TH1D >( "genJetsPt", "genJetsPt", 1000, 0., 1000. );
	histos1D_[ "genJetsEta" ] = fs_->make< TH1D >( "genJetsEta", "genJetsEta", 100, -5., 5. );
	histos1D_[ "genLeadJetPt" ] = fs_->make< TH1D >( "genLeadJetPt", "genLeadJetPt", 1000, 0., 1000. );
	histos1D_[ "genLeadJetEta" ] = fs_->make< TH1D >( "genLeadJetEta", "genLeadJetEta", 100, -5., 5. );

	histos1D_[ "respRecoJetsPt" ] = fs_->make< TH1D >( "respRecoJetsPt", "respRecoJetsPt", 40, 0., 10. );
	histos2D_[ "respRecoJetsPtvsGen" ] = fs_->make< TH2D >( "respRecoJetsPtvsGen", "respRecoJetsPtvsGen", 2000, 0., 2000., 40, 0., 10. );
	histos2D_[ "respRecoJetsPtvsReco" ] = fs_->make< TH2D >( "respRecoJetsPtvsReco", "respRecoJetsPtvsReco", 2000, 0., 2000., 40, 0., 10. );
	histos1D_[ "respRecoJetsEta" ] = fs_->make< TH1D >( "respRecoJetsEta", "respRecoJetsEta", 40, 0., 10. );
	histos2D_[ "respRecoJetsEtavsGen" ] = fs_->make< TH2D >( "respRecoJetsEtavsGen", "respRecoJetsEtavsGen", 100, -5., 5., 40, 0., 10. );
	histos2D_[ "respRecoJetsEtavsReco" ] = fs_->make< TH2D >( "respRecoJetsEtavsReco", "respRecoJetsEtavsReco", 100, -5., 5., 40, 0., 10. );
	histos1D_[ "respRecoLeadJetPt" ] = fs_->make< TH1D >( "respRecoLeadJetPt", "respRecoLeadJetPt", 40, 0., 10. );
	histos2D_[ "respRecoLeadJetPtvsGen" ] = fs_->make< TH2D >( "respRecoLeadJetPtvsGen", "respRecoLeadJetPtvsGen", 2000, 0., 2000., 40, 0., 10. );
	histos2D_[ "respRecoLeadJetPtvsReco" ] = fs_->make< TH2D >( "respRecoLeadJetPtvsReco", "respRecoLeadJetPtvsReco", 2000, 0., 2000., 40, 0., 10. );
	histos1D_[ "respRecoLeadJetEta" ] = fs_->make< TH1D >( "respRecoLeadJetEta", "respRecoLeadJetEta", 40, 0., 10. );
	histos2D_[ "respRecoLeadJetEtavsGen" ] = fs_->make< TH2D >( "respRecoLeadJetEtavsGen", "respRecoLeadJetEtavsGen", 100, -5., 5., 40, 0., 10. );
	histos2D_[ "respRecoLeadJetEtavsReco" ] = fs_->make< TH2D >( "respRecoLeadJetEtavsReco", "respRecoLeadJetEtavsReco", 100, -5., 5., 40, 0., 10. );

	histos1D_[ "respHLTJetsPt" ] = fs_->make< TH1D >( "respHLTJetsPt", "respHLTJetsPt", 40, 0., 10. );
	histos2D_[ "respHLTJetsPtvsGen" ] = fs_->make< TH2D >( "respHLTJetsPtvsGen", "respHLTJetsPtvsGen", 2000, 0., 2000., 40, 0., 10. );
	histos2D_[ "respHLTJetsPtvsHLT" ] = fs_->make< TH2D >( "respHLTJetsPtvsHLT", "respHLTJetsPtvsHLT", 2000, 0., 2000., 40, 0., 10. );
	histos1D_[ "respHLTJetsEta" ] = fs_->make< TH1D >( "respHLTJetsEta", "respHLTJetsEta", 40, 0., 10. );
	histos2D_[ "respHLTJetsEtavsGen" ] = fs_->make< TH2D >( "respHLTJetsEtavsGen", "respHLTJetsEtavsGen", 100, -5., 5., 40, 0., 10. );
	histos2D_[ "respHLTJetsEtavsHLT" ] = fs_->make< TH2D >( "respHLTJetsEtavsHLT", "respHLTJetsEtavsHLT", 100, -5., 5., 40, 0., 10. );
	histos1D_[ "respHLTLeadJetPt" ] = fs_->make< TH1D >( "respHLTLeadJetPt", "respHLTLeadJetPt", 40, 0., 10. );
	histos2D_[ "respHLTLeadJetPtvsGen" ] = fs_->make< TH2D >( "respHLTLeadJetPtvsGen", "respHLTLeadJetPtvsGen", 2000, 0., 2000., 40, 0., 10. );
	histos2D_[ "respHLTLeadJetPtvsHLT" ] = fs_->make< TH2D >( "respHLTLeadJetPtvsHLT", "respHLTLeadJetPtvsHLT", 2000, 0., 2000., 40, 0., 10. );
	histos1D_[ "respHLTLeadJetEta" ] = fs_->make< TH1D >( "respHLTLeadJetEta", "respHLTLeadJetEta", 40, 0., 10. );
	histos2D_[ "respHLTLeadJetEtavsGen" ] = fs_->make< TH2D >( "respHLTLeadJetEtavsGen", "respHLTLeadJetEtavsGen", 100, -5., 5., 40, 0., 10. );
	histos2D_[ "respHLTLeadJetEtavsHLT" ] = fs_->make< TH2D >( "respHLTLeadJetEtavsHLT", "respHLTLeadJetEtavsHLT", 100, -5., 5., 40, 0., 10. );

	histos1D_[ "recoHT" ] = fs_->make< TH1D >( "recoHT", "recoHT", 5000, 0., 5000. );
	histos1D_[ "genHT" ] = fs_->make< TH1D >( "genHT", "genHT", 5000, 0., 5000. );
	histos1D_[ "respRecoJetsHT" ] = fs_->make< TH1D >( "respRecoJetsHT", "respRecoJetsHT", 40, 0., 10. );
	histos2D_[ "respRecoJetsHTvsGen" ] = fs_->make< TH2D >( "respRecoJetsHTvsGen", "respRecoJetsHTvsGen", 5000, 0., 5000., 40, 0., 10. );
	histos2D_[ "respRecoJetsHTvsReco" ] = fs_->make< TH2D >( "respRecoJetsHTvsReco", "respRecoJetsHTvsReco", 5000, 0., 5000., 40, 0., 10. );
	histos1D_[ "respHLTJetsHT" ] = fs_->make< TH1D >( "respHLTJetsHT", "respHLTJetsHT", 40, 0., 10. );
	histos2D_[ "respHLTJetsHTvsGen" ] = fs_->make< TH2D >( "respHLTJetsHTvsGen", "respHLTJetsHTvsGen", 5000, 0., 5000., 40, 0., 10. );
	histos2D_[ "respHLTJetsHTvsHLT" ] = fs_->make< TH2D >( "respHLTJetsHTvsHLT", "respHLTJetsHTvsHLT", 5000, 0., 5000., 40, 0., 10. );

	histos1D_[ "recoPUJetsPt" ] = fs_->make< TH1D >( "recoPUJetsPt", "recoPUJetsPt", 1000, 0., 1000. );
	histos1D_[ "recoPUJetsEta" ] = fs_->make< TH1D >( "recoPUJetsEta", "recoPUJetsEta", 100, -5., 5. );

	///// Sumw2 all the histos
	for( auto const& histo : histos1D_ ) histos1D_[ histo.first ]->Sumw2();
	for( auto const& histo : histos2D_ ) histos2D_[ histo.first ]->Sumw2();
}

void TriggerResponses::fillDescriptions(ConfigurationDescriptions & descriptions) {

	ParameterSetDescription desc;
	desc.add<InputTag>("bits", 	InputTag("TriggerResults", "", "HLT2"));
	desc.add<InputTag>("objects", 	InputTag("slimmedPatTrigger"));
	desc.add<string>("baseTrigger", 	"HLT_PFHT800");
	desc.add<double>("hltjetEta", 	2.5);
	desc.add<double>("hltjetPt", 	10);
	desc.add<double>("recojetEta", 	2.5);
	desc.add<double>("recojetPt", 	10);
	desc.add<bool>("AK8jets", 	true);
	desc.add<bool>("DEBUG", 	false);
	desc.add<InputTag>("recojets", 	InputTag("slimmedJets"));
	desc.add<InputTag>("genjets", 	InputTag("slimmedGenJets"));
}
      
//define this as a plug-in
DEFINE_FWK_MODULE(TriggerResponses);
