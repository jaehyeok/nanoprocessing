//
// code taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagCalibration
//
// how to apply SF: https://twiki.cern.ch/twiki/bin/view/CMS/BTagSFMethods
//
// babymaker: https://github.com/richstu/babymaker/blob/master/bmaker/src/jet_met_tools.cc#L336-L399

#include "utilities.h"

// without CMSSW / standalone:
#include "BTagCalibrationStandalone.h"
#include "BTagCalibrationStandalone.cpp"

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
