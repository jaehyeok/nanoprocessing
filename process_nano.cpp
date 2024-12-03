// ------------------------------------
// Author : Jae Hyeok Yoo
//          jaehyeokyoo@korea.ac.kr
//          jae.hyeok.yoo@cern.ch
//          2019.10.28
// ------------------------------------
// command: ./skim.exe /xrootd_user/jaehyeok/xrootd/2016v4/2019_12_10/processed/ rpvfitnbge0
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream> // for stringstream

#include "TROOT.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TSystem.h"
#include "TH2F.h"
#include "TEllipse.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TBranch.h"
#include "TString.h"
#include "TLorentzVector.h"
#include "TChain.h"
#include "TObjString.h"

// JEC
#include "JetCorrectorParameters.h"
#include "JetCorrectionUncertainty.h"
#include "FactorizedJetCorrector.h"

// fastjet
#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
// btag
#include "BTagCalibrationStandalone.h"

//#include "utilities.h" // included in jetTools.h  
#include "jetTools.h"  
#include "mcTools.h"  
#include "lepTools.h"  
#include "inJSON.h"  

// --- compile --- 
// g++  process_nano.cpp JetCorrectorParameters.cpp FactorizedJetCorrector.cpp SimpleJetCorrector.cpp BTagCalibrationStandalone.cpp `fastjet-config --cxxflags --libs --plugins` `root-config --cflags --glibs` -o process_nano.exe
// --- run -------
// ./process_nano.exe /xrootd/store/mc/RunIISummer16NanoAODv4/SMS-T1tbs_RPV_mGluino1600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/NANOAODSIM/PUMoriond17_Nano14Dec2018_102X_mcRun2_asymptotic_v6_ext1-v1/ root://cms-xrdr.private.lo:2094//xrd/store/user/jaehyeok/2016v4/2019_12_06/ SMS-T1tbs_RPV_mGluino1600_TuneCUETP8M1 flist_outputdir_2016v4_2019_11_07.txt


using namespace std;

const bool DEBUG = false; 

// JEC  
FactorizedJetCorrector *JetCorrector;
JetCorrectionUncertainty *JecUnc;
//TFile *electronSF;
//TFile *muonSF;

