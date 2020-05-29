// ------------------------------------
// Author : Jae Hyeok Yoo
//          jaehyeokyoo@korea.ac.kr
// ------------------------------------
//

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
#include "TH1D.h"


#include "utilities.h"  

// compile
//  $ g++ skim.cpp  `root-config --cflags --glibs` -o skim.exe

using namespace std;

const bool DEBUG = false; 


// store mean here
float w_btag_dcsv_mean;
vector<float> sys_bctag_mean;
vector<float> sys_ucsgtag_mean;
float w_pu_mean;
vector<float> sys_pu_mean;
float w_isr_mean;
vector<float> sys_isr_mean;
float w_lep_mean;
vector<float> sys_lep_mean;
float weight_over_w_lumi_mean; 

//
vector<float> vec_w_btag_dcsv; 
vector<float> vec_w_isr; 


void save_weights(TString inputfile) 
{
	//
  TChain ch("tree");
  ch.Add(inputfile);
  float w_btag_dcsv_ =1;
  float w_isr_ =1;
  ch.SetBranchAddress("w_btag_dcsv",   	 &w_btag_dcsv_);
  ch.SetBranchAddress("w_isr",   	 &w_isr_);
  for(Long64_t entry = 0; entry < ch.GetEntries(); ++entry)
  {
    ch.GetEntry(entry); 
    vec_w_btag_dcsv.push_back(w_btag_dcsv_);
    vec_w_isr.push_back(w_isr_);
  }
}

void copy_onefile(TString inputfile) 
{
  TChain ch("tree");
  ch.Add(inputfile);
	
  TObjArray *tokens = inputfile.Tokenize("/"); 
  TString outputfile = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries()-1)))->GetString();
  outputfile.ReplaceAll(".root", "_norm.root");

  //
  TFile *newfile= new TFile(outputfile,"recreate");
  // remove branch
  ch.SetBranchStatus("w_btag_dcsv", 0);
  ch.SetBranchStatus("w_isr", 0);
  TTree *ctree = ch.CopyTree(""); 
  newfile->cd();
  if(ctree) ctree->Write();
  newfile->Close();
}

void norm_onefile(TString inputfile, TString outputdir) 
{	
  TObjArray *tokens = inputfile.Tokenize("/"); 
  TString outputfile = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries()-1)))->GetString();
  outputfile.ReplaceAll(".root", "_norm.root");
	
  TFile *file_new = new TFile(outputfile,"update");
  file_new->cd();
  TTree *tree_new = (TTree*)file_new->Get("tree");

  float w_btag_dcsv=1.; 
  TBranch *b_w_btag_dcsv = tree_new->Branch("w_btag_dcsv", &w_btag_dcsv);
  float w_isr=1.; 
  TBranch *b_w_isr = tree_new->Branch("w_isr", &w_isr);

  for(Long64_t entry = 0; entry < tree_new->GetEntries(); ++entry)
  {	
    tree_new->GetEntry(entry); 
    w_btag_dcsv=vec_w_btag_dcsv.at(entry)/w_btag_dcsv_mean; 
    w_isr=vec_w_isr.at(entry)/w_isr_mean; 
    
    b_w_btag_dcsv->Fill(); 
    b_w_isr->Fill(); 
  }

  //
  vec_w_btag_dcsv.clear(); 
  vec_w_isr.clear(); 

  //
  file_new->cd();
  tree_new->Write();
  file_new->Close();

  cout << "copying " << outputfile << " to " << outputdir << endl; 
  gSystem->Exec(Form("mv %s %s", outputfile.Data(), outputdir.Data()));
}


# ifndef __CINT__  // the following code will be invisible for the interpreter
int main(int argc, char **argv)
//int main()
{
  TString inputdir, outputdir, tag, prenormdir; 
  
  if(argc<3)
  {
    cout << " Please provide proper arguments" << endl;
    cout << "" << endl;
    cout << "   ./process.exe [input dir] [tag] [output dir] [pre-norm dir] " << endl; 
    cout << "" << endl;
    cout << "   [output dir] [pre-norm dir] are optional" << endl;
    cout << "" << endl;
    return 0;
  }
  else if(argc==3) 
  {
    inputdir     = argv[1];
    tag          = argv[2];
   
    outputdir = inputdir;
    outputdir.ReplaceAll("processed", "norm_weights");
    cout << " input   dir  : " << inputdir << endl;
    cout << " output  dir  : " << outputdir << endl;
    cout << " tag          : " << tag << endl;
  }
  else if(argc==4) 
  {
    inputdir     = argv[1];
    tag          = argv[2];
    outputdir    = argv[3];
   
    cout << " input   dir  : " << inputdir << endl;
    cout << " output  dir  : " << outputdir << endl;
    cout << " tag          : " << tag << endl;
  }
  else if(argc==5) 
  {
    inputdir     = argv[1];
    tag          = argv[2];
    outputdir    = argv[3];
    prenormdir   = argv[4];
   
    cout << " input   dir  : " << inputdir 		<< endl;
    cout << " output  dir  : " << outputdir 	<< endl;
    cout << " tag          : " << tag 				<< endl;
    cout << " prenorm dir  : " << prenormdir 	<< endl;
  }

  // make output directory
  gSystem->mkdir(outputdir.Data());

	//
	// Get mean of weights
	//
	
	// need to get the mean of each weight including syst branches
	// w_btag_dcsv, sys_bctag, sys_ucsgtag
	// w_pu, sys_pu
	// w_isr, sys_isr
	// w_lep, sys_lep
	// weight/w_lumi 

  vector<TString> prenorm_files = globVector(Form("%s/*%s*.root", prenormdir.Data(), tag.Data())); 
  TChain ch_mean("tree");	
  for(int i=0; i<prenorm_files.size(); i++) ch_mean.Add(prenorm_files.at(i));
	cout << "weights calculated using " << prenorm_files.size() << " files" << endl;
	
  TH1D  *h_w_btag_dcsv = new TH1D("h_w_btag_dcsv","h_w_btag_dcsv",100,-5,5);
  ch_mean.Draw("w_btag_dcsv>>h_w_btag_dcsv","","geoff");
  w_btag_dcsv_mean = h_w_btag_dcsv->GetMean();
  cout << "w_btag_dcsv mean = " << w_btag_dcsv_mean << endl;
	
  TH1D  *h_w_isr = new TH1D("h_w_isr","h_w_isr",100,-5,5);
  ch_mean.Draw("w_isr>>h_w_isr","","geoff");
  w_isr_mean = h_w_isr->GetMean();
  cout << "w_isr mean = " << w_isr_mean << endl;
 
	// 
	// Process 
	// 

	// get list of files in a directory
  vector<TString> files = globVector(Form("%s/*%s*.root", inputdir.Data(), tag.Data())); 
	cout << "processing " << files.size() << " files" << endl;

  // process files one by one 
	for(int i=0; i<files.size(); i++)
  {
    cout << "processing: " << files.at(i) << endl; 
    save_weights(files.at(i)); 
    copy_onefile(files.at(i)); 
    norm_onefile(files.at(i), outputdir); 
  }
  return 0;
}
# endif
