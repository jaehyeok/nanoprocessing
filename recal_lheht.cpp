#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>

#include "TROOT.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
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

#include "TObjString.h"
#include "TH3D.h"
#include "TLegend.h"
#include "TLine.h"
#include "TStyle.h"
#include "THStack.h"

// JEC
#include "JetCorrectorParameters.h"
#include "JetCorrectionUncertainty.h"
#include "FactorizedJetCorrector.h"

// btag
#include "BTagCalibrationStandalone.h"

//#include "jetTools.h"
//#include "mcTools.h"
#include "lepTools.h"
#include "inJSON.h"
#include "utilities.h"

using namespace std;

void copy_onefile(TString inputfile)
{
  TH1::SetDefaultSumw2();

  TChain *ch = new TChain("tree");
  ch->Add(inputfile);

  // name output file
  TObjArray* tokens = inputfile.Tokenize("/");
  TString filename = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries()-1)))->GetString();
  filename.ReplaceAll(".root", "_recal_lheht.root");

  // remove branch
  ch->SetBranchStatus("lhe_ht", 0);
  if(inputfile.Contains("TTJets_Tune")) ch->SetBranchStatus("stitch_ht", 0);

  // make output file
  TFile* newfile = new TFile("/data3/nanoprocessing/Running/"+filename, "recreate");

  // copy tree
  TTree *ctree = ch->CopyTree("");
  newfile->cd();
  if(ctree) ctree->Write();
  newfile->Close();

}

void recal_lheht_onefile(TString inputfile, TString year, TString process)
{
  gROOT->ProcessLine("#include <vector>");

  // get output file in Running folder
  TObjArray* tokens = inputfile.Tokenize("/");
  TString filename = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries()-1)))->GetString();
  filename.ReplaceAll(".root", "_recal_lheht.root");
  //
  TFile* file_new = new TFile("/data3/nanoprocessing/Running/"+filename, "update");
  file_new->cd();
  TTree* tree_new = (TTree*)file_new->Get("tree");

  // set branch address
  vector<float>* gen_pt_=0;
  vector<int>* gen_PartIdxMother_=0;
  vector<int>* gen_status_=0;
  vector<int>* gen_pdgId_=0;
  tree_new->SetBranchAddress("gen_pt", &gen_pt_);
  tree_new->SetBranchAddress("gen_PartIdxMother", &gen_PartIdxMother_);
  tree_new->SetBranchAddress("gen_status", &gen_status_);
  tree_new->SetBranchAddress("gen_pdgId", &gen_pdgId_);

  // recalculate and make branch (lhe_ht and stitch_ht)
  float lhe_ht_  = 0.;
  TBranch* b_lhe_ht;
  b_lhe_ht = tree_new->Branch("lhe_ht", &lhe_ht_);
      // TTJets inclusive only
  bool isInclusive = false;
  if(inputfile.Contains("TTJets_Tune")) isInclusive = true;

  // start event loop
  for(Long64_t entry=0; entry<tree_new->GetEntries(); entry++) {
    tree_new->GetEntry(entry);
    if(entry%10000==0) cout << entry << " / " << tree_new->GetEntries() << endl;

    for(int i=0; i<gen_pt_->size(); i++) {
      if(gen_PartIdxMother_->at(i)==0 && gen_status_->at(i)>=20 && gen_status_->at(i)<30 && abs(gen_pdgId_->at(i))!=6) {
	lhe_ht_ = lhe_ht_ + gen_pt_->at(i);
      }
    }
    if(isInclusive) {
      bool stitch_ht_ = true;
      TBranch* b_stitch_ht;
      b_stitch_ht = tree_new->Branch("stitch_ht", &stitch_ht_);
      if(lhe_ht_ > 600) stitch_ht_ = false;
      b_stitch_ht->Fill();
      stitch_ht_ = true;
    }
    b_lhe_ht->Fill();
    lhe_ht_=0.;
  }
  file_new->cd();
  tree_new->Write();
  file_new->Close();

  // cp
  cout << Form("... cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/recalculate_lheht/%s/%s ", filename.Data(), year.Data(), process.Data()) << endl;
  gSystem->Exec(Form("cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/recalculate_lheht/%s/%s ", filename.Data(), year.Data(), process.Data()));
  // rm
  cout << Form("... rm /data3/nanoprocessing/Running/%s", filename.Data()) << endl;
  gSystem->Exec(Form("rm /data3/nanoprocessing/Running/%s", filename.Data()));

}