void process_nano(TString inputfile, TString outputdir, float sumWeights, TString samplename, int nfiles, int &filenumber, TString year) 
{
  // 
  filenumber++; 

  //
//  int year    = 0;
  char First_ = '/'; 
  Int_t idxLast  = inputfile.Index("_13TeV");        // before making b-tag eff file
//  Int_t idxLast  = inputfile.Index("/NANO");         // after making b-tag eff file
  Int_t idxRemov = inputfile.Index("RunII");
  TString temp_ = inputfile.Copy();
  TString tag   = temp_.Remove(idxLast);
  tag           = tag.Remove(0,idxRemov);
  Int_t idxFirst = tag.First(First_);
  tag           = tag.Remove(0,idxFirst+1);
  
  cout<< "TAG             : " <<tag<< endl;

  cout << " year: " << year << endl;
  bool isData = false;
  if(inputfile.Contains("JetHT") || samplename.Contains("JetHT"))            isData = true;
  if(inputfile.Contains("SingleMuon") || samplename.Contains("SingleMuon"))  isData = true;
  cout << " is data?: " << isData << endl;

  // size of fat jets
  float Rparam = 1.2;
  
  // cross section 
  float xsec = getXsec(inputfile); 

  // ------------------------------------------------------------------
  // set up btag sf
  // ------------------------------------------------------------------
  // setup calibration + reader
  TFile *f_btef;
  if(!isData){
    if(year=="2016")f_btef = new TFile("btagEfficiency/230613/"+year+"/btagEfficiency_"+tag+"_2016.root","READ");
    else if(year=="2017")f_btef = new TFile("btagEfficiency/230613/"+year+"/btagEfficiency_"+tag+"_2017.root","READ");
    else if(year=="2018")f_btef = new TFile("btagEfficiency/230613/"+year+"/btagEfficiency_"+tag+"_2018.root","READ");
    else if(year=="UL2016_preVFP")f_btef = new TFile("btagEfficiency/ultralegacy/ptetabinned/"+year+"/btagEfficiency_"+samplename+"_"+year+".root","READ");
    else if(year=="UL2016")f_btef = new TFile("btagEfficiency/ultralegacy/ptetabinned/"+year+"/btagEfficiency_"+samplename+"_"+year+".root","READ");
    else if(year=="UL2017")f_btef = new TFile("btagEfficiency/ultralegacy/ptetabinned/"+year+"/btagEfficiency_"+samplename+"_"+year+".root","READ");
    else if(year=="UL2018")f_btef = new TFile("btagEfficiency/ultralegacy/ptetabinned/"+year+"/btagEfficiency_"+samplename+"_"+year+".root","READ");
    cout << "btagEfficiency file: " << f_btef->GetName() << endl;
  }

  string csvfile;
  if(year=="2016") csvfile = "data/prelegacy/DeepCSV_2016LegacySF_V1.csv"; //for loose cuts only 
  //else if(year=="2017") csvfile = "data/prelegacy/DeepCSV_94XSF_V5_B_F.csv"; // V5 is the same as V4 except for iterative fit. Since we don't use iterative fit, we can use V4 instead of V5. It seems that ROOT can not read formula of V5.
  //else if(year=="2018") csvfile = "data/prelegacy/DeepCSV_102XSF_V2.csv"; // S/A
  else if(year=="2017") csvfile = "data/prelegacy/DeepCSV_94XSF_V4_B_F.csv";
  else if(year=="2018") csvfile = "data/prelegacy/DeepCSV_102XSF_V1.csv";
  else if(year=="UL2016_preVFP") csvfile = "data/ultralegacy/wp_deepCSV_UL2016_preVFP.csv";
  else if(year=="UL2016") csvfile = "data/ultralegacy/wp_deepCSV_UL2016.csv";
  else if(year=="UL2017") csvfile = "data/ultralegacy/wp_deepCSV_UL2017.csv";
  else if(year=="UL2018") csvfile = "data/ultralegacy/wp_deepCSV_UL2018.csv";
  cout << " btag sf file: " << csvfile << endl;  
  BTagCalibration calib("DeepCSV", csvfile);
  /*
  BTagCalibrationReader calibreader(BTagEntry::OP_RESHAPING,  // operating point
      "central",                                              // central sys type
      {"up_hf", "down_hf", "up_lf", "down_lf"});                                // other sys types
  calibreader.load(calib, BTagEntry::FLAV_B,     "iterativefit");
  calibreader.load(calib, BTagEntry::FLAV_C,     "iterativefit");
  calibreader.load(calib, BTagEntry::FLAV_UDSG,  "iterativefit"); 
  // */
  BTagCalibrationReader calibreader(BTagEntry::OP_MEDIUM,  // operating point
      "central",                                              // central sys type
//      {"up", "down"});                                // other sys types
      {"up_uncorrelated", "down_uncorrelated", "up_correlated", "down_correlated"});                                // other sys types
  calibreader.load(calib, BTagEntry::FLAV_B,     "comb");
  calibreader.load(calib, BTagEntry::FLAV_C,     "comb");
  calibreader.load(calib, BTagEntry::FLAV_UDSG,  "incl"); 

  // Lepton SF files
  TFile *electronSF;
  if(year=="2016") electronSF = new TFile("data/prelegacy/ElectronScaleFactors_Run2016.root","read");
  else if(year=="2017") electronSF = new TFile("data/prelegacy/ElectronScaleFactors_Run2017.root","read");
  else if(year=="2018") electronSF = new TFile("data/prelegacy/ElectronScaleFactors_Run2018.root","read");
  else if(year=="UL2016_preVFP") electronSF = new TFile("data/ultralegacy/fullsim_electron_isolated_UL2016_preVFP.root","read");
  else if(year=="UL2016") electronSF = new TFile("data/ultralegacy/fullsim_electron_isolated_UL2016_postVFP.root","read");
  else if(year=="UL2017") electronSF = new TFile("data/ultralegacy/fullsim_electron_isolated_UL2017.root","read");
  else if(year=="UL2018") electronSF = new TFile("data/ultralegacy/fullsim_electron_isolated_UL2018.root","read");
    
  TFile *muonSF;
  if(year=="2016")muonSF = new TFile("data/prelegacy/TnP_NUM_MiniIsoTight_DENOM_MediumID_VAR_map_pt_eta.root","read");
  else if(year=="2017"||year=="2018")muonSF = new TFile("data/prelegacy/2017MiniIso0.2AndMediumID_SF.root","read");
  else if(year=="UL2016_preVFP")muonSF = new TFile("data/ultralegacy/fullsim_muon_isolated_UL2016_preVFP.root","read");
  else if(year=="UL2016")muonSF = new TFile("data/ultralegacy/fullsim_muon_isolated_UL2016_postVFP.root","read");
  else if(year=="UL2017")muonSF = new TFile("data/ultralegacy/fullsim_muon_isolated_UL2017.root","read");
  else if(year=="UL2018")muonSF = new TFile("data/ultralegacy/fullsim_muon_isolated_UL2018.root","read");

  // PU reweight file
  TFile *f_pu_weight = new TFile("data/ultralegacy/pileup/weight/"+year+"/pu_weight_"+samplename+"_"+year+".root","READ");


  // ------------------------------------------------------------------ 
  // json for DATA 
  // ------------------------------------------------------------------ 
  std::vector< std::vector<int> > VRunLumi; VRunLumi.clear();
  if(isData) 
  {
    string jsonfile;
    if(year=="2016") jsonfile = "data/prelegacy/golden_Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16.json";
    else if(year=="2017") jsonfile = "data/prelegacy/golden_Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17.json";
    else if(year=="2018") jsonfile = "data/prelegacy/golden_Cert_314472-325175_13TeV_PromptReco_Collisions18.json";
    else if(year=="UL2016_preVFP"||year=="UL2016") jsonfile = "data/ultralegacy/Cert_271036-284044_13TeV_Legacy2016_Collisions16_JSON.txt";
    else if(year=="UL2017") jsonfile = "data/ultralegacy/Cert_294927-306462_13TeV_UL2017_Collisions17_GoldenJSON.txt";
    else if(year=="UL2018") jsonfile = "data/ultralegacy/Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON.txt";
    else {
      cout << "[Error] No proper choice of JSON files!!" << endl;
      return ;
    }
    VRunLumi = MakeVRunLumi(jsonfile); 
    cout << "json file : " << jsonfile << endl;       
  } 
  else cout << "[MJ Analysis] No JSON files applied because it is MC" << endl;


  // ------------------------------------------------------------------ 
  // Get tree from a nanoaod file 
  // ------------------------------------------------------------------ 
  TFile *f = TFile::Open(inputfile, "READ");
  TTree *tree = (TTree*)f->Get("Events");
  // 
  // set address of variables to read 
  // 
  // global
  ULong64_t   event_  = 0;
  UInt_t      run_    = 0;
  UInt_t      ls_     = 0;
  Float_t     MET_pt  = 0;
  Float_t     MET_phi = 0;
  Float_t     Pileup_nTrueInt = 0; 
  Int_t       Pileup_nPU = 0;
  Float_t     fixedGridRhoFastjetAll = 0; 
  // deepTag
  Float_t     FatJet_deepTagMD_TvsQCD[100];
  Float_t     FatJet_deepTag_TvsQCD[100];
  // weights
  Float_t     btagWeight_CSVV2 = 1;
  Float_t     btagWeight_DeepCSVB = 1;
  Float_t     genWeight        = 1;
  //LHE HT incomming
  Float_t     LHE_HTIncoming = 0;
  Float_t     L1PreFiringWeight_Dn = 1;
  Float_t     L1PreFiringWeight_Nom = 1;
  Float_t     L1PreFiringWeight_Up = 1;
  Float_t     L1PreFiringWeight_ECAL_Dn = 1;
  Float_t     L1PreFiringWeight_ECAL_Nom = 1;
  Float_t     L1PreFiringWeight_ECAL_Up = 1;
  Float_t     L1PreFiringWeight_Muon_StatDn = 1;
  Float_t     L1PreFiringWeight_Muon_StatUp = 1;
  Float_t     L1PreFiringWeight_Muon_SystDn = 1;
  Float_t     L1PreFiringWeight_Muon_SystUp = 1;
  Float_t     L1PreFiringWeight_Muon_Nom = 1;
  //LHE Scale Weight
  Float_t     LHEScaleWeight[9];
  // MC 
  UInt_t  nGenPart=0;
  Float_t GenPart_eta[500];  
  Float_t GenPart_mass[500]; 
  Float_t GenPart_phi[500];  
  Float_t GenPart_pt[500];   
  Int_t   GenPart_genPartIdxMother[500]; 
  Int_t   GenPart_pdgId[500]; 
  Int_t   GenPart_status[500];
  Int_t   GenPart_statusFlags[500];
  // leptons   // 25 -> 100 for testing
  UInt_t  nElectron=0;  
  Float_t Electron_pt[100];         
  Float_t Electron_eta[100];  
  Float_t Electron_phi[100]; 
  Float_t Electron_mass[100]; 
  Float_t Electron_dz[100];
  Float_t Electron_dxy[100];
  Int_t   Electron_cutBased[100];  // cut-based ID Fall17 V2: susy recommendation, use medium(https://github.com/richstu/babymaker/blob/master/bmaker/src/lepton_tools.cc#L289)
  //Int_t   Electron_cutBased_Spring15[100];  // Spring15 ID to compare with AN-2016/187 (1L MJ 2016 data)  
  Bool_t  Electron_mvaFall17V2noIso_WP90[100];
  Int_t   Electron_jetIdx[100];    // index of the associated jet (-1 if none)  
  Int_t   Electron_pdgId[100];   
  Float_t   Electron_miniPFRelIso_all[100];   
  Float_t   Electron_pfRelIso03_all[100];   
  Int_t   Electron_vidNestedWPBitmap[100];   
  //Int_t   Electron_vidNestedWPBitmapSpring15[100];   
  UInt_t  nMuon=0;  
  Float_t Muon_pt[100]; 
  Float_t Muon_eta[100];  
  Float_t Muon_phi[100]; 
  Float_t Muon_mass[100]; 
  Float_t Muon_dz[100]; 
  Float_t Muon_dxy[100]; 
  Bool_t  Muon_mediumId[100];  // medium id taken from babymaker: https://github.com/richstu/babymaker/blob/master/bmaker/src/lepton_tools.cc#L190  
  Bool_t  Muon_mediumPromptId[100];
  Int_t   Muon_jetIdx[100];    // index of the associated jet (-1 if none)  
  Int_t   Muon_pdgId[100];   
  Float_t   Muon_miniPFRelIso_all[100];   
  // jets
  UInt_t     nJet=0;  
  Float_t    Jet_pt[100];
  Float_t    Jet_eta[100];
  Float_t    Jet_phi[100];
  Float_t    Jet_m[100];
  Float_t    Jet_btagCSVV2[100];
  Float_t    Jet_btagDeepB[100];
//  Float_t    Jet_btagDeepC[100];
  Float_t    Jet_rawFactor[100];
  Float_t    Jet_area[100];
  Float_t    Jet_qgl[100];
  Float_t    Jet_pt_jerUp[100];
  Float_t    Jet_pt_jerDown[100];
  Int_t      Jet_jetId[100];
  Int_t      Jet_nElectrons[100];
  Int_t      Jet_nMuons[100];
  Int_t      Jet_hadronFlavour[100];
  Int_t      Jet_partonFlavour[100];
  // filters 
  Bool_t Flag_METFilters=true;
  Bool_t Flag_goodVertices=true;
  Bool_t Flag_globalSuperTightHalo2016Filter=true;
  Bool_t Flag_HBHENoiseFilter=true;
  Bool_t Flag_HBHENoiseIsoFilter=true;
  Bool_t Flag_EcalDeadCellTriggerPrimitiveFilter=true;
  Bool_t Flag_BadPFMuonFilter=true;
  Bool_t Flag_BadPFMuonDzFilter=true;
  Bool_t Flag_eeBadScFilter=true;
  Bool_t Flag_ecalBadCalibFilter=true;
  // trigger 
  Bool_t HLT_PFJet450=true;
  Bool_t HLT_PFHT900=true;
  Bool_t HLT_PFHT1050=true;
  Bool_t HLT_IsoMu24=true;
  Bool_t HLT_IsoMu27=true;

  // global
  tree->SetBranchAddress("event",               &event_);
  tree->SetBranchAddress("run",                 &run_);
  tree->SetBranchAddress("luminosityBlock",     &ls_);
  tree->SetBranchAddress("MET_pt",              &MET_pt);
  tree->SetBranchAddress("MET_phi",             &MET_phi);
  tree->SetBranchAddress("fixedGridRhoFastjetAll",          &fixedGridRhoFastjetAll);
  //LHE HT incoming
  //PreFiring weight
  if(!isData && (year=="2016" || year=="2017" || year=="UL2016_preVFP" || year=="UL2016" || year=="UL2017")){
    tree->SetBranchAddress("L1PreFiringWeight_Dn",  &L1PreFiringWeight_Dn);
    tree->SetBranchAddress("L1PreFiringWeight_Nom",  &L1PreFiringWeight_Nom);
    tree->SetBranchAddress("L1PreFiringWeight_Up",  &L1PreFiringWeight_Up);
    tree->SetBranchAddress("L1PreFiringWeight_ECAL_Dn",  &L1PreFiringWeight_ECAL_Dn);
    tree->SetBranchAddress("L1PreFiringWeight_ECAL_Nom",  &L1PreFiringWeight_ECAL_Nom);
    tree->SetBranchAddress("L1PreFiringWeight_ECAL_Up",  &L1PreFiringWeight_ECAL_Up);
    tree->SetBranchAddress("L1PreFiringWeight_Muon_StatDn",  &L1PreFiringWeight_Muon_StatDn);
    tree->SetBranchAddress("L1PreFiringWeight_Muon_StatUp",  &L1PreFiringWeight_Muon_StatUp);
    tree->SetBranchAddress("L1PreFiringWeight_Muon_SystDn",  &L1PreFiringWeight_Muon_SystDn);
    tree->SetBranchAddress("L1PreFiringWeight_Muon_SystUp",  &L1PreFiringWeight_Muon_SystUp);
    tree->SetBranchAddress("L1PreFiringWeight_Muon_Nom",  &L1PreFiringWeight_Muon_Nom);
  }
  if(!isData){
    tree->SetBranchAddress("LHEScaleWeight",  &LHEScaleWeight);
    tree->SetBranchAddress("LHE_HTIncoming",  &LHE_HTIncoming);
    tree->SetBranchAddress("Pileup_nTrueInt", &Pileup_nTrueInt);
    tree->SetBranchAddress("Pileup_nPU",      &Pileup_nPU);
  }
  // deepTag
  if(!isData)
  {
    tree->SetBranchAddress("FatJet_deepTagMD_TvsQCD",    &FatJet_deepTagMD_TvsQCD);
    tree->SetBranchAddress("FatJet_deepTag_TvsQCD",    &FatJet_deepTag_TvsQCD);
  }
  // weights 
  if(!isData){
    tree->SetBranchAddress("btagWeight_CSVV2",    &btagWeight_CSVV2);
    tree->SetBranchAddress("btagWeight_DeepCSVB",    &btagWeight_DeepCSVB);
  }
  // MC 
  if(!isData)
  {
    tree->SetBranchAddress("genWeight",           &genWeight);
    tree->SetBranchAddress("nGenPart",            &nGenPart);
    tree->SetBranchAddress("GenPart_eta",         &GenPart_eta);
    tree->SetBranchAddress("GenPart_mass",        &GenPart_mass);
    tree->SetBranchAddress("GenPart_pt",          &GenPart_pt);
    tree->SetBranchAddress("GenPart_phi",         &GenPart_phi);
    tree->SetBranchAddress("GenPart_genPartIdxMother",  &GenPart_genPartIdxMother);
    tree->SetBranchAddress("GenPart_pdgId",       &GenPart_pdgId);
    tree->SetBranchAddress("GenPart_status",      &GenPart_status); // Particle status. 1=stable
    tree->SetBranchAddress("GenPart_statusFlags", &GenPart_statusFlags); // don't know what this variable is
  }
  // leptons 
  tree->SetBranchAddress("nElectron",           &nElectron);
  tree->SetBranchAddress("Electron_pt",         &Electron_pt);
  tree->SetBranchAddress("Electron_eta",        &Electron_eta);
  tree->SetBranchAddress("Electron_phi",        &Electron_phi);
  tree->SetBranchAddress("Electron_mass",       &Electron_mass);
  tree->SetBranchAddress("Electron_dxy",        &Electron_dxy);
  tree->SetBranchAddress("Electron_dz",         &Electron_dz);
  tree->SetBranchAddress("Electron_cutBased",   &Electron_cutBased);
  //tree->SetBranchAddress("Electron_cutBased_Spring15",   &Electron_cutBased_Spring15);
  tree->SetBranchAddress("Electron_mvaFall17V2noIso_WP90",   &Electron_mvaFall17V2noIso_WP90);
  tree->SetBranchAddress("Electron_jetIdx",     &Electron_jetIdx);
  tree->SetBranchAddress("Electron_pdgId",      &Electron_pdgId);
  tree->SetBranchAddress("Electron_miniPFRelIso_all", &Electron_miniPFRelIso_all);
  tree->SetBranchAddress("Electron_pfRelIso03_all", &Electron_pfRelIso03_all);
  tree->SetBranchAddress("Electron_vidNestedWPBitmap", &Electron_vidNestedWPBitmap);
  //tree->SetBranchAddress("Electron_vidNestedWPBitmapSpring15", &Electron_vidNestedWPBitmapSpring15);
  tree->SetBranchAddress("nMuon",               &nMuon);
  tree->SetBranchAddress("Muon_pt",             &Muon_pt);
  tree->SetBranchAddress("Muon_eta",            &Muon_eta);
  tree->SetBranchAddress("Muon_phi",            &Muon_phi);
  tree->SetBranchAddress("Muon_mass",           &Muon_mass);
  tree->SetBranchAddress("Muon_dxy",            &Muon_dxy);
  tree->SetBranchAddress("Muon_dz",             &Muon_dz);
  tree->SetBranchAddress("Muon_mediumId",       &Muon_mediumId);
  tree->SetBranchAddress("Muon_mediumPromptId", &Muon_mediumPromptId);
  tree->SetBranchAddress("Muon_jetIdx",         &Muon_jetIdx);
  tree->SetBranchAddress("Muon_pdgId",          &Muon_pdgId);
  tree->SetBranchAddress("Muon_miniPFRelIso_all",    &Muon_miniPFRelIso_all);
  // jets
  tree->SetBranchAddress("nJet",                &nJet);
  tree->SetBranchAddress("Jet_pt",              &Jet_pt);
  tree->SetBranchAddress("Jet_eta",             &Jet_eta); 
  tree->SetBranchAddress("Jet_phi",             &Jet_phi); 
  tree->SetBranchAddress("Jet_mass",            &Jet_m); 
  tree->SetBranchAddress("Jet_btagCSVV2",       &Jet_btagCSVV2); 
  tree->SetBranchAddress("Jet_btagDeepB",       &Jet_btagDeepB); 
//  tree->SetBranchAddress("Jet_btagDeepC",       &Jet_btagDeepC);
  tree->SetBranchAddress("Jet_qgl",		&Jet_qgl);
  tree->SetBranchAddress("Jet_pt_jerUp",	&Jet_pt_jerUp);
  tree->SetBranchAddress("Jet_pt_jerDown",	&Jet_pt_jerDown);
  tree->SetBranchAddress("Jet_jetId",           &Jet_jetId); 
  tree->SetBranchAddress("Jet_nElectrons",      &Jet_nElectrons); 
  tree->SetBranchAddress("Jet_nMuons",          &Jet_nMuons); 
  if(!isData)
  {
    tree->SetBranchAddress("Jet_hadronFlavour",   &Jet_hadronFlavour); 
    tree->SetBranchAddress("Jet_partonFlavour",   &Jet_partonFlavour);
  }
  // filters 
  tree->SetBranchAddress("Flag_METFilters",                         &Flag_METFilters); // not sure if this is enough 
  tree->SetBranchAddress("Flag_goodVertices",                       &Flag_goodVertices);
  tree->SetBranchAddress("Flag_globalSuperTightHalo2016Filter",     &Flag_globalSuperTightHalo2016Filter);
  tree->SetBranchAddress("Flag_HBHENoiseFilter",                    &Flag_HBHENoiseFilter);
  tree->SetBranchAddress("Flag_HBHENoiseIsoFilter",                 &Flag_HBHENoiseIsoFilter);
  tree->SetBranchAddress("Flag_EcalDeadCellTriggerPrimitiveFilter", &Flag_EcalDeadCellTriggerPrimitiveFilter);
  tree->SetBranchAddress("Flag_BadPFMuonFilter",                    &Flag_BadPFMuonFilter);
  tree->SetBranchAddress("Flag_BadPFMuonDzFilter",                  &Flag_BadPFMuonDzFilter);
  tree->SetBranchAddress("Flag_eeBadScFilter",                      &Flag_eeBadScFilter);
  tree->SetBranchAddress("Flag_ecalBadCalibFilter",                 &Flag_ecalBadCalibFilter);
  // trigger
  if(year=="2016"||year=="UL2016_preVFP"||year=="UL2016")
  {
    tree->SetBranchAddress("HLT_PFHT900",     &HLT_PFHT900);
    tree->SetBranchAddress("HLT_PFJet450",    &HLT_PFJet450);
  }
  else if(year=="2017"||year=="2018"||year=="UL2017"||year=="UL2018")
  {
    tree->SetBranchAddress("HLT_PFHT1050",    &HLT_PFHT1050);
  }
  tree->SetBranchAddress("HLT_IsoMu24",   &HLT_IsoMu24);
  tree->SetBranchAddress("HLT_IsoMu27",   &HLT_IsoMu27);

  // ------------------------------------------------------------------ 
  // New file 
  // ------------------------------------------------------------------ 
  TObjArray *tokens = inputfile.Tokenize("/"); 
  TString outputfile = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries()-1)))->GetString();
  outputfile.ReplaceAll(".root", Form("_fatjetbaby_%s.root", samplename.Data()));

  TFile *babyFile_ = new TFile("/data3/nanoprocessing/Running/"+outputfile, "RECREATE");
  babyFile_->cd();
  TTree *babyTree_ = new TTree("tree", "baby ntuple from nanoaod");

  cout << " processing file  : " << inputfile << endl;
  cout << " output file name : " << outputfile << endl;
  cout << " sample name      : " << samplename << endl;

  // 
  // Define new variables to write 
  // 
  // need this line because ROOT does not know vector<float>
  // one needs to generate dictionary using rootcint or 
  // just include this line 
  // [ref] http://root.cern.ch/phpBB3/viewtopic.php?t=8467
  gROOT->ProcessLine("#include <vector>"); 

  //
  // branches in the new tree
  //
  ULong64_t  event = 0;
  UInt_t   run = 0;
  UInt_t   ls = 0;

  //Global   
  float weight      =1;
  float w_btag_csv  =1;
  float w_btag_dcsv =1;
  //float w_btag_dcsv_norm =1;
  float w_pu        =1;
  float w_lumi      =1;
  float w_toppt     =1;
  float w_lep       =1;
  float w_isr       =1;
  float isr_wgt     =1;
  float isr_norm    =1;
  int nisr          =0;
  bool stitch_ht    =true;

  //std::vector<float> w_pdf;
  //float eff_trig;
  int npv           =-1;
  int ntrupv        =-1;
  float ntrupv_mean =-1;
  float met         =-1;
  float met_phi     =-1;
  float ht          =0;
  //float mt;
  //LHE_HTIncoming
  float lhe_ht      =-1;
  float l1pre_nom = -1;
  float l1pre_dn = -1;
  float l1pre_up = -1;

  std::vector<bool> trig;
  bool stitch;
  bool pass=true;
  bool fromGS=false;
  bool matched;
  float llp_dR;
  /*
  //MC   
  int ntruleps;
  int ntrumus;
  int ntruels;
  int ntrutaush;
  int ntrutausl;
  */
  //Leptons   
  int nleps;
  std::vector<float> leps_pt;
  std::vector<float> leps_eta;
  std::vector<float> leps_phi;
  std::vector<float> leps_miniso;
  std::vector<int>   leps_pdgid;

  int nmus;
  //int nvmus;
  std::vector<float> mus_pt;
  std::vector<float> mus_eta;
  std::vector<float> mus_phi;
  std::vector<bool>  mus_sigid;
  std::vector<float> mus_miniso;
  std::vector<float> mus_d0;
  std::vector<float> mus_dz;
  //std::vector<int> mus_charge;

  int nels;
  //int nvels;
  std::vector<float> els_pt;
  std::vector<float> els_eta;
  //std::vector<float> els_sceta;
  std::vector<float> els_phi;
  std::vector<bool>  els_sigid;
  std::vector<bool>  els_spr15_sigid;
  std::vector<float> els_miniso;
  std::vector<float> els_reliso;
  //std::vector<bool> els_ispf;
  std::vector<float> els_d0;
  std::vector<float> els_dz;
  //std::vector<int> els_charge;

  //Jets   
  int njets;
  int nbm;

  std::vector<float> jets_pt;
  std::vector<float> jets_eta;
  std::vector<float> jets_phi;
  std::vector<float> jets_m;
  std::vector<float> jets_csv;
  std::vector<float> jets_dcsvb;
