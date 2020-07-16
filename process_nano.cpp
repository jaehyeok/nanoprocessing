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
TFile *electronSF;
TFile *muonSF;

void process_nano(TString inputfile, TString outputdir, float sumWeights, TString samplename, int nfiles, int &filenumber) 
{
  // 
  filenumber++; 

  //
  int year    = 0;
  if(inputfile.Contains("RunIISummer16")) year = 2016;
  else if(inputfile.Contains("RunIIFall17")) year = 2017;
  else if(inputfile.Contains("RunIIAutumn18")) year = 2018;
  if(inputfile.Contains("Run2016")) year = 2016;
  else if(inputfile.Contains("Run2017")) year = 2017;
  else if(inputfile.Contains("Run2018")) year = 2018;
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
  // string csvfile = "data/DeepCSV_2016LegacySF_V1.csv"; //for loose cuts only 
  // string csvfile = "data/DeepCSV_2016LegacySF_V1_TuneCP5.csv"; 
  string csvfile = "data/DeepCSV_2016LegacySF_V1.csv"; //for loose cuts only 
  if(year==2017) csvfile = "data/DeepCSV_94XSF_V4_B_F.csv";
  if(year==2018) csvfile = "data/DeepCSV_102XSF_V1.csv";
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
      {"up", "down"});                                // other sys types
  calibreader.load(calib, BTagEntry::FLAV_B,     "comb");
  calibreader.load(calib, BTagEntry::FLAV_C,     "comb");
  calibreader.load(calib, BTagEntry::FLAV_UDSG,  "incl"); 


	// Lepton SF files
  /*TFile *electronSF;
  if(year==2016) electronSF = new TFile("data/ElectronScaleFactors_Run2016.root","read");
  else if(year==2017) electronSF = new TFile("data/ElectronScaleFactors_Run2017.root","read");
  else if(year==2018) electronSF = new TFile("data/ElectronScaleFactors_Run2018.root","read");*/
  electronSF = new TFile("data/ElectronScaleFactors_Run2016.root","read");
    
	/*TFile *muonSF;
  if(year==2016)muonSF = new TFile("data/TnP_NUM_MiniIsoTight_DENOM_LooseID_VAR_map_pt_eta.root","read");
  else if(year>=2017)muonSF = new TFile("data/2017MiniIso0.2AndMediumID_SF.root","read");// */
  muonSF = new TFile("data/TnP_NUM_MiniIsoTight_DENOM_LooseID_VAR_map_pt_eta.root","read");

  // ------------------------------------------------------------------ 
  // json for DATA 
  // ------------------------------------------------------------------ 
  std::vector< std::vector<int> > VRunLumi; VRunLumi.clear();
  if(isData) 
  {
    string jsonfile;
    if(year==2016) 
    {
      jsonfile = "data/golden_Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16.json";
    }
    else if(year==2017)
    { 
      jsonfile = "data/golden_Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17.json";
    } 
    else if(year==2018)
    {
      jsonfile = "data/golden_Cert_314472-325175_13TeV_PromptReco_Collisions18.json";
    } 
    else
    {
      cout << "[Error] No proper choice of JSON files!!" << endl;
      return ;
    }
    VRunLumi = MakeVRunLumi(jsonfile); 
    cout << "json file : " << jsonfile << endl;       
  } 
  else 
  {
    cout << "[MJ Analysis] No JSON files applied because it is MC" << endl;
  }

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
  Int_t       Pileup_nPU      = 0; 
  Float_t     fixedGridRhoFastjetAll = 0; 
  // weights
  Float_t     btagWeight_CSVV2 = 1;
  Float_t     btagWeight_DeepCSVB = 1;
  Float_t     genWeight        = 1;
  //LHE HT incomming
  Float_t     LHE_HTIncoming = 0;
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
  // leptons 
  UInt_t  nElectron=0;  
  Float_t Electron_pt[25]; 
  Float_t Electron_eta[25];  
  Float_t Electron_phi[25]; 
  Int_t   Electron_cutBased[25];  // cut-based ID Fall17 V2: susy recommendation, use medium(https://github.com/richstu/babymaker/blob/master/bmaker/src/lepton_tools.cc#L289)
  //Int_t   Electron_cutBased_Spring15[25];  // Spring15 ID to compare with AN-2016/187 (1L MJ 2016 data)  
  Int_t   Electron_jetIdx[25];    // index of the associated jet (-1 if none)  
  Int_t   Electron_pdgId[25];   
  Float_t   Electron_miniPFRelIso_all[25];   
  Float_t   Electron_pfRelIso03_all[25];   
  Int_t   Electron_vidNestedWPBitmap[25];   
  //Int_t   Electron_vidNestedWPBitmapSpring15[25];   
  UInt_t  nMuon=0;  
  Float_t Muon_pt[25]; 
  Float_t Muon_eta[25];  
  Float_t Muon_phi[25]; 
  Bool_t   Muon_mediumId[25];  // medium id taken from babymaker: https://github.com/richstu/babymaker/blob/master/bmaker/src/lepton_tools.cc#L190  
  Int_t   Muon_jetIdx[25];    // index of the associated jet (-1 if none)  
  Int_t   Muon_pdgId[25];   
  Float_t   Muon_miniPFRelIso_all[25];   
  // jets
  UInt_t     nJet=0;  
  Float_t    Jet_pt[100];
  Float_t    Jet_eta[100];
  Float_t    Jet_phi[100];
  Float_t    Jet_m[100];
  Float_t    Jet_btagCSVV2[100];
  Float_t    Jet_btagDeepB[100];
  Float_t    Jet_btagDeepC[100];
  Float_t    Jet_rawFactor[100];
  Float_t    Jet_area[100];
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
  Bool_t Flag_eeBadScFilter=true;
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
  tree->SetBranchAddress("LHE_HTIncoming",  &LHE_HTIncoming);
  tree->SetBranchAddress("LHEScaleWeight",  &LHEScaleWeight);
  if(!isData)
  {
    tree->SetBranchAddress("Pileup_nTrueInt",     &Pileup_nTrueInt);
    tree->SetBranchAddress("Pileup_nPU",          &Pileup_nPU);
  }
  // weights 
  if(!isData)
    tree->SetBranchAddress("btagWeight_CSVV2",    &btagWeight_CSVV2);
    tree->SetBranchAddress("btagWeight_DeepCSVB",    &btagWeight_DeepCSVB);
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
  tree->SetBranchAddress("Electron_cutBased",   &Electron_cutBased);
  //tree->SetBranchAddress("Electron_cutBased_Spring15",   &Electron_cutBased_Spring15);
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
  tree->SetBranchAddress("Muon_mediumId",       &Muon_mediumId);
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
  tree->SetBranchAddress("Jet_btagDeepC",       &Jet_btagDeepC); 
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
  tree->SetBranchAddress("Flag_eeBadScFilter",                      &Flag_eeBadScFilter);
  // trigger
  if(year==2016)
  {
    tree->SetBranchAddress("HLT_PFHT900",     &HLT_PFHT900);
    tree->SetBranchAddress("HLT_PFJet450",    &HLT_PFJet450);
  }
  if(year>2016)
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

  TFile *babyFile_ = new TFile(outputfile, "RECREATE");
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

  std::vector<bool> trig;
  bool stitch;
  bool pass=true;
  bool fromGS=false;
  bool matched;
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
  std::vector<bool> mus_sigid;
  std::vector<float> mus_miniso;
  //std::vector<float> mus_d0;
  //std::vector<float> mus_dz;
  //std::vector<int> mus_charge;

  int nels;
  //int nvels;
  std::vector<float> els_pt;
  std::vector<float> els_eta;
  //std::vector<float> els_sceta;
  std::vector<float> els_phi;
  std::vector<bool> els_sigid;
  std::vector<bool> els_spr15_sigid;
  std::vector<float> els_miniso;
  std::vector<float> els_reliso;
  //std::vector<bool> els_ispf;
  //std::vector<float> els_d0;
  //std::vector<float> els_dz;
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
  std::vector<float> jets_dcsvc;
  std::vector<bool>  jets_id;
  std::vector<bool>  jets_islep;
  //std::vector<int>   jets_fjet_index;
  std::vector<int>   jets_hflavor;

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
  std::vector<float> sys_bctag;
  std::vector<float> sys_udsgtag;
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

  // global
  babyTree_->Branch("run",               &run);    
  babyTree_->Branch("ls",                &ls);    
  babyTree_->Branch("event",             &event);
  babyTree_->Branch("ht",                &ht);
  babyTree_->Branch("met",               &met);
  babyTree_->Branch("met_phi",           &met_phi);
  babyTree_->Branch("lhe_ht",            &lhe_ht);
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
  babyTree_->Branch("nels",              &nels);    
  babyTree_->Branch("els_pt",            &els_pt);    
  babyTree_->Branch("els_eta",           &els_eta);    
  babyTree_->Branch("els_phi",           &els_phi);    
  babyTree_->Branch("els_sigid",         &els_sigid);    
  babyTree_->Branch("els_spr15_sigid",   &els_spr15_sigid);    
  babyTree_->Branch("els_miniso",        &els_miniso);    
  babyTree_->Branch("els_reliso",        &els_reliso);    
  // jets 
  babyTree_->Branch("njets",             &njets);    
  babyTree_->Branch("nbm",               &nbm);    
  babyTree_->Branch("jets_pt",           &jets_pt);    
  babyTree_->Branch("jets_eta",          &jets_eta);    
  babyTree_->Branch("jets_phi",          &jets_phi);    
  babyTree_->Branch("jets_m",            &jets_m);    
  babyTree_->Branch("jets_csv",          &jets_csv);    
  babyTree_->Branch("jets_dcsvb",        &jets_dcsvb);    
  babyTree_->Branch("jets_dcsvc",        &jets_dcsvc);    
  babyTree_->Branch("jets_id",           &jets_id);    
  babyTree_->Branch("jets_islep",        &jets_islep);    
  babyTree_->Branch("jets_hflavor",      &jets_hflavor);    
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
  babyTree_->Branch("fromGS",           &fromGS);
  //
  babyTree_->Branch("sys_mj12",          &sys_mj12);    
  babyTree_->Branch("sys_lep",           &sys_lep);    
  babyTree_->Branch("sys_ht",            &sys_ht);    
  babyTree_->Branch("sys_nbm",           &sys_nbm);    
  babyTree_->Branch("sys_njets",         &sys_njets);    
  babyTree_->Branch("sys_mur",           &sys_mur);    
  babyTree_->Branch("sys_muf",           &sys_muf);    
  babyTree_->Branch("sys_murf",          &sys_murf);    
  babyTree_->Branch("sys_isr",           &sys_isr);    
  babyTree_->Branch("sys_bctag",         &sys_bctag);    
  babyTree_->Branch("sys_udsgtag",       &sys_udsgtag);    
  // triggers 
  babyTree_->Branch("trig_jet450",       &trig_jet450);    
  babyTree_->Branch("trig_ht900",        &trig_ht900);    
  babyTree_->Branch("trig_ht1050",       &trig_ht1050);    
  babyTree_->Branch("trig_isomu24",      &trig_isomu24);    
  babyTree_->Branch("trig_isomu27",      &trig_isomu27);    
  babyTree_->Branch("pass_hbheiso",      &pass_hbheiso);    


  // 
  // Histgrom : to draw eta-phi plot of energy deposit 
  //  (1) Bin size is 0.087x0.087 to mimic the size of hcal tower
  //  (2) Bin Entry is the sum over energies of PF candidates in a given bin  
  // 
  TH2F *h2 = new TH2F("h2","h2", 115, -5.0, 5.0, 72, -1*TMath::Pi(), TMath::Pi());
  TFile *f_btef = new TFile("btagEfficiency_signal_m1600.root","READ");//FIXME
  // 
  // Loop over entries
  // 
  Int_t nentries = (Int_t)tree->GetEntries();
  if(DEBUG) nentries = 1;
  cout<<"The number of entries in thie file is: "<<nentries<<endl;

  // main event loop
  for(int ievt = 0; ievt<nentries; ievt++) {
  //for(int ievt = 0; ievt<1000; ievt++) {

    // Counting to see progress
    if(ievt%100000==0) 
    { 
      cout << " ...... " << ievt << "/" << nentries << " processed (file number = " << filenumber << "/" << nfiles << ")" << endl; 
    }

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
    nels       =   0;           
    els_pt.clear();         
    els_eta.clear();          
    els_phi.clear();          
    els_sigid.clear();          
    els_spr15_sigid.clear();          
    els_miniso.clear();          
    els_reliso.clear();          
    // jets 
    njets      =   0;        
    nbm        =   0;          
    jets_pt.clear();      
    jets_eta.clear();      
    jets_phi.clear();      
    jets_m.clear();        
    jets_csv.clear();      
    jets_dcsvb.clear();      
    jets_dcsvc.clear();      
    jets_id.clear();      
    jets_islep.clear();     
    jets_hflavor.clear();      
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
    sys_ht.clear();
    sys_lep.clear();
    sys_njets.clear();
    sys_nbm.clear();
    sys_mur.clear();
    sys_muf.clear();
    sys_murf.clear();
    sys_isr.clear();
    sys_bctag.clear();
    sys_udsgtag.clear();
    //
    trig_jet450=true;
    trig_ht900=true;
    trig_ht1050=true;
    trig_isomu24=true;
    trig_isomu27=true;
    pass_hbheiso=true;

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
    ntrupv  = Pileup_nPU;
    ntrupv_mean  = Pileup_nTrueInt;
    lhe_ht = LHE_HTIncoming;

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
      els_sigid.push_back(idElectron_noIso(Electron_vidNestedWPBitmap[iE], 3)); // 3 = medium 
      //els_spr15_sigid.push_back(idElectron_noIso(Electron_vidNestedWPBitmapSpring15[iE], 3));  // 3 = medium 
      els_miniso.push_back(Electron_miniPFRelIso_all[iE]); 
      els_reliso.push_back(Electron_pfRelIso03_all[iE]); 
      if(Electron_pt[iE]<20)  continue;           
      if(abs(Electron_eta[iE])>2.5)  continue;           
      if(!idElectron_noIso(Electron_vidNestedWPBitmap[iE], 3)) continue;           // medium WP
      if(Electron_miniPFRelIso_all[iE]>0.1) continue; // miniso
      els_SFner    =  getLepSF(electronSF, true, Electron_pt[iE], Electron_eta[iE]); // error point in 2017 year FIXME
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
      mus_sigid.push_back(Muon_mediumId[iM]); 
      mus_miniso.push_back(Muon_miniPFRelIso_all[iM]); 
      if(Muon_pt[iM]<20)  continue;           
      if(abs(Muon_eta[iM])>2.4)  continue;           
      if(!Muon_mediumId[iM]) continue;                // medium WP
      if(Muon_miniPFRelIso_all[iM]>0.2) continue;     // miniso 
      
      mus_SFner    =  getLepSF(muonSF, false, Muon_pt[iM], Muon_eta[iM]);
      w_lep        *= mus_SFner.at(0);
      sys_lep_up   *= (mus_SFner.at(0)+mus_SFner.at(1));
      sys_lep_down *= (mus_SFner.at(0)-mus_SFner.at(1));
      mus_SFner.clear();

//      cout<<sys_lep_up<<" "<<sys_lep_down<<" "<<endl;

      nmus++;
      leps_pt.push_back(Muon_pt[iM]); 
      leps_eta.push_back(Muon_eta[iM]); 
      leps_phi.push_back(Muon_phi[iM]); 
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
    sys_jets_pt_up.clear();
    sys_jets_pt_down.clear(); 
    int sys_njets_up = 0;
    int sys_njets_down = 0;
    int sys_nbm_up = 0;
    int sys_nbm_down = 0;
    float sys_ht_up = 0;
    float sys_ht_down = 0;
    float sys_mj12_up = 0;
    float sys_mj12_down = 0;
    float sys_bctag_up = 1;
    float sys_bctag_down = 1;
    float sys_udsgtag_up = 1;
    float sys_udsgtag_down = 1;
    for(int iJ = 0; iJ < nJet; iJ++) 
    {
      jets_pt.push_back(Jet_pt[iJ]); 
      jets_eta.push_back(Jet_eta[iJ]);
//cout<< Jet_eta[iJ] <<" <--eta, pt--> " << Jet_pt[iJ] << endl;
      jets_phi.push_back(Jet_phi[iJ]); 
      jets_m.push_back(Jet_m[iJ]); 
      jets_hflavor.push_back(Jet_hadronFlavour[iJ]); 
      jets_csv.push_back(Jet_btagCSVV2[iJ]); 
      jets_dcsvb.push_back(Jet_btagDeepB[iJ]); 
      jets_dcsvc.push_back(Jet_btagDeepC[iJ]);
      bool jetid = true;
      if(year==2016 && Jet_jetId[iJ]<3 ) jetid=false; // tight Id    
      if(year>=2017 && Jet_jetId[iJ]<2 ) jetid=false; // tight Id 
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
      if(abs(Jet_eta[iJ])>2.4) continue;
      if(!jetid)               continue; 
      if(jetislep)             continue; 
     
      // deepCSV  cuts
      float csv_cut = 0.6321; 
      if(year==2017) csv_cut = 0.4941;
      if(year==2018) csv_cut = 0.4184;
      
      // nominal 
      if(jets_pt.at(iJ)>30)
      {
        njets++;
        ht += Jet_pt[iJ];
        if(Jet_btagDeepB[iJ]>csv_cut) nbm++; 
        if(!isData) 
	  {
	    w_btag_dcsv *= getBtagWeight(f_btef,calibreader, Jet_pt[iJ], Jet_eta[iJ], Jet_hadronFlavour[iJ], Jet_btagDeepB[iJ], csv_cut);
	    sys_bctag_up *= getBtagWeight(f_btef,calibreader, Jet_pt[iJ], Jet_eta[iJ], Jet_hadronFlavour[iJ], Jet_btagDeepB[iJ], csv_cut, "up_hf");
	    sys_bctag_down *= getBtagWeight(f_btef,calibreader, Jet_pt[iJ], Jet_eta[iJ], Jet_hadronFlavour[iJ], Jet_btagDeepB[iJ], csv_cut, "down_hf");
	    sys_udsgtag_up *= getBtagWeight(f_btef,calibreader, Jet_pt[iJ], Jet_eta[iJ], Jet_hadronFlavour[iJ], Jet_btagDeepB[iJ], csv_cut, "up_lf");
	    sys_udsgtag_down *= getBtagWeight(f_btef,calibreader, Jet_pt[iJ], Jet_eta[iJ], Jet_hadronFlavour[iJ], Jet_btagDeepB[iJ], csv_cut, "down_lf");

	  }
      }
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
    sys_mur.push_back(LHEScaleWeight[7]);	sys_mur.push_back(LHEScaleWeight[1]);	
    sys_muf.push_back(LHEScaleWeight[5]);	sys_muf.push_back(LHEScaleWeight[3]);
    sys_murf.push_back(LHEScaleWeight[8]);	sys_murf.push_back(LHEScaleWeight[0]);

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
    vector<fastjet::PseudoJet> input_particles;
    double FatjetConstituent_px_tmp, FatjetConstituent_py_tmp, FatjetConstituent_pz_tmp, FatjetConstituent_energy_tmp;

    for(int iJ = 0; iJ < jets_pt.size(); iJ++) { 

      TLorentzVector JetLV; 
      JetLV.SetPtEtaPhiM(jets_pt.at(iJ), jets_eta.at(iJ), jets_phi.at(iJ), jets_m.at(iJ));

      // jet selection
      if(jets_pt.at(iJ)<30)           continue;
      if(abs(jets_eta.at(iJ))>2.4)    continue;
      if(jets_id.at(iJ)==false)       continue;

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
    vector<fastjet::PseudoJet> inclusive_jets = clust_seq.inclusive_jets(ptmin);
    //Sort by pt
    vector<fastjet::PseudoJet> sorted_jets = sorted_by_pt(inclusive_jets);
    //fill fastjet output into vectors, continue as original code
    for(int isortjets = 0; isortjets< (int)sorted_jets.size(); isortjets++){
      //store only if pt >3 GeV to match CMS jets
      if(TMath::Sqrt( sorted_jets[isortjets].px()*sorted_jets[isortjets].px()
            +sorted_jets[isortjets].py()*sorted_jets[isortjets].py())>(DEBUG?0:3)) {
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
    } 

		//
		// systematics 
		//

    if(!isData){
		// JEC systematics (fill only for MC)
    sys_mj12_up   =  getMJ(sys_jets_pt_up, jets_eta, jets_phi, jets_m, jets_id);
    sys_mj12_down =  getMJ(sys_jets_pt_down, jets_eta, jets_phi, jets_m, jets_id);

		  // fill jec syst branches: vec.at(0) is up and vec.at(1) is down 
    sys_njets.push_back(sys_njets_up);	sys_njets.push_back(sys_njets_down);	
    sys_nbm.push_back(sys_nbm_up);	sys_nbm.push_back(sys_nbm_down);	
    sys_ht.push_back(sys_ht_up);	sys_ht.push_back(sys_ht_down);	
    sys_mj12.push_back(sys_mj12_up);	sys_mj12.push_back(sys_mj12_down);	

		  // btagging
    sys_bctag.push_back(sys_bctag_up);	sys_bctag.push_back(sys_bctag_down);	
    sys_udsgtag.push_back(sys_udsgtag_up);	sys_udsgtag.push_back(sys_udsgtag_down);	
    }

    if(!isData){//number of ISR-->TTbar_Madgraph, signal.
    int nisr_(0);
    TLorentzVector JetLV_, GenLV_; 
    for(size_t ijet(0); ijet<jets_pt.size(); ijet++){
      bool matched_ = false;
      if(jets_pt.at(ijet)<30) continue;
      if(abs(jets_eta.at(ijet))>2.4) continue;
      if(jets_id.at(ijet)==0) continue;
      if(jets_islep.at(ijet)==1) continue;

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

    /*if(year==2016){
      if((inputfile.Contains("TTJets_"))) isr_norm =1.101;
      if(inputfile.Contains("SMS-T1tbs_RPV_mGluino1000")) isr_norm = 1.27982;
      else if(inputfile.Contains("SMS-T1tbs_RPV_mGluino1100")) isr_norm = 1.28991;
      else if(inputfile.Contains("SMS-T1tbs_RPV_mGluino1200")) isr_norm = 1.29881;
      else if(inputfile.Contains("SMS-T1tbs_RPV_mGluino1300")) isr_norm = 1.30728;
      else if(inputfile.Contains("SMS-T1tbs_RPV_mGluino1400")) isr_norm = 1.31325;
      else if(inputfile.Contains("SMS-T1tbs_RPV_mGluino1500")) isr_norm = 1.31898;
      else if(inputfile.Contains("SMS-T1tbs_RPV_mGluino1600")) isr_norm = 1.32481;
      else if(inputfile.Contains("SMS-T1tbs_RPV_mGluino1700")) isr_norm = 1.32986;
      else if(inputfile.Contains("SMS-T1tbs_RPV_mGluino1800")) isr_norm = 1.33543;
      else if(inputfile.Contains("SMS-T1tbs_RPV_mGluino1900")) isr_norm = 1.33801;
      else if(inputfile.Contains("SMS-T1tbs_RPV_mGluino2000")) isr_norm = 1.34401;
      else if(inputfile.Contains("SMS-T1tbs_RPV_mGluino2100")) isr_norm = 1.34697;
      else if(inputfile.Contains("SMS-T1tbs_RPV_mGluino2200")) isr_norm = 1.35132;*/

    if(year==2016){
      if(nisr_==0)       isr_wgt = 1.; 
      else if(nisr_==1)  isr_wgt = 0.920; 
      else if(nisr_==2)  isr_wgt = 0.821; 
      else if(nisr_==3)  isr_wgt = 0.715; 
      else if(nisr_==4)  isr_wgt = 0.662; 
      else if(nisr_==5)  isr_wgt = 0.561; 
      else if(nisr_>=6)  isr_wgt = 0.511; 
    }

    w_isr = isr_wgt*isr_norm;
    if(year==2016){
      sys_isr.push_back(w_isr+((1-w_isr)/2));
      sys_isr.push_back(w_isr-((1-w_isr)/2));
    }
    if(year>=2017){
      sys_isr.push_back(+0);
      sys_isr.push_back(-0);
    }
    nisr = nisr_;
  }

    if(!isData){
      for(size_t ijet(0); ijet<jets_pt.size();ijet++){
	if(jets_pt.at(ijet)<30) continue;
	if(abs(jets_eta.at(ijet))>2.4) continue;
	if(jets_id.at(ijet)==0) continue;
	if(jets_islep.at(ijet)==1) continue;

	for(size_t imc(0); imc<gen_pt.size(); imc++){
	  if((gen_PartIdxMother.at(imc))==-1) continue;
	  int momid = abs(gen_pdgId.at(gen_PartIdxMother.at(imc)));
	  int momstat = gen_statusFlags.at(gen_PartIdxMother.at(imc));
	  int genId = abs(gen_pdgId.at(imc));

	  if(nisr>0 && gen_PartIdxMother.at(imc)==0 && momid==21 && (genId==5 || genId==4)) fromGS = true;//gluon split to b quark
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
      w_pu       = getPUweight(samplename, year, ntrupv_mean, 0); // syst=-1 0 1 (down nominal up)
    }

    if(isData) 
    {
      w_btag_csv  = 1;
      w_btag_dcsv = 1;
      w_lumi      = 1;
      w_pu        = 1;
    }
    if ((inputfile.Contains("SMS-T1tbs_RPV")) || inputfile.Contains("TTJets_Tune")) weight = w_btag_dcsv * w_lumi * w_pu * w_isr;
    else {
      weight = w_btag_dcsv * w_lumi * w_pu;
      w_isr = 1;
    }
    if((inputfile.Contains("TTJets_Tune")) && lhe_ht>600) stitch_ht = false;

    // filters and triggers 
    //https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2
    if(year==2016)
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
    else
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
   trig_isomu24  = HLT_IsoMu24;
   trig_isomu27  = HLT_IsoMu27;
   pass_hbheiso = Flag_HBHENoiseIsoFilter;

   
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
      for(int ijets=0; ijets<(int)fjets_eta.size(); ijets++){
        cone[ijets] = new TEllipse(fjets_eta.at(ijets), fjets_phi.at(ijets), Rparam, Rparam);
        cone[ijets]->SetFillStyle(3003);
        cone[ijets]->SetFillColor(kYellow);
        cone[ijets]->Draw();
      }

      c->SaveAs(Form("EtaPhiViewPFCand_Run%i_Lumi%i_Event%llu_R%.1f.pdf", 
            run, ls, event, Rparam));
      h2->Reset(); 
      for(int ijets=0; ijets<(int)fjets_eta.size(); ijets++) delete cone[ijets];

    } 

  } // event loop


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
  delete f;

  // copy output file to outputdir
  cout << "... transferring output file" << endl;
  cout << Form("... xrdcp %s %s", outputfile.Data(), outputdir.Data()) << endl;  
  gSystem->Exec(Form("xrdcp %s %s", outputfile.Data(), outputdir.Data()));  
  cout << Form("rm %s", outputfile.Data()) << endl;  
  gSystem->Exec(Form("rm %s", outputfile.Data()));  
}


# ifndef __CINT__  // the following code will be invisible for the interpreter
int main(int argc, char **argv)
{

  bool useCondor = true;
  TString inputdir, outputdir, process, list_processed; 
  
  if(argc<5)
  {
    cout << " Please provide proper arguments" << endl;
    cout << "" << endl;
    cout << "   ./process_nano.exe [input dir] [output dir] [process] [list of processed files]" << endl; 
    cout << "" << endl;
    return 0;
  }
  else 
  {
    inputdir    = argv[1];
    outputdir   = argv[2];
    process     = argv[3];
    list_processed    = argv[4];
    
    cout << "-----------------------------------------------------------------------" << endl;
    cout << " input   dir                  : " << inputdir << endl;
    cout << " output  dir                  : " << outputdir << endl;
    cout << " process                      : " << process << endl;
    cout << " list of processed files      : " << list_processed << endl;
    cout << "-----------------------------------------------------------------------" << endl;
  }

  // is this data?
  bool isData = false;
  if(inputdir.Contains("Run201") || process.Contains("Run201"))          isData = true;
  if(inputdir.Contains("JetHT") || process.Contains("JetHT"))            isData = true;
  if(inputdir.Contains("SingleMuon") || process.Contains("SingleMuon"))  isData = true;
 
  // year
  int year    = 0;
  if(inputdir.Contains("RunIISummer16")) year = 2016;
  else if(inputdir.Contains("RunIIFall17")) year = 2017;
  else if(inputdir.Contains("RunIIAutumn18")) year = 2018;
  if(inputdir.Contains("Run2016")) year = 2016;
  else if(inputdir.Contains("Run2017")) year = 2017;
  else if(inputdir.Contains("Run2018")) year = 2018;
  cout << " year: " << year << endl;
  
  // JECs
  if(0)
  {
    JetCorrectorParameters *L1JetPar;
    JetCorrectorParameters *L2JetPar;
    JetCorrectorParameters *L3JetPar;
    JetCorrectorParameters *ResJetPar;

    if(year==2016){
      L1JetPar  = new JetCorrectorParameters("data/jec/Summer16_07Aug2017_V11_MC_L1FastJet_AK4PFchs.txt","");
      L2JetPar  = new JetCorrectorParameters("data/jec/Summer16_07Aug2017_V11_MC_L2Relative_AK4PFchs.txt");
      L3JetPar  = new JetCorrectorParameters("data/jec/Summer16_07Aug2017_V11_MC_L3Absolute_AK4PFchs.txt");
      ResJetPar = new JetCorrectorParameters("data/jec/Summer16_07Aug2017_V11_MC_L2L3Residual_AK4PFchs.txt");
    }
    if(year==2017){
      L1JetPar  = new JetCorrectorParameters("data/jec/Fall17_17Nov2017_V32_MC_L1FastJet_AK4PFchs.txt","");
      L2JetPar  = new JetCorrectorParameters("data/jec/Fall17_17Nov2017_V32_MC_L2Relative_AK4PFchs.txt","");
      L3JetPar  = new JetCorrectorParameters("data/jec/Fall17_17Nov2017_V32_MC_L3Absolute_AK4PFchs.txt","");
      ResJetPar = new JetCorrectorParameters("data/jec/Fall17_17Nov2017_V32_MC_L2L3Residual_AK4PFchs.txt","");
    }
    if(year==2018){
      L1JetPar  = new JetCorrectorParameters("data/jec/Autumn18_V8_MC_L1FastJet_AK4PFchs.txt","");
      L2JetPar  = new JetCorrectorParameters("data/jec/Autumn18_V8_MC_L2Relative_AK4PFchs.txt","");
      L3JetPar  = new JetCorrectorParameters("data/jec/Autumn18_V8_MC_L3Absolute_AK4PFchs.txt","");
      ResJetPar = new JetCorrectorParameters("data/jec/Autumn18_V8_MC_L2L3Residual_AK4PFchs.txt","");
    }
    //  Load the JetCorrectorParameter objects into a vector, IMPORTANT: THE ORDER MATTERS HERE !!!! 
    vector<JetCorrectorParameters> vPar;
    vPar.push_back(*L1JetPar);
    vPar.push_back(*L2JetPar);
    vPar.push_back(*L3JetPar);
    vPar.push_back(*ResJetPar);
    static FactorizedJetCorrector *jetCorrector = 0; 
    JetCorrector = new FactorizedJetCorrector(vPar);
  }
  // JEC systs
  if(year==2016) JecUnc  = new JetCorrectionUncertainty("data/jec/Summer16_07Aug2017_V11_MC_Uncertainty_AK4PFchs.txt");
  if(year==2017) JecUnc  = new JetCorrectionUncertainty("data/jec/Fall17_17Nov2017_V32_MC_Uncertainty_AK4PFchs.txt");
  if(year==2018) JecUnc  = new JetCorrectionUncertainty("data/jec/Autumn18_V8_MC_Uncertainty_AK4PFchs.txt");

  // get list of files in a directory to calculate w_lumi
  // w_lumi = xsec[fb] * genWeight / sum(genWeights)
  // => https://twiki.cern.ch/twiki/bin/view/Main/CMGMonojetAnalysisTools
  //vector<TString> files = globVector(Form("/xrootd/%s/*/*.root", inputdir.Data())); 
  vector<TString> files = getFileListFromFile(Form("flist/%d/flist_%s.txt", year, process.Data()));
  vector<TString> files_original = files; 
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
    TChain ch("Events");
    for(int ifile=0; ifile<files.size(); ifile++)
    {
      ch.Add(files.at(ifile));
    }
    //nfiles = ch.GetListOfFiles()->GetEntries(); 
    TH1D *h = new TH1D("h","h",1,-1,1);
    ch.Draw("0>>h", "genWeight","goff"); 
    sumWeights = h->Integral();
    cout << " sumWeights: " << sumWeights << endl; 
    cout << "-----------------------------------------------------------------------" << endl;
    delete h;
  }
  // 
  vector<TString> files_processed = getFileListFromFile(Form("flist/%s", list_processed.Data())); 
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
       
    if(file_is_processed) 
    { 
      cout << "file already exists in " << outputdir << endl; 
      cout << "skip it!" << endl;
      continue;
    }
    cout << "procesing " << files.at(i) << endl; 
    process_nano(files.at(i), outputdir, sumWeights, process, nfiles, filenumber); 
  }

  return 0;
}
# endif