void recal_lheht_onefile_inclusive(TString inputfile, TString year, TString process)
{
  gROOT->ProcessLine("#include <vector>");

  // get output file in Running folder
  TObjArray* tokens = inputfile.Tokenize("/");
  TString filename = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries()-1)))->GetString();
  filename.ReplaceAll(".root", "_recal_lheht.root");
  //
  TFile* file_new = new TFile("/data3/nanoprocessing/Running/"+filename, "update");
  file_new->cd();
  TTree* tree_new = (TTree*)file_new->Get("tree");

  // set branch address
  vector<float>* gen_pt_=0;
  vector<int>* gen_PartIdxMother_=0;
  vector<int>* gen_status_=0;
  vector<int>* gen_pdgId_=0;
  tree_new->SetBranchAddress("gen_pt", &gen_pt_);
  tree_new->SetBranchAddress("gen_PartIdxMother", &gen_PartIdxMother_);
  tree_new->SetBranchAddress("gen_status", &gen_status_);
  tree_new->SetBranchAddress("gen_pdgId", &gen_pdgId_);

  // recalculate and make branch (lhe_ht and stitch_ht)
  float lhe_ht_  = 0.;
  bool stitch_ht_ = true;
  TBranch* b_lhe_ht;
  TBranch* b_stitch_ht;
  b_lhe_ht = tree_new->Branch("lhe_ht", &lhe_ht_);
  b_stitch_ht = tree_new->Branch("stitch_ht", &stitch_ht_);

  // start event loop
  for(Long64_t entry=0; entry<tree_new->GetEntries(); entry++) {
    tree_new->GetEntry(entry);
    if(entry%10000==0) cout << entry << " / " << tree_new->GetEntries() << endl;

    for(int i=0; i<gen_pt_->size(); i++) {
      if(gen_PartIdxMother_->at(i)==0 && gen_status_->at(i)>=20 && gen_status_->at(i)<30 && abs(gen_pdgId_->at(i))!=6) {
	lhe_ht_ = lhe_ht_ + gen_pt_->at(i);
      }
    }
    if(lhe_ht_ > 600) stitch_ht_ = false;
    b_stitch_ht->Fill();
    b_lhe_ht->Fill();
    stitch_ht_ = true;
    lhe_ht_=0.;
  }
  file_new->cd();
  tree_new->Write();
  file_new->Close();

  // cp
  cout << Form("... cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/recalculate_lheht/%s/%s ", filename.Data(), year.Data(), process.Data()) << endl;
  gSystem->Exec(Form("cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/recalculate_lheht/%s/%s ", filename.Data(), year.Data(), process.Data()));
  // rm
  cout << Form("... rm /data3/nanoprocessing/Running/%s", filename.Data()) << endl;
  gSystem->Exec(Form("rm /data3/nanoprocessing/Running/%s", filename.Data()));

}


int main(int argc, char **argv)
{

  TString year, process;
  year    = argv[1];
  process = argv[2];

  vector<TString> input_files;
  input_files = getFileListFromFile(Form("flist/processed/%s/flist_%s.txt", year.Data(), process.Data()));

  for(unsigned int i=0; i<input_files.size(); i++) {
    if(process.Contains("TTJets_Tune")) {
      cout << "TTJets Inclusive samples" << endl;
      copy_onefile(input_files.at(i));
      recal_lheht_onefile_inclusive(input_files.at(i), year, process);
    } 
    else {
      copy_onefile(input_files.at(i));
      recal_lheht_onefile(input_files.at(i), year, process);
    }
  }


  return 0;

}
