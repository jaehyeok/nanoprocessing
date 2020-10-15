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
vector<float> vec_w_lumi; 
vector<float> vec_weight; 


void save_weights(TString inputfile) 
{
	//
  TChain ch("tree");
  ch.Add(inputfile);
  float w_btag_dcsv_ =1;
  float w_isr_ =1;
  float w_lumi_ =1;
  float weight_ =1;
  ch.SetBranchAddress("w_btag_dcsv",   	 	&w_btag_dcsv_);
  ch.SetBranchAddress("w_isr",   	 				&w_isr_);
  ch.SetBranchAddress("w_lumi",   	 			&w_lumi_);
  ch.SetBranchAddress("weight",   	 		  &weight_);
  for(Long64_t entry = 0; entry < ch.GetEntries(); ++entry)
  {
    ch.GetEntry(entry); 
    vec_w_btag_dcsv.push_back(w_btag_dcsv_);
    vec_w_isr.push_back(w_isr_);
    vec_w_lumi.push_back(w_lumi_);
    vec_weight.push_back(weight_);
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
  ch.SetBranchStatus("w_isr", 			0);
  ch.SetBranchStatus("weight", 			0);
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
  float weight=1.; 
  TBranch *b_weight = tree_new->Branch("weight", &weight);

  for(Long64_t entry = 0; entry < tree_new->GetEntries(); ++entry)
  {	
    tree_new->GetEntry(entry); 
    w_btag_dcsv=vec_w_btag_dcsv.at(entry)/w_btag_dcsv_mean; 
    w_isr=vec_w_isr.at(entry)/w_isr_mean; 
    weight=vec_weight.at(entry)/weight_over_w_lumi_mean; 
    
    b_w_btag_dcsv->Fill(); 
    b_w_isr->Fill(); 
    b_weight->Fill(); 
  }

  //
  vec_w_btag_dcsv.clear(); 
  vec_w_isr.clear(); 
  vec_w_lumi.clear(); 
  vec_weight.clear(); 

  //
  file_new->cd();
  tree_new->Write();
  file_new->Close();

	// copy output file to outputdir
  cout << "... transferring output file" << endl;
  cout << Form("... xrdcp %s %s", outputfile.Data(), outputdir.Data()) << endl;  
  gSystem->Exec(Form("xrdcp %s %s", outputfile.Data(), outputdir.Data()));  
  cout << Form("rm %s", outputfile.Data()) << endl;  
  gSystem->Exec(Form("rm %s", outputfile.Data()));  
}


# ifndef __CINT__  // the following code will be invisible for the interpreter
int main(int argc, char **argv)
//int main()
{
  bool useCondor = true;
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
    prenormdir   = inputdir;
  }
  else if(argc==4) 
  {
    inputdir     = argv[1];
    tag          = argv[2];
    outputdir    = argv[3];
    prenormdir   = inputdir;
  }
  else if(argc==5) 
  {
    inputdir     = argv[1];
    tag          = argv[2];
    outputdir    = argv[3];
    prenormdir   = argv[4];
  }
	
	int year    = 0;
  if(inputdir.Contains("/2016/")) year = 2016;
  else if(inputdir.Contains("/2017/")) year = 2017;
  else if(inputdir.Contains("/2018/")) year = 2018;
  
  cout << " input   dir  : " << inputdir 		<< endl;
  cout << " output  dir  : " << outputdir 	<< endl;
  cout << " tag          : " << tag 				<< endl;
  cout << " prenorm dir  : " << prenormdir 	<< endl;
  cout << " year         : " << year 	<< endl;
  

  // make output directory
  gSystem->mkdir(outputdir.Data());
      
  if(useCondor)
  {
    outputdir.ReplaceAll("/xrootd_user","root://cms-xrdr.private.lo:2094//xrd/store/user");
    outputdir.ReplaceAll("/xrootd","");
  }
  //
  // Automatic Detection whether the Tag is inclusive or not inclusive.
  //
  vector<TString> tags,tag_all;
  TString tag_fixed;
  if(tag.Contains("HT")){
    cout<<"It's NOT inclusive!"<<endl;

    string file_path = __FILE__;
    string dir_path = file_path.substr(0,file_path.rfind("\\"));
    cout<<file_path<<endl;
    tag_all = getFileListFromFile(Form("nanoprocessing/condor/samples/samples%d_v7.txt",year)); //FIXME : You should change this directory to your nanoprocessing directory.

    for(auto tag_check:tag_all){
      if(!tag_check.Contains("HT")) continue;
      if(!tag_check.Contains(tag)) continue;
      tag_fixed = tag_check.Remove(tag_check.Index("_13TeV"));
      tag_fixed = tag_fixed.Remove(0,tag_fixed.Index("HT")+2);
      tags.push_back(tag.Remove(tag.Index("HT")+2)+tag_fixed);
      cout<<tag<<endl;
    }
  }
  else tags.push_back(tag);
  vector<TString> prenorm_files;
 	//
	// Get mean of weights
	//
	
	// need to get the mean of each weight including syst branches
	// w_btag_dcsv, sys_bctag, sys_ucsgtag
	// w_pu, sys_pu
	// w_isr, sys_isr
	// w_lep, sys_lep
	// weight/w_lumi 

  //vector<TString> prenorm_files = globVector(Form("%s/*%s*.root", prenormdir.Data(), tag.Data())); 
  for(auto tag_ : tags){
    vector<TString> prenorm_files_ = getFileListFromFile(Form("flist/norm/%d/flist_prenorm_%s.txt", year, tag_.Data()));
    for(auto fname : prenorm_files_){
      prenorm_files.push_back(fname);  
      cout<<fname<<endl;
    }
    cout<<tag_<<endl;
  }
  TChain ch_mean("tree");	
  for(int i=0; i<prenorm_files.size(); i++)
  {
    if(useCondor) 
    { 
      prenorm_files.at(i).ReplaceAll("/xrootd_user","root://cms-xrdr.private.lo:2094//xrd/store/user");
      prenorm_files.at(i).ReplaceAll("/xrootd","");
    }
    cout << prenorm_files.at(i) << endl;
		ch_mean.Add(prenorm_files.at(i));
  }

	cout << "weights calculated using " << prenorm_files.size() << " files" << endl;
	cout << "number of events in the babies: " << ch_mean.GetEntries() << endl;
	if(ch_mean.GetEntries()==0) return 0;

  TH1D  *h_w_btag_dcsv = new TH1D("h_w_btag_dcsv","h_w_btag_dcsv",100,-5,5);
  ch_mean.Draw("w_btag_dcsv>>h_w_btag_dcsv","","geoff");
  w_btag_dcsv_mean = h_w_btag_dcsv->GetMean();
  cout << "w_btag_dcsv mean = " << w_btag_dcsv_mean << endl;
	
  TH1D  *h_w_isr = new TH1D("h_w_isr","h_w_isr",100,-5,5);
  ch_mean.Draw("w_isr>>h_w_isr","","geoff");
  w_isr_mean = h_w_isr->GetMean();
  cout << "w_isr mean = " << w_isr_mean << endl;
  
	TH1D  *h_weight_over_w_lumi = new TH1D("h_weight_over_w_lumi","h_weight_over_w_lumi",100,-5,5);
  ch_mean.Draw("weight/w_lumi>>h_weight_over_w_lumi","","geoff");
  weight_over_w_lumi_mean = h_weight_over_w_lumi->GetMean();
  cout << "weight/w_lumi mean = " << weight_over_w_lumi_mean << endl;
 
	// 
	// Process 
	// 

	// get list of files in a directory
  //vector<TString> files = globVector(Form("%s/*%s*.root", inputdir.Data(), tag.Data())); 
  vector<TString> tonorm_files;
  for(auto tag_ : tags){
    vector<TString> tonorm_files_ = getFileListFromFile(Form("flist/norm/%d/flist_tonorm_%s.txt", year, tag_.Data()));
    for(auto fname : tonorm_files_){
      tonorm_files.push_back(fname);  
      cout<<fname<<endl;
    }
  }
  //vector<TString> tonorm_files = getFileListFromFile(Form("flist/norm/%d/flist_tonorm_%s.txt", year, tag.Data()));
	cout << "processing " << tonorm_files.size() << " files" << endl;

  // process files one by one 
	for(int i=0; i<tonorm_files.size(); i++)
  {
    if(useCondor)
    {
      tonorm_files.at(i).ReplaceAll("/xrootd_user","root://cms-xrdr.private.lo:2094//xrd/store/user");
      tonorm_files.at(i).ReplaceAll("/xrootd","");
    }
    cout << "processing: " << tonorm_files.at(i) << endl; 
    save_weights(tonorm_files.at(i)); 
    copy_onefile(tonorm_files.at(i)); 
    norm_onefile(tonorm_files.at(i), outputdir); 
  }
  return 0;
}
# endif