//  std::vector<float> jets_dcsvc;
  std::vector<float> jets_qgl;
  std::vector<bool>  jets_id;
  std::vector<bool>  jets_islep;
  //std::vector<int>   jets_fjet_index;
  std::vector<int>   jets_hflavor;
  std::vector<bool>  jets_hem;
  std::vector<bool>  jets_pt_jerUp;
  std::vector<bool>  jets_pt_jerDown;
  // top tagging
  std::vector<float> toptag_md_ttvsqcd;
  std::vector<float> toptag_ttvsqcd;

  // GenParticle
  int ngen;
  std::vector<float> gen_pt;
  std::vector<float> gen_eta;
  std::vector<float> gen_phi;
  std::vector<float> gen_m;
  std::vector<int> gen_PartIdxMother;
  std::vector<int> gen_pdgId;
  std::vector<int> gen_status;
  std::vector<int> gen_statusFlags;

  //    std::vector<float> dr_bb;
  //Fat Jets   
  //    int nfjets;
  //    float mj12;
  float mj12;
  std::vector<float> fjets_pt;
  std::vector<float> fjets_eta;
  std::vector<float> fjets_phi;
  std::vector<float> fjets_m;
  std::vector<int> fjets_nconst;
  //    std::vector<float> fjets_sumcsv;
  //    std::vector<float> fjets_poscsv;
  //    std::vector<int> fjets_btags;
  //syst   
  std::vector<float> sys_bctag_uncor;
  std::vector<float> sys_bctag_cor;
  std::vector<float> sys_udsgtag_uncor;
  std::vector<float> sys_udsgtag_cor;
  std::vector<float> sys_pu;
  std::vector<float> sys_lep;
  std::vector<float> sys_mur;
  std::vector<float> sys_muf;
  std::vector<float> sys_murf;
  std::vector<float> sys_isr;
  std::vector<float> sys_ht;
  std::vector<float> sys_mj12;
  std::vector<int>   sys_njets;
  std::vector<int>   sys_nbm;
  std::vector<bool>  sys_pass;
  // trigger 
  bool trig_jet450=true; 
  bool trig_ht900=true; 
  bool trig_ht1050=true;
  bool trig_isomu24=true; 
  bool trig_isomu27=true; 
  bool pass_hbheiso=true;
  // weight
  float gen_weight = 1;

  // global
  babyTree_->Branch("run",               &run);    
  babyTree_->Branch("ls",                &ls);    
  babyTree_->Branch("event",             &event);
  babyTree_->Branch("ht",                &ht);
  babyTree_->Branch("met",               &met);
  babyTree_->Branch("ntrupv",            &ntrupv);
  babyTree_->Branch("ntrupv_mean",       &ntrupv_mean);
  babyTree_->Branch("met_phi",           &met_phi);
  babyTree_->Branch("lhe_ht",            &lhe_ht);
  babyTree_->Branch("l1pre_nom", 	 &l1pre_nom);
  babyTree_->Branch("l1pre_dn", 	 &l1pre_dn);
  babyTree_->Branch("l1pre_up",		 &l1pre_up);
  babyTree_->Branch("stitch_ht",         &stitch_ht);
  // weights 
  babyTree_->Branch("weight",            &weight);
  babyTree_->Branch("w_btag_csv",    	 &w_btag_csv);
  babyTree_->Branch("w_btag_dcsv",   	 &w_btag_dcsv);
  //babyTree_->Branch("w_btag_dcsv_norm",  &w_btag_dcsv_norm);
  babyTree_->Branch("w_lumi",            &w_lumi);
  babyTree_->Branch("w_pu",              &w_pu);
  babyTree_->Branch("w_lep",             &w_lep);
  babyTree_->Branch("xsec",		 &xsec);
  babyTree_->Branch("w_isr",		 &w_isr);
  babyTree_->Branch("isr_wgt",		 &isr_wgt);
  babyTree_->Branch("isr_norm",		 &isr_norm);
  babyTree_->Branch("nisr",		 &nisr);
  babyTree_->Branch("matched",	         &matched);
  // leptons 
  babyTree_->Branch("nleps",             &nleps);    
  babyTree_->Branch("leps_pt",           &leps_pt);    
  babyTree_->Branch("leps_eta",          &leps_eta);    
  babyTree_->Branch("leps_phi",          &leps_phi);    
  babyTree_->Branch("leps_pdgid",        &leps_pdgid);    
  babyTree_->Branch("leps_miniso",       &leps_miniso);    
  babyTree_->Branch("nmus",              &nmus);    
  babyTree_->Branch("mus_pt",            &mus_pt);    
  babyTree_->Branch("mus_eta",           &mus_eta);    
  babyTree_->Branch("mus_phi",           &mus_phi);    
  babyTree_->Branch("mus_sigid",         &mus_sigid);    
  babyTree_->Branch("mus_miniso",        &mus_miniso);    
  babyTree_->Branch("mus_d0",            &mus_d0);    
  babyTree_->Branch("mus_dz",            &mus_dz);    
  babyTree_->Branch("nels",              &nels);    
  babyTree_->Branch("els_pt",            &els_pt);    
  babyTree_->Branch("els_eta",           &els_eta);    
  babyTree_->Branch("els_phi",           &els_phi);    
  babyTree_->Branch("els_sigid",         &els_sigid);    
  babyTree_->Branch("els_spr15_sigid",   &els_spr15_sigid);    
  babyTree_->Branch("els_miniso",        &els_miniso);    
  babyTree_->Branch("els_reliso",        &els_reliso);    
  babyTree_->Branch("els_d0",            &els_d0);    
  babyTree_->Branch("els_dz",            &els_dz);    
  // jets 
  babyTree_->Branch("njets",             &njets);
  babyTree_->Branch("nbm",               &nbm);    
  babyTree_->Branch("jets_pt",           &jets_pt);    
  babyTree_->Branch("jets_eta",          &jets_eta);    
  babyTree_->Branch("jets_phi",          &jets_phi);    
  babyTree_->Branch("jets_m",            &jets_m);    
  babyTree_->Branch("jets_csv",          &jets_csv);    
  babyTree_->Branch("jets_dcsvb",        &jets_dcsvb);    
