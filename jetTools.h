//
// code taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagCalibration
//
// how to apply SF: https://twiki.cern.ch/twiki/bin/view/CMS/BTagSFMethods
//
// babymaker: https://github.com/richstu/babymaker/blob/master/bmaker/src/jet_met_tools.cc#L336-L399

#include "utilities.h"
#include "TROOT.h"
#include "TH3D.h"
#include "TFile.h"
#include "TLorentzVector.h"

// fastjet
#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>

// without CMSSW / standalone:
#include "BTagCalibrationStandalone.h"
//#include "BTagCalibrationStandalone.cpp"

float getBtagWeight(TFile *f, BTagCalibrationReader calibreader, float jet_pt, float jet_eta, float jet_hflavor, float csv, float csv_cut, const char* syst="central")
{
  //
  float btag_weight = 1;
  float eff(0), SF(0);
  float P_data(0), P_mc(0);

  TH3D *btag_eff;
  
  // absolute eta
  float jet_abseta = jet_eta;
  if(jet_eta<0) jet_abseta = -1*jet_eta;
  
/*
  SF_b = 1;
  SF_l = 1;
  SF_c = 1; */

  // ** FIXME : add effeciency ** //

  //cout<<jet_hflavor<<endl;
  //cout<<syst<<endl;

  int binx, biny, binz;
  if (abs(jet_hflavor) == 5 ){    //HF		 
    if(syst=="up_hf"||syst=="down_hf"){
      btag_eff = (TH3D*)f->Get("btagEfficiency_medium_comb");
      if(syst=="up_hf") syst="up";
      if(syst=="down_hf") syst="down";
    }
    else{
      syst="central";
      btag_eff = (TH3D*)f->Get("btagEfficiency_medium_comb_central");
    }
    SF = calibreader.eval_auto_bounds(syst, BTagEntry::FLAV_B, jet_abseta, jet_pt, csv);
    binx = btag_eff->GetXaxis()->FindBin(jet_eta);
    biny = btag_eff->GetYaxis()->FindBin(jet_pt);
    binz = btag_eff->GetZaxis()->FindBin(jet_hflavor);

    eff         = btag_eff->GetBinContent(binx,biny,binz);
  }
  else if( abs(jet_hflavor) == 4 ){  //C
    if(syst=="up_hf"||syst=="down_hf"){
      btag_eff = (TH3D*)f->Get("btagEfficiency_medium_comb");
      if(syst=="up_hf") syst="up";
      if(syst=="down_hf") syst="down";
    }
    else{
      syst="central";
      btag_eff = (TH3D*)f->Get("btagEfficiency_medium_comb_central");
    }
    
    SF = calibreader.eval_auto_bounds(syst, BTagEntry::FLAV_C, jet_abseta, jet_pt, csv);
    binx = btag_eff->GetXaxis()->FindBin(jet_eta);
    biny = btag_eff->GetYaxis()->FindBin(jet_pt);
    binz = btag_eff->GetZaxis()->FindBin(jet_hflavor);

    eff         = btag_eff->GetBinContent(binx,biny,binz);
  }
  else { //LF
    if(syst=="up_lf"||syst=="down_lf"){
      if(syst=="up_lf") syst="up";
      if(syst=="down_lf") syst="down";
    }
    else{
      syst="central";
    }

    SF = calibreader.eval_auto_bounds(syst, BTagEntry::FLAV_UDSG, jet_abseta, jet_pt, csv);
    btag_eff = (TH3D*)f->Get("btagEfficiency_medium_incl");
    binx = btag_eff->GetXaxis()->FindBin(jet_abseta);
    biny = btag_eff->GetYaxis()->FindBin(jet_pt);
    binz = btag_eff->GetZaxis()->FindBin(jet_hflavor);

    eff         = btag_eff->GetBinContent(binx,biny,binz);
  }
//  cout<<syst<<endl;
//  cout<<SF<<endl;
  if(csv>csv_cut){
    P_data      = SF*eff;
    P_mc        = eff;
  }
  else{
    P_data      = 1-SF*eff;
    P_mc        = 1-eff;
  }
  btag_weight = P_data/P_mc;
//  cout<<binx<<","<<biny<<","<<binz<<endl;
//  cout<<eff<<endl;
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
