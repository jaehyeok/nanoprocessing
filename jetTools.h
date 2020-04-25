//
// code taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagCalibration
//
// how to apply SF: https://twiki.cern.ch/twiki/bin/view/CMS/BTagSFMethods
//
// babymaker: https://github.com/richstu/babymaker/blob/master/bmaker/src/jet_met_tools.cc#L336-L399

#include "utilities.h"

// fastjet
#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>

// without CMSSW / standalone:
#include "BTagCalibrationStandalone.h"
//#include "BTagCalibrationStandalone.cpp"

float getBtagWeight(BTagCalibrationReader calibreader, float jet_pt, float jet_eta, float jet_hflavor, float csv) //, char* syst)
{
/*
  // setup calibration + reader
BTagCalibration calib("DeepCSV", "data/DeepCSV_2016LegacySF_V1.csv");
BTagCalibrationReader calibreader(BTagEntry::OP_RESHAPING,  // operating point
                             "central",                // central sys type
                             {"up_jes", "down_jes"});          // other sys types

calibreader.load(calib, BTagEntry::FLAV_B,     "iterativefit");
calibreader.load(calib, BTagEntry::FLAV_C,     "iterativefit");
calibreader.load(calib, BTagEntry::FLAV_UDSG,  "iterativefit");
*/
  //
  float btag_weight = 1;

  // absolute eta
  float jet_abseta = jet_eta;
  if(jet_eta<0) jet_abseta = -1*jet_eta;

  if (abs(jet_hflavor) == 5 ){    //HF		  
    btag_weight = calibreader.eval_auto_bounds("central", BTagEntry::FLAV_B, jet_abseta, jet_pt, csv);		  
  }
  else if( abs(jet_hflavor) == 4 ){  //C
    btag_weight = calibreader.eval_auto_bounds("central", BTagEntry::FLAV_C, jet_abseta, jet_pt, csv);
  }
  else { //LF
    btag_weight = calibreader.eval_auto_bounds("central", BTagEntry::FLAV_UDSG, jet_abseta, jet_pt,csv);
  }

  if(btag_weight==0) return 1; 
  else return btag_weight; 
}

bool jetIsLepton(float jets_eta, float jets_phi, vector<float> leps_eta, vector<float> leps_phi) 
{ 
  bool islep = false;
  for(int ilep=0; ilep<leps_eta.size(); ilep++) 
  { 
      if (deltaR(jets_eta, leps_eta.at(ilep), jets_phi, leps_phi.at(ilep)) < 0.4) islep = true;
  }
  return islep;
}

float getMJ(vector<float> jets_pt, vector<float> jets_eta, vector<float> jets_phi, vector<float> jets_m, vector<bool> jets_id)
{ 
  float Rparam = 1.2;
	
	float mj12;
    
	// Loop over R=0.5 jets, form into PseudoJets vector
	vector<fastjet::PseudoJet> input_particles;

	for(int iJ = 0; iJ < jets_pt.size(); iJ++) { 

		TLorentzVector JetLV; 
		JetLV.SetPtEtaPhiM(jets_pt.at(iJ), jets_eta.at(iJ), jets_phi.at(iJ), jets_m.at(iJ));

		// jet selection
		if(jets_pt.at(iJ)<30)           continue;
		if(abs(jets_eta.at(iJ))>2.4)    continue;
		if(jets_id.at(iJ)==false)       continue;

		input_particles.push_back(fastjet::PseudoJet(JetLV.Px(), JetLV.Py(), JetLV.Pz(), JetLV.E()));
	}
  
	//  
	fastjet::Strategy strategy = fastjet::Best;
	fastjet::RecombinationScheme recomb_scheme = fastjet::E_scheme;
	fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, Rparam, recomb_scheme, strategy);

	// run the jet clustering with the above jet definition
	fastjet::ClusterSequence clust_seq(input_particles, jet_def);
  
	//
	double ptmin = 0.0; // could use 3.0 here, instead of applying later 
	vector<fastjet::PseudoJet> inclusive_jets = clust_seq.inclusive_jets(ptmin);
	//Sort by pt
	vector<fastjet::PseudoJet> sorted_jets = sorted_by_pt(inclusive_jets);
	//fill fastjet output into vectors, continue as original code
	for(int isortjets = 0; isortjets< (int)sorted_jets.size(); isortjets++)
	{
		//store only if pt >3 GeV to match CMS jets
		if(TMath::Sqrt( sorted_jets[isortjets].px()*sorted_jets[isortjets].px()
					+sorted_jets[isortjets].py()*sorted_jets[isortjets].py())>3) 
		{
        mj12 += sorted_jets[isortjets].m(); 
		}
	}

	return mj12;
}