//  babyTree_->Branch("jets_dcsvc",        &jets_dcsvc);
  babyTree_->Branch("jets_qgl",		 &jets_qgl);
  babyTree_->Branch("jets_id",           &jets_id);    
  babyTree_->Branch("jets_islep",        &jets_islep);    
  babyTree_->Branch("jets_hflavor",      &jets_hflavor);    
  babyTree_->Branch("jets_hem",     	 &jets_hem);    
  babyTree_->Branch("jets_pt_jerUp",     &jets_pt_jerUp);
  babyTree_->Branch("jets_pt_jerDown",   &jets_pt_jerDown);
  //deepTag
  babyTree_->Branch("toptag_md_ttvsqcd", &toptag_md_ttvsqcd);
  babyTree_->Branch("toptag_ttvsqcd", &toptag_ttvsqcd);
  // fatjet
  babyTree_->Branch("mj12",              &mj12);    
  babyTree_->Branch("fjets_pt",          &fjets_pt);    
  babyTree_->Branch("fjets_eta",         &fjets_eta);    
  babyTree_->Branch("fjets_phi",         &fjets_phi);    
  babyTree_->Branch("fjets_m",           &fjets_m);    
  babyTree_->Branch("fjets_nconst",      &fjets_nconst);    
  // GenParticles
  babyTree_->Branch("ngen",              &ngen);
  babyTree_->Branch("gen_pt",            &gen_pt);
  babyTree_->Branch("gen_eta",           &gen_eta);
  babyTree_->Branch("gen_phi",           &gen_phi);
  babyTree_->Branch("gen_m",             &gen_m);
  babyTree_->Branch("gen_PartIdxMother", &gen_PartIdxMother);
  babyTree_->Branch("gen_pdgId",         &gen_pdgId);
  babyTree_->Branch("gen_status",        &gen_status);
  babyTree_->Branch("gen_statusFlags",   &gen_statusFlags);
  // filters
  babyTree_->Branch("pass",              &pass);
  babyTree_->Branch("fromGS",            &fromGS);
  babyTree_->Branch("llp_dR",            &llp_dR);
  //
  babyTree_->Branch("sys_mj12",          &sys_mj12);    
  babyTree_->Branch("sys_pu",            &sys_pu);    
  babyTree_->Branch("sys_lep",           &sys_lep);    
  babyTree_->Branch("sys_ht",            &sys_ht);    
  babyTree_->Branch("sys_nbm",           &sys_nbm);    
  babyTree_->Branch("sys_njets",         &sys_njets);    
  babyTree_->Branch("sys_mur",           &sys_mur);    
  babyTree_->Branch("sys_muf",           &sys_muf);    
  babyTree_->Branch("sys_murf",          &sys_murf);    
  babyTree_->Branch("sys_isr",           &sys_isr);    
  babyTree_->Branch("sys_bctag_uncor",   &sys_bctag_uncor);    
  babyTree_->Branch("sys_bctag_cor",     &sys_bctag_cor);    
  babyTree_->Branch("sys_udsgtag_uncor",    &sys_udsgtag_uncor);    
  babyTree_->Branch("sys_udsgtag_cor",      &sys_udsgtag_cor);    
  // triggers 
  babyTree_->Branch("trig_jet450",       &trig_jet450);    
  babyTree_->Branch("trig_ht900",        &trig_ht900);    
  babyTree_->Branch("trig_ht1050",       &trig_ht1050);    
  babyTree_->Branch("trig_isomu24",      &trig_isomu24);    
  babyTree_->Branch("trig_isomu27",      &trig_isomu27);    
  babyTree_->Branch("pass_hbheiso",      &pass_hbheiso);    
  // weight
  babyTree_->Branch("gen_weight",         &gen_weight);    


  // 
  // Histgrom : to draw eta-phi plot of energy deposit 
  //  (1) Bin size is 0.087x0.087 to mimic the size of hcal tower
  //  (2) Bin Entry is the sum over energies of PF candidates in a given bin  
  // 
  TH2F *h2 = new TH2F("h2","h2", 115, -5.0, 5.0, 72, -1*TMath::Pi(), TMath::Pi());
  // 
  // Loop over entries
  // 
  Int_t nentries = (Int_t)tree->GetEntries();
  if(DEBUG) nentries = 1;
  cout<<"The number of entries in this file is: "<<nentries<<endl;

  // main event loop
 for(int ievt = 0; ievt<nentries; ievt++) {

    // Counting to see progress
  if(ievt%100000==0) 
    //if(ievt%1==0) 
    { 
      cout << " ...... " << ievt << "/" << nentries << " processed (file number = " << filenumber << "/" << nfiles << ")" << endl; 
    }

    /*for(int i = 0; i < jets_pt.size(); i++)
	cout<< "jets_pt: " << jets_pt[i] << endl;

    for(int i = 0; i < jets_hflavor.size(); i++ )
	cout<< "jets_hflavor: " << jets_hflavor[i] << endl;*/

    // get the entry of event ievt
    tree->GetEntry(ievt);

    // intialize baby variables
    run        =   -1;
    ls         =   -1;
    event      =   -1;
    ht         =   0;
    met        =   -1;
    met_phi    =   -1;
    ntrupv        =   0;
    ntrupv_mean   =   0;
    lhe_ht   =     -1;
    stitch_ht = true;
    l1pre_nom = -1;
    l1pre_dn = -1;
    l1pre_up = -1;
    // weights 
    weight        =    1;
    w_lep         =    1; 
    w_lumi        =    1;
    w_btag_csv    =    1;
    w_btag_dcsv   =    1;
    w_pu          =    1;
    // leptons 
    nleps      =   0;           
    leps_pt.clear();         
    leps_eta.clear();          
    leps_phi.clear();          
    leps_pdgid.clear();          
    leps_miniso.clear();          
    nmus       =   0;           
    mus_pt.clear();         
    mus_eta.clear();          
    mus_phi.clear();          
    mus_sigid.clear();          
    mus_miniso.clear();          
    mus_d0.clear();
    mus_dz.clear();
    nels       =   0;           
    els_pt.clear();         
    els_eta.clear();          
    els_phi.clear();          
    els_sigid.clear();          
    els_spr15_sigid.clear();          
    els_miniso.clear();          
    els_reliso.clear();          
    els_d0.clear();
    els_dz.clear();
    // jets 
    njets      =   0;        
    nbm        =   0;          
    jets_pt.clear();      
    jets_eta.clear();      
    jets_phi.clear();      
    jets_m.clear();        
    jets_csv.clear();      
    jets_dcsvb.clear();      
//    jets_dcsvc.clear();
    jets_qgl.clear();
    jets_id.clear();      
    jets_islep.clear();     
    jets_hflavor.clear();      
    jets_hem.clear();
    jets_pt_jerUp.clear();
    jets_pt_jerDown.clear();
    toptag_md_ttvsqcd.clear();
    toptag_ttvsqcd.clear();
    //
    mj12         =   0;
    fjets_pt.clear();
    fjets_eta.clear();
    fjets_phi.clear();
    fjets_m.clear();
    fjets_nconst.clear();
    //
    ngen = 0;
    gen_pt.clear();
    gen_eta.clear();
    gen_phi.clear();
    gen_m.clear();
    gen_PartIdxMother.clear();
    gen_pdgId.clear();
    gen_status.clear();
    gen_statusFlags.clear();
    //    
    pass=true;
    fromGS=false;
    //
    sys_mj12.clear();
    sys_pu.clear();
    sys_ht.clear();
    sys_lep.clear();
    sys_njets.clear();
    sys_nbm.clear();
    sys_mur.clear();
    sys_muf.clear();
    sys_murf.clear();
    sys_isr.clear();
    sys_bctag_uncor.clear();
    sys_bctag_cor.clear();
    sys_udsgtag_uncor.clear();
    sys_udsgtag_cor.clear();
    sys_pass.clear();
    //
    trig_jet450=true;
    trig_ht900=true;
    trig_ht1050=true;
    trig_isomu24=true;
    trig_isomu27=true;
    pass_hbheiso=true;
    //
    gen_weight=999;

    // apply json in data
    if(isData && !inJSON(VRunLumi,run,ls)) continue;

    //
    // global variables 
    //
    event   = event_;
    run     = run_;
    ls      = ls_;
    met     = MET_pt;
    met_phi = MET_phi; 

    ntrupv = Pileup_nPU;
    ntrupv_mean  = Pileup_nTrueInt;
    lhe_ht = LHE_HTIncoming;

    if(!isData && (year=="2016" || year=="2017" || year=="UL2016_preVFP" || year=="UL2016" || year=="UL2017")){
      l1pre_nom = L1PreFiringWeight_Nom;
      l1pre_dn = L1PreFiringWeight_Dn;
      l1pre_up = L1PreFiringWeight_Up;
    }
    else if(!isData && year == "2018" || year=="UL2018"){
      l1pre_nom = 1;
      l1pre_dn = 1;
      l1pre_up = 1;
    }

    bool pnf = true;
//    pnf = ProblematicEvent(inputfile, event);
//    if(!pnf) continue;

    //
    // get electrons
    //

    vector<float> els_SFner;
    els_SFner.clear();
    float sys_lep_up   = 1;
    float sys_lep_down = 1;

    for(int iE = 0; iE < nElectron; iE++) // error point in 2018 year FIXME
    {
      els_pt.push_back(Electron_pt[iE]); 
      els_eta.push_back(Electron_eta[iE]); 
      els_phi.push_back(Electron_phi[iE]); 
      els_dz.push_back(Electron_dz[iE]);
      els_d0.push_back(Electron_dxy[iE]);
//      els_sigid.push_back(idElectron_cutBased(Electron_cutBased[iE], 3));	  // 3 = medium 
      els_sigid.push_back(Electron_mvaFall17V2noIso_WP90[iE]);
      els_miniso.push_back(Electron_miniPFRelIso_all[iE]); 
      els_reliso.push_back(Electron_pfRelIso03_all[iE]); 
      if(Electron_pt[iE]<20)  continue;           
      if(abs(Electron_eta[iE])>2.5)  continue;           
      if((abs(Electron_eta[iE])>1.442) && (abs(Electron_eta[iE])<1.556)) continue;
      //if(Electron_cutBased[iE]<3) continue;    // medium WP
      if(!Electron_mvaFall17V2noIso_WP90[iE]) continue;
      if(Electron_miniPFRelIso_all[iE]>0.1) continue; // miniso
      if(Electron_dz[iE]>0.05) continue;
      if(Electron_dxy[iE]>0.1) continue;
      els_SFner    =  getLepSF(electronSF, true, Electron_pt[iE], Electron_eta[iE], year);
      w_lep        *= els_SFner.at(0);
      sys_lep_up   *= (els_SFner.at(0)+els_SFner.at(1));
      sys_lep_down *= (els_SFner.at(0)-els_SFner.at(1));
      els_SFner.clear();

//      cout<<sys_lep_up<<" "<<sys_lep_down<<" ";

      nels++;
      leps_pt.push_back(Electron_pt[iE]); 
      leps_eta.push_back(Electron_eta[iE]); 
      leps_phi.push_back(Electron_phi[iE]); 
      leps_pdgid.push_back(Electron_pdgId[iE]); 
      leps_miniso.push_back(Electron_miniPFRelIso_all[iE]); 
      nleps++;
    } 
    
		//
    // get muons
    //

    vector<float> mus_SFner;
    mus_SFner.clear();

    for(int iM = 0; iM < nMuon; iM++)
    {
      mus_pt.push_back(Muon_pt[iM]); 
      mus_eta.push_back(Muon_eta[iM]); 
      mus_phi.push_back(Muon_phi[iM]); 
      //mus_sigid.push_back(Muon_mediumId[iM]); 
      mus_sigid.push_back(Muon_mediumPromptId[iM]); 
      mus_miniso.push_back(Muon_miniPFRelIso_all[iM]); 
      mus_d0.push_back(Muon_dz[iM]);
      mus_dz.push_back(Muon_dxy[iM]);
      if(Muon_pt[iM]<20)  continue;           
      if(abs(Muon_eta[iM])>2.4)  continue;           
      //if(!Muon_mediumId[iM]) continue;                // medium WP
      if(!Muon_mediumPromptId[iM]) continue;                // medium WP
      if(Muon_miniPFRelIso_all[iM]>0.2) continue;     // miniso 
      if(Muon_dz[iM]>0.05) continue;
      if(Muon_dxy[iM]>0.1) continue;
      
      mus_SFner    =  getLepSF(muonSF, false, Muon_pt[iM], Muon_eta[iM], year);
      w_lep        *= mus_SFner.at(0);
      sys_lep_up   *= (mus_SFner.at(0)+mus_SFner.at(1));
      sys_lep_down *= (mus_SFner.at(0)-mus_SFner.at(1));
      mus_SFner.clear();

//      cout<<sys_lep_up<<" "<<sys_lep_down<<" "<<endl;

      nmus++;
      leps_pt.push_back(Muon_pt[iM]); 
      leps_eta.push_back(Muon_eta[iM]); 
      leps_phi.push_back(Muon_phi[iM]); 
//      leps_phi.push_back(Muon_mass[iM]); 
      leps_pdgid.push_back(Muon_pdgId[iM]);
      leps_miniso.push_back(Muon_miniPFRelIso_all[iM]); 
      nleps++;
    }
    sys_lep.push_back(sys_lep_up);
    sys_lep.push_back(sys_lep_down);

 //   if(nleps!=0){cout<< nleps << " " << endl;} 
    //
    // get jets
    //
    // systematics up/down jets_pt: not stored in the babies
    std::vector<float> sys_jets_pt_up;
    std::vector<float> sys_jets_pt_down;
    std::vector<float> sys_jets_pt_jerUp;
    std::vector<float> sys_jets_pt_jerDown;
    sys_jets_pt_up.clear();
    sys_jets_pt_down.clear(); 
    sys_jets_pt_jerUp.clear();
    sys_jets_pt_jerDown.clear();
    int sys_njets_up = 0;
    int sys_njets_down = 0;
    int sys_njets_jerUp = 0;
    int sys_njets_jerDown = 0;
    int sys_nbm_up = 0;
    int sys_nbm_down = 0;
    int sys_nbm_jerUp = 0;
    int sys_nbm_jerDown = 0;
    float sys_ht_up = 0;
    float sys_ht_down = 0;
    float sys_ht_jerUp = 0;
    float sys_ht_jerDown = 0;
    float sys_mj12_up = 0;
    float sys_mj12_down = 0;
    float sys_mj12_jerUp = 0;
    float sys_mj12_jerDown = 0;
    float sys_bctag_uncor_up = 1;
    float sys_bctag_uncor_down = 1;
    float sys_bctag_cor_up = 1;
    float sys_bctag_cor_down = 1;
    float sys_udsgtag_uncor_up = 1;
    float sys_udsgtag_uncor_down = 1;
    float sys_udsgtag_cor_up = 1;
    float sys_udsgtag_cor_down = 1;
    bool hem_tf;
    // ref of HEM issue: https://twiki.cern.ch/twiki/bin/viewauth/CMS/SUSRecommendationsRun2UltraLegacy#HEM_issue_in_2018
    for(int iJ = 0; iJ < nJet; iJ++) 
    {
      hem_tf = false;
      if(year=="2018"||year=="UL2018"){//HEM effect in 2018 year
        if(isData && run>=319077 && Jet_eta[iJ]>-3.0 && Jet_eta[iJ]<-1.3 && Jet_phi[iJ]>-1.57 && Jet_phi[iJ]<-0.87) hem_tf = true;
//        if(isData /*&& run>=319077*/ && Jet_eta[iJ]>-3.0 && Jet_eta[iJ]<-1.3 && Jet_phi[iJ]>-1.57 && Jet_phi[iJ]<-0.87) hem_tf = true; // for HEM study
        if(!isData && (event%10>=0 && event%10<=5) && Jet_eta[iJ]>-3.0 && Jet_eta[iJ]<-1.3 && Jet_phi[iJ]>-1.57 && Jet_phi[iJ]<-0.87) hem_tf = true;
      }
      if(abs(Jet_eta[iJ])>6 || iJ > 50){//FIXME
//        cout<< "-- "<< Jet_pt[iJ] << " : " << Jet_eta[iJ] << " : " << Jet_phi[iJ] << " : " << Jet_m[iJ] << " : " << Jet_hadronFlavour[iJ] << " : " << Jet_btagCSVV2[iJ] << " : " << Jet_btagDeepB[iJ] << " : " << Jet_btagDeepC[iJ] << " : " << event << " --" << nJet << endl;
        cout<< "-- "<< Jet_pt[iJ] << " : " << Jet_eta[iJ] << " : " << Jet_phi[iJ] << " : " << Jet_m[iJ] << " : " << Jet_hadronFlavour[iJ] << " : " << Jet_btagCSVV2[iJ] << " : " << Jet_btagDeepB[iJ] << " : " <<  event << " --" << nJet << endl;

      }
      jets_pt.push_back(Jet_pt[iJ]); 
      jets_eta.push_back(Jet_eta[iJ]);
      jets_phi.push_back(Jet_phi[iJ]); 
      jets_m.push_back(Jet_m[iJ]); 
      jets_hflavor.push_back(Jet_hadronFlavour[iJ]); 
      jets_csv.push_back(Jet_btagCSVV2[iJ]); 
      jets_dcsvb.push_back(Jet_btagDeepB[iJ]); 
      jets_qgl.push_back(Jet_qgl[iJ]);
//      jets_dcsvc.push_back(Jet_btagDeepC[iJ]);
      jets_hem.push_back(hem_tf);

      sys_jets_pt_jerUp.push_back(Jet_pt_jerUp[iJ]);
      sys_jets_pt_jerDown.push_back(Jet_pt_jerDown[iJ]);
      jets_pt_jerUp.push_back(Jet_pt_jerUp[iJ]);
      jets_pt_jerDown.push_back(Jet_pt_jerDown[iJ]);

      bool jetid = true;
      if(year=="2016" && Jet_jetId[iJ]<3 ) jetid=false; // tight Id  #FIXME 230321: when we start using UL, it(3->2) must be changed!!  (ref: https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookNanoAOD#Jets)
      if((year=="2017"||year=="2018"||year=="UL2016_preVFP"||year=="UL2016"||year=="UL2017"||year=="UL2018") && Jet_jetId[iJ]<2 ) jetid=false; // tight Id 
      jets_id.push_back(jetid);

      bool jetislep = false;
      jetislep = jetIsLepton(Jet_eta[iJ], Jet_phi[iJ], leps_eta, leps_phi);
      jets_islep.push_back(jetislep);
 
      //JEC systematics 
      if(0)
      {
        JetCorrector->setJetPt(Jet_pt[iJ]*(1-Jet_rawFactor[iJ]));
        JetCorrector->setJetEta(Jet_eta[iJ]);
        JetCorrector->setJetA(Jet_area[iJ]);
        JetCorrector->setRho(fixedGridRhoFastjetAll);
        //sys_jets_pt.push_back(JetCorrector->getCorrection()*Jet_pt[iJ]*(1-Jet_rawFactor[iJ]));
      } 

      JecUnc->setJetPt(Jet_pt[iJ]); // here you must use the CORRECTED jet pt
      // Contain Jet_eta within -5.4 ~ 5.4
      float Jet_eta_lessthan5p4 = Jet_eta[iJ];
      if(Jet_eta_lessthan5p4>5.4) Jet_eta_lessthan5p4=5.39; 
      if(Jet_eta_lessthan5p4<-5.4) Jet_eta_lessthan5p4=-5.39; 
      JecUnc->setJetEta(Jet_eta_lessthan5p4);
      float jec_unc = JecUnc->getUncertainty(true);
      sys_jets_pt_up.push_back(Jet_pt[iJ]*(1+jec_unc)); 
      sys_jets_pt_down.push_back(Jet_pt[iJ]*(1-jec_unc)); 

      // more jet selection
      if(abs(jets_eta.at(iJ))>2.4) continue;
      if(!jetid)               continue; 
      if(jetislep)             continue; 
      if(jets_hem.at(iJ))	continue;

      // deepCSV  cuts
      float csv_cut;
      if(year=="2016") csv_cut = 0.6321; 
      else if(year=="2017") csv_cut = 0.4941;
      else if(year=="2018") csv_cut = 0.4184;
      else if(year=="UL2016_preVFP") csv_cut = 0.6001;
      else if(year=="UL2016") csv_cut = 0.5847;
      else if(year=="UL2017") csv_cut = 0.4506;
      else if(year=="UL2018") csv_cut = 0.4168;
      
//cout<<iJ<<" , " << jets_pt.size()<<endl;
      // nominal 
      if(jets_pt.at(iJ)>30)
      {
        njets++;
        ht += Jet_pt[iJ];
        if(Jet_btagDeepB[iJ]>csv_cut) nbm++; 
        if(!isData)
	{
	  w_btag_dcsv            *= getBtagWeight(f_btef,calibreader, Jet_pt[iJ], Jet_eta[iJ], Jet_hadronFlavour[iJ], Jet_btagDeepB[iJ], csv_cut, "central");
	  sys_bctag_uncor_up     *= getBtagWeight(f_btef,calibreader, Jet_pt[iJ], Jet_eta[iJ], Jet_hadronFlavour[iJ], Jet_btagDeepB[iJ], csv_cut, "up_uncorrelated");
	  sys_bctag_uncor_down   *= getBtagWeight(f_btef,calibreader, Jet_pt[iJ], Jet_eta[iJ], Jet_hadronFlavour[iJ], Jet_btagDeepB[iJ], csv_cut, "down_uncorrelated");
	  sys_bctag_cor_up       *= getBtagWeight(f_btef,calibreader, Jet_pt[iJ], Jet_eta[iJ], Jet_hadronFlavour[iJ], Jet_btagDeepB[iJ], csv_cut, "up_correlated");
	  sys_bctag_cor_down     *= getBtagWeight(f_btef,calibreader, Jet_pt[iJ], Jet_eta[iJ], Jet_hadronFlavour[iJ], Jet_btagDeepB[iJ], csv_cut, "down_correlated");
	  sys_udsgtag_uncor_up   *= getBtagWeight(f_btef,calibreader, Jet_pt[iJ], Jet_eta[iJ], Jet_hadronFlavour[iJ], Jet_btagDeepB[iJ], csv_cut, "up_uncorrelated");
	  sys_udsgtag_uncor_down *= getBtagWeight(f_btef,calibreader, Jet_pt[iJ], Jet_eta[iJ], Jet_hadronFlavour[iJ], Jet_btagDeepB[iJ], csv_cut, "down_uncorrelated");
	  sys_udsgtag_cor_up     *= getBtagWeight(f_btef,calibreader, Jet_pt[iJ], Jet_eta[iJ], Jet_hadronFlavour[iJ], Jet_btagDeepB[iJ], csv_cut, "up_correlated");
	  sys_udsgtag_cor_down   *= getBtagWeight(f_btef,calibreader, Jet_pt[iJ], Jet_eta[iJ], Jet_hadronFlavour[iJ], Jet_btagDeepB[iJ], csv_cut, "down_correlated");
	}
      }
      //      cout<<w_btag_dcsv<<endl;
      // jec syst up 
      if(sys_jets_pt_up.at(iJ)>30)
      {
        sys_njets_up++;
        sys_ht_up += sys_jets_pt_up.at(iJ);
        if(Jet_btagDeepB[iJ]>csv_cut) sys_nbm_up++;
      }
      // jec syst down 
      if(sys_jets_pt_down.at(iJ)>30)
      {
        sys_njets_down++;
        sys_ht_down += sys_jets_pt_down.at(iJ);
        if(Jet_btagDeepB[iJ]>csv_cut) sys_nbm_down++;
      }

      // jer syst up
      if(sys_jets_pt_jerUp.at(iJ)>30){
	sys_njets_jerUp++;
	sys_ht_jerUp += sys_jets_pt_jerUp.at(iJ);
        if(Jet_btagDeepB[iJ]>csv_cut) sys_nbm_jerUp++;
      }
      // jer syst down
      if(sys_jets_pt_jerDown.at(iJ)>30){
	sys_njets_jerDown++;
	sys_ht_jerDown += sys_jets_pt_jerDown.at(iJ);
        if(Jet_btagDeepB[iJ]>csv_cut) sys_nbm_jerDown++;
      }
    }

    // weight systematics 
    // 1. renorm/factorization scales
    // Float_t LHE scale variation weights (w_var / w_nominal); 
    // [0] is mur=0.5 muf=0.5 ; 
    // [1] is mur=0.5 muf=1 ; 
    // [2] is mur=0.5 muf=2 ; 
    // [3] is mur=1 muf=0.5 ; 
    // [4] is mur=1 muf=1 ; 
    // [5] is mur=1 muf=2 ; 
    // [6] is mur=2 muf=0.5 ; 
    // [7] is mur=2 muf=1 ; 
    // [8] is mur=2 muf=2 *
    // 2 is up, 0.5 is down

    // These samples have a different definition of LHEScaleWeight compared to other samples
    if(samplename.Contains("ST_s-channel_4f_hadronicDecays_") || samplename.Contains("TTJets_TuneCP5_")) {
      sys_mur.push_back(LHEScaleWeight[6]);
      sys_mur.push_back(LHEScaleWeight[1]);	
      sys_muf.push_back(LHEScaleWeight[4]);
      sys_muf.push_back(LHEScaleWeight[3]);
      sys_murf.push_back(LHEScaleWeight[7]);
      sys_murf.push_back(LHEScaleWeight[0]);
    }
    // These samples do not have a branch \"LHEScaleWeight\"
    // Systs. for mur/muf/murf in these samples are obtained using make_other_mu_syst.cxx in RPV analysis code
    else if(samplename.Contains("WW_TuneCP5_13TeV-pythia8") || samplename.Contains("WZ_TuneCP5_13TeV-pythia8") || samplename.Contains("ZZ_TuneCP5_13TeV-pythia8")) {
      sys_mur.push_back(1);
      sys_mur.push_back(1);	
      sys_muf.push_back(1);
      sys_muf.push_back(1);
      sys_murf.push_back(1);
      sys_murf.push_back(1);
    }
    else {
      sys_mur.push_back(LHEScaleWeight[7]);
      sys_mur.push_back(LHEScaleWeight[1]);	
      sys_muf.push_back(LHEScaleWeight[5]);
      sys_muf.push_back(LHEScaleWeight[3]);
      sys_murf.push_back(LHEScaleWeight[8]);
      sys_murf.push_back(LHEScaleWeight[0]);
    }


    for(int iGen = 0; iGen < nGenPart; iGen++)
    {
      gen_pt.push_back(GenPart_pt[iGen]);
      gen_eta.push_back(GenPart_eta[iGen]);
      gen_phi.push_back(GenPart_phi[iGen]);
      gen_m.push_back(GenPart_mass[iGen]);
      gen_pdgId.push_back(GenPart_pdgId[iGen]);
      gen_PartIdxMother.push_back(GenPart_genPartIdxMother[iGen]);
      gen_status.push_back(GenPart_status[iGen]);
      gen_statusFlags.push_back(GenPart_statusFlags[iGen]);
      ngen++;
    }

    // 
    // Fatjet reconstruction 
    // 

    // ---------------------------------------
    //          Block 1 
    // ---------------------------------------

    // Loop over R=0.5 jets, form into PseudoJets vector
    vector<fastjet::PseudoJet> input_particles; input_particles.clear();
    double FatjetConstituent_px_tmp, FatjetConstituent_py_tmp, FatjetConstituent_pz_tmp, FatjetConstituent_energy_tmp;

    for(int iJ = 0; iJ < jets_pt.size(); iJ++) { 

      TLorentzVector JetLV; 
      JetLV.SetPtEtaPhiM(jets_pt.at(iJ), jets_eta.at(iJ), jets_phi.at(iJ), jets_m.at(iJ));

      // jet selection
      if(jets_pt.at(iJ)<30)           continue;
      if(abs(jets_eta.at(iJ))>2.4)    continue;
      if(jets_id.at(iJ)==false)       continue;
      if(jets_hem.at(iJ)) continue;                    // jetislep is not applied in order to cluster MJ with lepton

      input_particles.push_back(fastjet::PseudoJet(JetLV.Px(), JetLV.Py(), JetLV.Pz(), JetLV.E()));
      h2->Fill(JetLV.Eta(), JetLV.Phi(), JetLV.E());
    }


    // ---------------------------------------
    //          Block 2 
    // ---------------------------------------

    //
    // Run Fastjet to reconstuct jets 
    //

    // RH create an object that represents your choice of jet algorithm and 
    // the associated parameters
    fastjet::Strategy strategy = fastjet::Best;
    fastjet::RecombinationScheme recomb_scheme = fastjet::E_scheme;
    fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, Rparam, recomb_scheme, strategy);

    // run the jet clustering with the above jet definition
    fastjet::ClusterSequence clust_seq(input_particles, jet_def);

    // ---------------------------------------
    //          Block 3 
    // ---------------------------------------

    // 
    // Get p4 of the reconstructed jets  
    //
    //RH
    double ptmin = 0.0; // could use 3.0 here, instead of applying later 
    vector<fastjet::PseudoJet> inclusive_jets; inclusive_jets.clear();
    inclusive_jets = clust_seq.inclusive_jets(ptmin);
    //Sort by pt
    vector<fastjet::PseudoJet> sorted_jets; sorted_jets.clear();
    sorted_jets = sorted_by_pt(inclusive_jets);
    //fill fastjet output into vectors, continue as original code
    for(int isortjets = 0; isortjets< (int)sorted_jets.size(); isortjets++){
      //store only if pt >3 GeV to match CMS jets
      if(TMath::Sqrt( sorted_jets[isortjets].px()*sorted_jets[isortjets].px()
            +sorted_jets[isortjets].py()*sorted_jets[isortjets].py())>(DEBUG?0:3)) {
        toptag_md_ttvsqcd.push_back(FatJet_deepTagMD_TvsQCD[isortjets]);
        toptag_ttvsqcd.push_back(FatJet_deepTag_TvsQCD[isortjets]);
        fjets_pt.push_back(sorted_jets[isortjets].pt());
        fjets_eta.push_back(sorted_jets[isortjets].eta());
        fjets_m.push_back(sorted_jets[isortjets].m());
        fjets_phi.push_back( sorted_jets[isortjets].phi()<TMath::Pi()?sorted_jets[isortjets].phi():sorted_jets[isortjets].phi()-2*TMath::Pi()); 
        fjets_nconst.push_back(sorted_jets[isortjets].constituents().size()); 
        mj12 += sorted_jets[isortjets].m();

        //
        // Link the constituents of fatjet
        //
        // Loop over constituents of a given fatjet and the jet_AK5PF branches 
        // and store the indices of matching jet_AK5PF.
        // Mathing is done based on px, py, pz and energy 
        // requiring each of them is as close as 0.01
        //

        //                vector<int> vec_constituentsindex;
        //                vec_constituentsindex.clear();
        //                vector<fastjet::PseudoJet> constituents = sorted_jets[isortjets].constituents();
        //                for(int iconstituent = 0; iconstituent < (int)constituents.size(); iconstituent++) { 
        //                    for(int ijet = 0; ijet < (int)FatjetConstituent_px_->size(); ijet++) { 
        //                        FatjetConstituent_px_tmp = FatjetConstituent_px_->at(ijet);
        //                        FatjetConstituent_py_tmp = FatjetConstituent_py_->at(ijet);
        //                        FatjetConstituent_pz_tmp = FatjetConstituent_pz_->at(ijet);
        //                        FatjetConstituent_energy_tmp = FatjetConstituent_energy_->at(ijet);
        //
        //                        if(TMath::Sqrt( FatjetConstituent_px_tmp*FatjetConstituent_px_tmp
        //                                    +FatjetConstituent_py_tmp*FatjetConstituent_py_tmp) < 30) continue;
        //                        if( TMath::Abs(FatjetConstituent_px_tmp - constituents[iconstituent].px())<0.01  
        //                                &&TMath::Abs(FatjetConstituent_py_tmp - constituents[iconstituent].py())<0.01 
        //                                &&TMath::Abs(FatjetConstituent_pz_tmp - constituents[iconstituent].pz())<0.01 
        //                                &&TMath::Abs(FatjetConstituent_energy_tmp - constituents[iconstituent].E())<0.01) { 
        //                            if(DEBUG) cout << "Constituent Index : " << ijet << endl;
        //                            vec_constituentsindex.push_back(ijet);
        //                        }
        //                    }
        //                } //for(int iconstituent = 0; iconstituent < (int)constituent->size(); iconstituent++)
        //                Fatjet_ConstituentIndex->push_back(vec_constituentsindex);
        //                
        // DEBUGGING
        if(DEBUG) {
          cout << event << " " 
            << TMath::Sqrt( sorted_jets[isortjets].px()*sorted_jets[isortjets].px()
                +sorted_jets[isortjets].py()*sorted_jets[isortjets].py()) << " " 
            << sorted_jets[isortjets].eta() << " " 
            << (sorted_jets[isortjets].phi()<TMath::Pi() ? sorted_jets[isortjets].phi() : (sorted_jets[isortjets].phi()-2*TMath::Pi())) << " "  
            << endl; 
        }
      }

//    if(sys_jets_pt_up[isortjets]<0 || sys_jets_pt_up[isortjets]>1000000) {
//      cout << "--------------------------------------------------------------------------" << endl;
//      cout << "--------------------------------------------------------------------------" << endl;
//      cout << "sys_jets_pt_up["<<isortjets<<"]: "<<sys_jets_pt_up[isortjets] << endl;
//      cout << "sys_jets_pt_down["<<isortjets<<"]: "<<sys_jets_pt_down[isortjets] << endl;
//      cout << "sys_jets_pt_jerUp["<<isortjets<<"]: "<<sys_jets_pt_jerUp[isortjets] << endl;
//      cout << "sys_jets_pt_jerDown["<<isortjets<<"]: "<<sys_jets_pt_jerDown[isortjets] << endl;
//      cout << "--------------------------------------------------------------------------" << endl;
//      cout << "--------------------------------------------------------------------------" << endl;
//    }

    } 

		//
		// systematics 
		//

    if(!isData){
		// JEC systematics (fill only for MC)
    sys_mj12_up   =  getMJ(sys_jets_pt_up, jets_eta, jets_phi, jets_m, jets_id, jets_hem);
    sys_mj12_down =  getMJ(sys_jets_pt_down, jets_eta, jets_phi, jets_m, jets_id, jets_hem);

    sys_mj12_jerUp   =  getMJ(sys_jets_pt_jerUp, jets_eta, jets_phi, jets_m, jets_id, jets_hem);
    sys_mj12_jerDown =  getMJ(sys_jets_pt_jerDown, jets_eta, jets_phi, jets_m, jets_id, jets_hem);


    // fill jec syst branches: vec.at(0) is up and vec.at(1) is down 
    // fill jer syst branches: vec.at(2) is up and vec.at(3) is down
    sys_njets.push_back(sys_njets_up);	sys_njets.push_back(sys_njets_down);	
    sys_njets.push_back(sys_njets_jerUp); sys_njets.push_back(sys_njets_jerDown);	
    sys_nbm.push_back(sys_nbm_up);	sys_nbm.push_back(sys_nbm_down);	
    sys_nbm.push_back(sys_nbm_jerUp);	sys_nbm.push_back(sys_nbm_jerDown);	
    sys_ht.push_back(sys_ht_up);	sys_ht.push_back(sys_ht_down);	
    sys_ht.push_back(sys_ht_jerUp);	sys_ht.push_back(sys_ht_jerDown);	
    sys_mj12.push_back(sys_mj12_up);	sys_mj12.push_back(sys_mj12_down);	
    sys_mj12.push_back(sys_mj12_jerUp);	sys_mj12.push_back(sys_mj12_jerDown);	

    // btagging
    sys_bctag_uncor.push_back(sys_bctag_uncor_up);	sys_bctag_uncor.push_back(sys_bctag_uncor_down);	
    sys_bctag_cor.push_back(sys_bctag_cor_up);	        sys_bctag_cor.push_back(sys_bctag_cor_down);	
    sys_udsgtag_uncor.push_back(sys_udsgtag_uncor_up);	sys_udsgtag_uncor.push_back(sys_udsgtag_uncor_down);	
    sys_udsgtag_cor.push_back(sys_udsgtag_cor_up);	sys_udsgtag_cor.push_back(sys_udsgtag_cor_down);	
    }

    if(!isData){
      // Since UL samples use CP5 tune, there is no need for ISR systematics
      int nisr_(0);
      TLorentzVector JetLV_, GenLV_; 
      for(size_t ijet(0); ijet<jets_pt.size(); ijet++){
        bool matched_ = false;
        if(jets_pt.at(ijet)<30) continue;
        if(abs(jets_eta.at(ijet))>2.4) continue;
        if(jets_id.at(ijet)==0) continue;
        if(jets_islep.at(ijet)==1) continue;
	if(jets_hem.at(ijet)==1) continue;

        JetLV_.SetPtEtaPhiM(jets_pt.at(ijet), jets_eta.at(ijet), jets_phi.at(ijet), jets_m.at(ijet));

        for(size_t imc(0); imc < gen_pt.size(); imc++){
          if((gen_PartIdxMother.at(imc))==-1) continue;
          int momid = abs(gen_pdgId.at(gen_PartIdxMother.at(imc)));
 
          if(!((gen_statusFlags.at(imc)>>7)&1) || abs(gen_pdgId.at(imc))>5) continue;
 
          if(!(momid==6 || momid==23 || momid==24 || momid==25 || momid>1e6)) continue;//6: top, 23: Z boson, 24: W boson, 25: Higgs, 1e6<: SUSY particle ---> matching condition is final state Jets.
          GenLV_.SetPtEtaPhiM(gen_pt.at(imc), gen_eta.at(imc), gen_phi.at(imc), gen_m.at(imc));
          float dR = JetLV_.DeltaR(GenLV_);//dR=sqrt(dphi^2+deta^2)
          if(dR<0.3){
            matched_ = true;
            matched = matched_;
          }
        }
        if(matched_==false) nisr_++;//--> not matched with final state.
      }

      TLorentzVector jets_1, jets_2;
      for(size_t imc(0); imc<gen_pt.size(); imc++){
        //cout<<"gen_idxMother: "<<gen_PartIdxMother.at(imc)<<endl;
        if((gen_PartIdxMother.at(imc))==-1) continue;
        int momid = abs(gen_pdgId.at(gen_PartIdxMother.at(imc)));

        if(!((momid >= 1000022 && momid <= 1000025) || momid==1000035 || momid==1000037)) continue; //neutralino
        if(!(abs(gen_pdgId.at(imc))==3 || abs(gen_pdgId.at(imc))==5 || abs(gen_pdgId.at(imc))==6)) continue;//tbs
        jets_1.SetPtEtaPhiM(gen_pt.at(imc), gen_eta.at(imc), gen_phi.at(imc), gen_m.at(imc));

        for(size_t igen(0); igen<gen_pt.size(); igen++){
	  jets_2.SetPtEtaPhiM(gen_pt.at(igen), gen_eta.at(igen), gen_phi.at(igen), gen_m.at(igen));
	  if(jets_1!=jets_2) llp_dR=jets_1.DeltaR(jets_2);
	  //cout<<"deltaR: "<<llp_dR<<endl;
        }
      }

      if(nisr_==0)       isr_wgt = 1.; 
      else if(nisr_==1)  isr_wgt = 0.920; 
      else if(nisr_==2)  isr_wgt = 0.821; 
      else if(nisr_==3)  isr_wgt = 0.715; 
      else if(nisr_==4)  isr_wgt = 0.662; 
      else if(nisr_==5)  isr_wgt = 0.561; 
      else if(nisr_>=6)  isr_wgt = 0.511; 

      //w_isr = isr_wgt*isr_norm;
      if(year=="2016"){
        w_isr = isr_wgt;
        sys_isr.push_back(w_isr+((1-w_isr)/2));
        sys_isr.push_back(w_isr-((1-w_isr)/2));
      }
      if(year=="2017"||year=="2018"||year=="UL2016_preVFP"||year=="UL2016"||year=="UL2017"||year=="UL2018"){
        // Since UL samples use CP5 tune, there is no need for ISR systematics
        w_isr = 1;
        sys_isr.push_back(+0);
        sys_isr.push_back(-0);
        /*    // Now, Tune version of signal sample is CP5 (241029).
        if(inputfile.Contains("SMS-T1tbs_RPV")){     // Tune version of signal sample is CP2. ISR corrections derived for samples with TuneCP2 are similar to the ones for 2016-TuneCUETP8M1. So it is recommended to use 2016 corrections.
          w_isr = isr_wgt;
          sys_isr.push_back(w_isr+((1-w_isr)/2));
          sys_isr.push_back(w_isr-((1-w_isr)/2));
        }
        else{
	  w_isr = 1;
          sys_isr.push_back(+0);
          sys_isr.push_back(-0);
        }
        */

      }
      nisr = nisr_;
    }

    if(!isData){
      for(int imc(0); imc<ngen; imc++){
        if(gen_PartIdxMother.at(imc)==-1) continue;//exclude incoming particle
  	int momid = abs(gen_pdgId.at(gen_PartIdxMother.at(imc)));
  	int momstat = gen_status.at(gen_PartIdxMother.at(imc));
  	int genId = abs(gen_pdgId.at(imc));
  
  	if(gen_PartIdxMother.at(imc)!=0 && (momstat<=21 || momstat>=29) && momid!=4 && momid!=5){ // momid 4 and 5 is come from hard process
  	  if(momid==21 && (genId==5 || genId==4)){
  	    fromGS = true;//gluon split to b quark
  	  }
        }
      }
    }

    // 
    // weights 
    //
    if(!isData)
    {
      w_btag_csv = btagWeight_CSVV2;
      w_lumi     = xsec*genWeight/sumWeights;//getXsec(samplename)*genWeight/sumWeights; // cross section in fb
      w_pu       = getPUweight(f_pu_weight, year, ntrupv_mean, 0); // syst=-1 0 1 (down nominal up)
      sys_pu.push_back(getPUweight(f_pu_weight, year, ntrupv_mean, 1));
      sys_pu.push_back(getPUweight(f_pu_weight, year, ntrupv_mean, -1));
//      w_pu       = getPUweight(samplename, year, ntrupv_mean, 0); // syst=-1 0 1 (down nominal up)
//      sys_pu.push_back(getPUweight(samplename, year, ntrupv_mean, 1));
//      sys_pu.push_back(getPUweight(samplename, year, ntrupv_mean, -1));
    }

    if(inputfile.Contains("TTJets_")){ // Now, Tune version of signal sample is CP5 (241029). 
//    if(inputfile.Contains("SMS-T1tbs_RPV") || inputfile.Contains("TTJets_")){  //FIXME Why it includes "TTJets"? -> Cuz in PL, TTbar and Signal samples are only needed to be applied ISR correction
      if(year=="2016" || year=="2017" || year=="UL2016_preVFP"||year=="UL2016"||year=="UL2017") weight = w_btag_dcsv * w_lumi * w_pu * w_isr * l1pre_nom * w_lep;
      else weight = w_btag_dcsv * w_lumi * w_pu * w_isr * w_lep;
    }
    else {
      if(year=="2016" || year=="2017"||year=="UL2016_preVFP"||year=="UL2016"||year=="UL2017") weight = w_btag_dcsv * w_lumi * w_pu * l1pre_nom * w_lep;
      else weight = w_btag_dcsv * w_lumi * w_pu * w_lep;
      w_isr = 1;
    }
    if(isData) 
    {
      w_btag_csv  = 1;
      w_btag_dcsv = 1;
      w_lumi      = 1;
      w_pu        = 1;
      weight = 1;
    }
    if((inputfile.Contains("TTJets_Tune")) && lhe_ht>600) stitch_ht = false;

    // filters and triggers 
    //https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2
    if(year=="2016")
    {
      pass = Flag_goodVertices*
        Flag_globalSuperTightHalo2016Filter*
        Flag_HBHENoiseFilter*
        Flag_HBHENoiseIsoFilter*
        Flag_EcalDeadCellTriggerPrimitiveFilter*
        Flag_BadPFMuonFilter;

      // triggers 
      trig_ht900  = HLT_PFHT900;
      trig_jet450 = HLT_PFJet450;
    } 
    else if(year=="2017"||year=="2018")
    {
      pass = Flag_goodVertices*
        Flag_globalSuperTightHalo2016Filter*
        Flag_HBHENoiseFilter*
        Flag_HBHENoiseIsoFilter*
        Flag_EcalDeadCellTriggerPrimitiveFilter*
        Flag_BadPFMuonFilter*
        Flag_eeBadScFilter;
        
      // triggers 
      trig_ht1050 = HLT_PFHT1050;
    }
    else if(year=="UL2016_preVFP"||year=="UL2016")
    {
      pass = Flag_goodVertices*
        Flag_globalSuperTightHalo2016Filter*
        Flag_HBHENoiseFilter*
        Flag_HBHENoiseIsoFilter*
        Flag_EcalDeadCellTriggerPrimitiveFilter*
        Flag_BadPFMuonFilter*
	Flag_BadPFMuonDzFilter*
	Flag_eeBadScFilter;

      // triggers
      trig_ht900  = HLT_PFHT900;
      trig_jet450 = HLT_PFJet450;
    }
    else if(year=="UL2017"||year=="UL2018")
    {
      pass = Flag_goodVertices*
        Flag_globalSuperTightHalo2016Filter*
        Flag_HBHENoiseFilter*
        Flag_HBHENoiseIsoFilter*
        Flag_EcalDeadCellTriggerPrimitiveFilter*
        Flag_BadPFMuonFilter*
        Flag_BadPFMuonDzFilter*
        Flag_eeBadScFilter*
        Flag_ecalBadCalibFilter;

      // triggers
      trig_ht1050 = HLT_PFHT1050;
    }

    trig_isomu24  = HLT_IsoMu24;
    trig_isomu27  = HLT_IsoMu27;
    pass_hbheiso = Flag_HBHENoiseIsoFilter;

      // weight
    gen_weight = genWeight;

   
    // Fill the branches 
    babyTree_->Fill(); // Fill all events

    // 
    // Draw a lego plot (eta, phi) 
    //
    if(DEBUG) {
      TCanvas *c = new TCanvas();
      c->cd(1);
      h2->Draw("colz");
      h2->SetTitle(Form("run=%i lumi=%i event=%llu R=%.1f", run, ls, event, Rparam));
      h2->SetMaximum(50); 
      h2->SetStats(0); 
      h2->SetXTitle("#eta"); 
      h2->SetYTitle("#phi"); 

      //Draw circles around jets
      TEllipse *cone[fjets_eta.size()]; 
      for(int imc=0; imc<(int)fjets_eta.size(); imc++){
        cone[imc] = new TEllipse(fjets_eta.at(imc), fjets_phi.at(imc), Rparam, Rparam);
        cone[imc]->SetFillStyle(3003);
        cone[imc]->SetFillColor(kYellow);
        cone[imc]->Draw();
      }

      c->SaveAs(Form("EtaPhiViewPFCand_Run%i_Lumi%i_Event%llu_R%.1f.pdf", 
            run, ls, event, Rparam));
      h2->Reset(); 
      for(int imc=0; imc<(int)fjets_eta.size(); imc++) delete cone[imc];
      delete c;
    } 
//if(ievt>=400000)cout<<event<<": !!!"<<endl;//FIXME
//  cout << "ievt: " << ievt << endl;
  }// event loop termination


  //
  // Write the baby file 
  //
  if(!DEBUG)
  {
    babyFile_->cd();
    babyTree_->Write();
    babyFile_->Close();
  }

  // update the tree and close file
  f->Close();
  muonSF->Close();
  electronSF->Close();


  //
  // cleanup
  //
  delete f_btef;
  delete electronSF;
  delete muonSF;
  delete f_pu_weight;
//  delete h2;

  // copy output file to outputdir
  cout << "... transferring output file" << endl;
  //cout << Form("... xrdcp %s %s", outputfile.Data(), outputdir.Data()) << endl;  //for KISTI server
  //gSystem->Exec(Form("xrdcp %s %s", outputfile.Data(), outputdir.Data()));  // for KISTI server
  cout << Form("... cp /data3/nanoprocessing/Running/%s %s", outputfile.Data(), outputdir.Data()) << endl;  // for KoreaUniv server
  gSystem->Exec(Form("cp /data3/nanoprocessing/Running/%s %s", outputfile.Data(), outputdir.Data()));  // for KoreaUniv server
  cout << Form("rm /data3/nanoprocessing/Running/%s", outputfile.Data()) << endl;  
  cout << "outputfile.Data(): " << outputfile.Data() << endl;
  gSystem->Exec(Form("rm /data3/nanoprocessing/Running/%s", outputfile.Data()));  

//  delete babyTree_;
//  delete babyFile_;
  delete f;
  cout << "process_nano function is done" << endl;
}









# ifndef __CINT__  // the following code will be invisible for the interpreter
int main(int argc, char **argv)
{
  int nthreads = 16;
//  ROOT::EnableImplicitMT(nthreads);
  bool useCondor = false;
  TString inputdir, outputdir, process, list_processed, year; 
  
  if(argc<6)
  {
    cout << " Please provide proper arguments" << endl;
    cout << "" << endl;
    cout << "   ./process_nano.exe [input dir] [output dir] [process] [list of processed files] [year]" << endl; 
    cout << "   year:   [PreLegacy] 2016, 2017, 2018   [UltraLegacy] UL2016_preVFP, UL2016, UL2017, UL2018" << endl;
    cout << "" << endl;
    return 0;
  }
  else 
  {
    inputdir    	= argv[1];
    outputdir   	= argv[2];
    process     	= argv[3];
    list_processed    	= argv[4];
    year		= argv[5];

    
    cout << "-----------------------------------------------------------------------" << endl;
    cout << " input   dir                  : " << inputdir << endl;
    cout << " output  dir                  : " << outputdir << endl;
    cout << " process                      : " << process << endl;
    cout << " list of processed files      : " << list_processed << endl;
    cout << " year      : " << year << endl;
    cout << "-----------------------------------------------------------------------" << endl;
  }

  // is this data?
  bool isData = false;
  if(inputdir.Contains("Run201") || process.Contains("Run201"))          isData = true;
  if(inputdir.Contains("JetHT") || process.Contains("JetHT"))            isData = true;
  if(inputdir.Contains("SingleMuon") || process.Contains("SingleMuon"))  isData = true;
 
  // year
//  int year    = 0;
//  if(inputdir.Contains("RunIISummer16")) year = 2016;
//  else if(inputdir.Contains("RunIIFall17")) year = 2017;
//  else if(inputdir.Contains("RunIIAutumn18")) year = 2018;
//  if(inputdir.Contains("Run2016")) year = 2016;
//  else if(inputdir.Contains("Run2017")) year = 2017;
//  else if(inputdir.Contains("Run2018")) year = 2018;
  cout << " year: " << year << endl;
  
  // JECs
  if(0)
  {
    JetCorrectorParameters *L1JetPar;
    JetCorrectorParameters *L2JetPar;
    JetCorrectorParameters *L3JetPar;
    JetCorrectorParameters *ResJetPar;

    if(year=="2016"){
      L1JetPar  = new JetCorrectorParameters("data/prelegacy/jec/Summer16_07Aug2017_V11_MC_L1FastJet_AK4PFchs.txt","");
      L2JetPar  = new JetCorrectorParameters("data/prelegacy/jec/Summer16_07Aug2017_V11_MC_L2Relative_AK4PFchs.txt");
      L3JetPar  = new JetCorrectorParameters("data/prelegacy/jec/Summer16_07Aug2017_V11_MC_L3Absolute_AK4PFchs.txt");
      ResJetPar = new JetCorrectorParameters("data/prelegacy/jec/Summer16_07Aug2017_V11_MC_L2L3Residual_AK4PFchs.txt");
    }
    else if(year=="2017"){
      L1JetPar  = new JetCorrectorParameters("data/prelegacy/jec/Fall17_17Nov2017_V32_MC_L1FastJet_AK4PFchs.txt","");
      L2JetPar  = new JetCorrectorParameters("data/prelegacy/jec/Fall17_17Nov2017_V32_MC_L2Relative_AK4PFchs.txt","");
      L3JetPar  = new JetCorrectorParameters("data/prelegacy/jec/Fall17_17Nov2017_V32_MC_L3Absolute_AK4PFchs.txt","");
      ResJetPar = new JetCorrectorParameters("data/prelegacy/jec/Fall17_17Nov2017_V32_MC_L2L3Residual_AK4PFchs.txt","");
    }
    else if(year=="2018"){
      //L1JetPar  = new JetCorrectorParameters("data/prelegacy/jec/Autumn18_V8_MC_L1FastJet_AK4PFchs.txt","");
      //L2JetPar  = new JetCorrectorParameters("data/prelegacy/jec/Autumn18_V8_MC_L2Relative_AK4PFchs.txt","");
      //L3JetPar  = new JetCorrectorParameters("data/prelegacy/jec/Autumn18_V8_MC_L3Absolute_AK4PFchs.txt","");
      //ResJetPar = new JetCorrectorParameters("data/prelegacy/jec/Autumn18_V8_MC_L2L3Residual_AK4PFchs.txt","");
      L1JetPar  = new JetCorrectorParameters("data/prelegacy/jec/Autumn18_V19_MC_L1FastJet_AK4PFchs.txt","");
      L2JetPar  = new JetCorrectorParameters("data/prelegacy/jec/Autumn18_V19_MC_L2Relative_AK4PFchs.txt","");
      L3JetPar  = new JetCorrectorParameters("data/prelegacy/jec/Autumn18_V19_MC_L3Absolute_AK4PFchs.txt","");
      ResJetPar = new JetCorrectorParameters("data/prelegacy/jec/Autumn18_V19_MC_L2L3Residual_AK4PFchs.txt","");
    }
    else if(year=="UL2016_preVFP"){
      L1JetPar  = new JetCorrectorParameters("data/ultralegacy/jec/Summer19UL16APV_V7_MC_L1FastJet_AK4PFchs.txt","");   // We can use Summer19 instead of Summer20. (ref: https://cms-talk.web.cern.ch/t/recommended-jec-jer-for-summer20ul-and-fastsim/1395)
      L2JetPar  = new JetCorrectorParameters("data/ultralegacy/jec/Summer19UL16APV_V7_MC_L2Relative_AK4PFchs.txt","");
      L3JetPar  = new JetCorrectorParameters("data/ultralegacy/jec/Summer19UL16APV_V7_MC_L3Absolute_AK4PFchs.txt","");
      ResJetPar = new JetCorrectorParameters("data/ultralegacy/jec/Summer19UL16APV_V7_MC_L2L3Residual_AK4PFchs.txt","");
    }
    else if(year=="UL2016"){
      L1JetPar  = new JetCorrectorParameters("data/ultralegacy/jec/Summer19UL16_V7_MC_L1FastJet_AK4PFchs.txt","");
      L2JetPar  = new JetCorrectorParameters("data/ultralegacy/jec/Summer19UL16_V7_MC_L2Relative_AK4PFchs.txt","");
      L3JetPar  = new JetCorrectorParameters("data/ultralegacy/jec/Summer19UL16_V7_MC_L3Absolute_AK4PFchs.txt","");
      ResJetPar = new JetCorrectorParameters("data/ultralegacy/jec/Summer19UL16_V7_MC_L2L3Residual_AK4PFchs.txt","");
    }
    else if(year=="UL2017"){
      L1JetPar  = new JetCorrectorParameters("data/ultralegacy/jec/Summer19UL17_V5_MC_L1FastJet_AK4PFchs.txt","");
      L2JetPar  = new JetCorrectorParameters("data/ultralegacy/jec/Summer19UL17_V5_MC_L2Relative_AK4PFchs.txt","");
      L3JetPar  = new JetCorrectorParameters("data/ultralegacy/jec/Summer19UL17_V5_MC_L3Absolute_AK4PFchs.txt","");
      ResJetPar = new JetCorrectorParameters("data/ultralegacy/jec/Summer19UL17_V5_MC_L2L3Residual_AK4PFchs.txt","");
    }
    else if(year=="UL2018"){
      L1JetPar  = new JetCorrectorParameters("data/ultralegacy/jec/Summer19UL18_V5_MC_L1FastJet_AK4PFchs.txt","");
      L2JetPar  = new JetCorrectorParameters("data/ultralegacy/jec/Summer19UL18_V5_MC_L2Relative_AK4PFchs.txt","");
      L3JetPar  = new JetCorrectorParameters("data/ultralegacy/jec/Summer19UL18_V5_MC_L3Absolute_AK4PFchs.txt","");
      ResJetPar = new JetCorrectorParameters("data/ultralegacy/jec/Summer19UL18_V5_MC_L2L3Residual_AK4PFchs.txt","");
    }

    //  Load the JetCorrectorParameter objects into a vector, IMPORTANT: THE ORDER MATTERS HERE !!!! 
    vector<JetCorrectorParameters> vPar; vPar.clear();
    vPar.push_back(*L1JetPar);
    vPar.push_back(*L2JetPar);
    vPar.push_back(*L3JetPar);
    vPar.push_back(*ResJetPar);
    static FactorizedJetCorrector *jetCorrector = 0; 
    JetCorrector = new FactorizedJetCorrector(vPar);

    delete L1JetPar;
    delete L2JetPar;
    delete L3JetPar;
    delete ResJetPar;
  }
  // JEC systs
  if(year=="2016") JecUnc  = new JetCorrectionUncertainty("data/prelegacy/jec/Summer16_07Aug2017_V11_MC_Uncertainty_AK4PFchs.txt");
  else if(year=="2017") JecUnc  = new JetCorrectionUncertainty("data/prelegacy/jec/Fall17_17Nov2017_V32_MC_Uncertainty_AK4PFchs.txt");
  else if(year=="2018") JecUnc  = new JetCorrectionUncertainty("data/prelegacy/jec/Autumn18_V19_MC_Uncertainty_AK4PFchs.txt");
  else if(year=="UL2016_preVFP") JecUnc  = new JetCorrectionUncertainty("data/ultralegacy/jec/Summer19UL16APV_V7_MC_Uncertainty_AK4PFchs.txt");
  else if(year=="UL2016") JecUnc  = new JetCorrectionUncertainty("data/ultralegacy/jec/Summer19UL16_V7_MC_Uncertainty_AK4PFchs.txt");
  else if(year=="UL2017") JecUnc  = new JetCorrectionUncertainty("data/ultralegacy/jec/Summer19UL17_V5_MC_Uncertainty_AK4PFchs.txt");
  else if(year=="UL2018") JecUnc  = new JetCorrectionUncertainty("data/ultralegacy/jec/Summer19UL18_V5_MC_Uncertainty_AK4PFchs.txt");

//  JecUnc = new JetCorrectionUncertainty(Sample_Year::JECUncertFile);

  // get list of files in a directory to calculate w_lumi
  // w_lumi = xsec[fb] * genWeight / sum(genWeights)
  // => https://twiki.cern.ch/twiki/bin/view/Main/CMGMonojetAnalysisTools
  //vector<TString> files = globVector(Form("/xrootd/%s/*/*.root", inputdir.Data())); 
  vector<TString> files; files.clear();
  files = getFileListFromFile(Form("flist/%s/flist_%s.txt", year.Data(), process.Data()));
  vector<TString> files_original; files_original.clear();
  files_original = files; 
  for(int ifile=0; ifile<files.size(); ifile++)
  {
    cout << files.at(ifile) << endl;
    if(useCondor) files.at(ifile).ReplaceAll("/xrootd","root://cms-xrdr.private.lo:2094//xrd");
  }

  cout<<process<<endl;
  float sumWeights = 1;
  int nfiles = files.size();
  int filenumber = 0;
  cout << " Total number of files: " << files.size() << endl; 
  if(!isData)
  {
    TChain *ch = new TChain("Events");
//    TChain ch("Events");
    for(int ifile=0; ifile<files.size(); ifile++)
    {
//      ch.Add(files.at(ifile));
      ch->Add(files.at(ifile));
    }
    //nfiles = ch.GetListOfFiles()->GetEntries(); 
    TH1D *h = new TH1D("h","h",1,-1,1);
//    ch.Draw("0>>h", "genWeight","goff"); 
    ch->Draw("0>>h", "genWeight","goff"); 
    sumWeights = h->Integral();
    cout << " sumWeights: " << sumWeights << endl; 
    cout << "-----------------------------------------------------------------------" << endl;
    delete h;
    delete ch;
  }
  // 
//  vector<TString> files_processed = getFileListFromFile(Form("flist/%s", list_processed.Data())); 
  vector<TString> files_processed; files_processed.clear();
  files_processed = getFileListFromFile(Form("flist/%s/%s", year.Data(), list_processed.Data())); 
  for(int i=0; i<files.size(); i++)
  {
    // check if a file is already there in the output directory
    // if it does, skip it
    bool file_is_processed = false;
    for(int j=0; j<files_processed.size(); j++)
    {
      TObjArray *tokens = files.at(i).Tokenize("/"); 
      TString outputfile = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries()-1)))->GetString();
      outputfile.ReplaceAll(".root", Form("_fatjetbaby_%s.root", process.Data()));
      if(outputfile == files_processed.at(j)) file_is_processed=true;
    }
       
    if(file_is_processed == true) 
    { 
      cout << "file already exists in " << outputdir << endl; 
      cout << "skip it!" << endl;
      continue;
    }
    cout << "processing " << files.at(i) << endl; 
    process_nano(files.at(i), outputdir, sumWeights, process, nfiles, filenumber, year); 
    cout << i << "th file is processed." << endl;
  }
  //delete JetCorrector;
  //delete JecUnc;

  return 0;
}
# endif
