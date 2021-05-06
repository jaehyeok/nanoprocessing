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
float sys_bctag_mean;
float sys_udsgtag_mean;
float sys_mur_mean;
float sys_muf_mean;
float sys_murf_mean;

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

vector<float> vec_sys_mur;
vector<float> vec_sys_muf;
vector<float> vec_sys_murf;
vector<float> vec_sys_bctag;
vector<float> vec_sys_udsgtag;

void save_weights(TString inputfile) 
{
	//
  TChain ch("tree");
  ch.Add(inputfile);
  float w_btag_dcsv_ =1;
  float w_isr_ =1;
  float w_lumi_ =1;
  float l1pre_nom_ =1;
  float weight_ =1;
  vector<float> *sys_mur_=new vector<float>;
  vector<float> *sys_muf_=new vector<float>;
  vector<float> *sys_murf_=new vector<float>;
  vector<float> *sys_bctag_=new vector<float>;
  vector<float> *sys_udsgtag_=new vector<float>;// */

  ch.SetBranchAddress("w_btag_dcsv",   	 	&w_btag_dcsv_);
  ch.SetBranchAddress("w_isr",   		&w_isr_);
  ch.SetBranchAddress("w_lumi",   		&w_lumi_);
  ch.SetBranchAddress("l1pre_nom",   		&l1pre_nom_);
  ch.SetBranchAddress("weight",   		&weight_);

  ch.SetBranchAddress("sys_mur",   		&sys_mur_);
  ch.SetBranchAddress("sys_muf",   		&sys_muf_);
  ch.SetBranchAddress("sys_murf",   		&sys_murf_);
  ch.SetBranchAddress("sys_bctag",   		&sys_bctag_);
  ch.SetBranchAddress("sys_udsgtag",   		&sys_udsgtag_);// */

  for(Long64_t entry = 0; entry < ch.GetEntries(); ++entry)
  {
    ch.GetEntry(entry); 
    vec_w_btag_dcsv.push_back(w_btag_dcsv_);
    vec_w_isr.push_back(w_isr_);
    vec_w_lumi.push_back(w_lumi_);
    vec_weight.push_back(weight_);

    vec_sys_mur.insert(vec_sys_mur.end(), sys_mur_->begin(), sys_mur_->end());
    vec_sys_muf.insert(vec_sys_muf.end(), sys_muf_->begin(), sys_muf_->end());
    vec_sys_murf.insert(vec_sys_murf.end(), sys_murf_->begin(), sys_murf_->end());
    vec_sys_bctag.insert(vec_sys_bctag.end(), sys_bctag_->begin(), sys_bctag_->end());
    vec_sys_udsgtag.insert(vec_sys_udsgtag.end(), sys_udsgtag_->begin(), sys_udsgtag_->end());// */
  }
}

void copy_onefile(TString inputfile) 
{
  TChain ch("tree");
  ch.Add(inputfile);

  bool mGluino = false;
  if(inputfile.Contains("SMS-T1tbs")) mGluino = true;

  TObjArray *tokens = inputfile.Tokenize("/"); 
  TString outputfile = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries()-1)))->GetString();
  outputfile.ReplaceAll(".root", "_norm.root");

  //
  TFile *newfile= new TFile(outputfile,"recreate");
  // remove branch
  ch.SetBranchStatus("w_btag_dcsv", 0);
  ch.SetBranchStatus("w_isr", 	    0);
  ch.SetBranchStatus("weight", 	    0);

  if(mGluino){
    ch.SetBranchStatus("sys_mur",	0);
    ch.SetBranchStatus("sys_muf",	0);
    ch.SetBranchStatus("sys_murf",0);
    ch.SetBranchStatus("sys_bctag",0);
    ch.SetBranchStatus("sys_udsgtag",0);
  }

  TTree *ctree = ch.CopyTree(""); 
  newfile->cd();
  if(ctree) ctree->Write();
  newfile->Close();
}

void norm_onefile(TString inputfile, TString outputdir, TString inputdir)
{	
  TObjArray *tokens = inputfile.Tokenize("/"); 
  TString outputfile = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries()-1)))->GetString();
  outputfile.ReplaceAll(".root", "_norm.root");
	
  TFile *file_new = new TFile(outputfile,"update");
  file_new->cd();
  TTree *tree_new = (TTree*)file_new->Get("tree");

  bool mGluino = false;
  if(inputfile.Contains("SMS-T1tbs")) mGluino = true;

  float w_btag_dcsv=1., w_isr=1., weight=1., frac1718=1.;
  vector<float> sys_mur;
  vector<float> sys_muf;
  vector<float> sys_murf;
  vector<float> sys_bctag;
  vector<float> sys_udsgtag;

  TBranch *b_frac1718, *b_w_btag_dcsv, *b_w_isr, *b_weight;
  TBranch *b_sys_mur, *b_sys_muf, *b_sys_murf, *b_sys_bctag, *b_sys_udsgtag;

  b_frac1718 = tree_new->Branch("frac1718",&frac1718);
  b_w_btag_dcsv = tree_new->Branch("w_btag_dcsv", &w_btag_dcsv);
  b_w_isr = tree_new->Branch("w_isr", &w_isr);
  b_weight = tree_new->Branch("weight", &weight);

  if(mGluino){
    b_sys_mur = tree_new->Branch("sys_mur",&sys_mur);
    b_sys_muf = tree_new->Branch("sys_muf",&sys_muf);
    b_sys_murf = tree_new->Branch("sys_murf",&sys_murf);
    b_sys_bctag = tree_new->Branch("sys_bctag",&sys_bctag);
    b_sys_udsgtag = tree_new->Branch("sys_udsgtag",&sys_udsgtag);
  }

  int year    = 0;
  if(inputdir.Contains("/2016/")) year = 2016;
  else if(inputdir.Contains("/2017/")) year = 2017;
  else if(inputdir.Contains("/2018/")) year = 2018;

  for(Long64_t entry = 0; entry < tree_new->GetEntries(); ++entry)
  {	
    tree_new->GetEntry(entry); 
    w_btag_dcsv=vec_w_btag_dcsv.at(entry)/w_btag_dcsv_mean; 
    w_isr=vec_w_isr.at(entry)/w_isr_mean; 
    weight=vec_weight.at(entry)/weight_over_w_lumi_mean;

    if(mGluino){
      sys_mur.push_back(vec_sys_mur.at(2*entry)/sys_mur_mean);
      sys_mur.push_back(vec_sys_mur.at(2*entry+1)/sys_mur_mean);
      sys_muf.push_back(vec_sys_muf.at(2*entry)/sys_muf_mean);
      sys_muf.push_back(vec_sys_muf.at(2*entry+1)/sys_muf_mean);
      sys_murf.push_back(vec_sys_murf.at(2*entry)/sys_murf_mean);
      sys_murf.push_back(vec_sys_murf.at(2*entry+1)/sys_murf_mean);
      sys_bctag.push_back(vec_sys_bctag.at(2*entry)/sys_bctag_mean);
      sys_bctag.push_back(vec_sys_bctag.at(2*entry+1)/sys_bctag_mean);
      sys_udsgtag.push_back(vec_sys_udsgtag.at(2*entry)/sys_udsgtag_mean);
      sys_udsgtag.push_back(vec_sys_udsgtag.at(2*entry+1)/sys_udsgtag_mean);
    }

    if(year == 2016) frac1718 = 1;
    else if(year == 2017) frac1718 = 41.5/(41.5+59.7);
    else if(year == 2018) frac1718 = 59.7/(41.5+59.7);

    b_frac1718->Fill();

    b_w_btag_dcsv->Fill(); 
    b_w_isr->Fill(); 
    b_weight->Fill(); 

    if(mGluino){
      b_sys_mur->Fill();
      b_sys_muf->Fill();
      b_sys_murf->Fill();
      b_sys_bctag->Fill();
      b_sys_udsgtag->Fill();
    }

    sys_mur.clear();
    sys_muf.clear();
    sys_murf.clear();
    sys_bctag.clear();
    sys_udsgtag.clear();
  }

  //
  vec_w_btag_dcsv.clear(); 
  vec_w_isr.clear(); 
  vec_w_lumi.clear(); 
  vec_weight.clear(); 

  vec_sys_mur.clear();
  vec_sys_muf.clear();
  vec_sys_murf.clear();
  vec_sys_bctag.clear();
  vec_sys_udsgtag.clear();

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
  ROOT::EnableImplicitMT(8);
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
    tag_all = getFileListFromFile(Form("/cms/scratch/yjeong/CMSSW_8_0_0/src/nanoprocessing/condor/samples/samples%d_v7.txt",year)); //FIXME : You should change this directory to your nanoprocessing directory.

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
  ch_mean.Draw("weight/(w_lumi*l1pre_nom)>>h_weight_over_w_lumi","","geoff");
  weight_over_w_lumi_mean = h_weight_over_w_lumi->GetMean();
  cout << "weight/(w_lumi mean*l1pre_nom) = " << weight_over_w_lumi_mean << endl;


  TH1D  *h_sys_mur = new TH1D("h_sys_mur","h_sys_mur",100,-5,5);
  ch_mean.Draw("sys_mur>>h_sys_mur","","geoff");
  sys_mur_mean = h_sys_mur->GetMean();
  cout << "sys_mur mean = " << sys_mur_mean << endl;

  TH1D  *h_sys_muf = new TH1D("h_sys_muf","h_sys_muf",100,-5,5);
  ch_mean.Draw("sys_muf>>h_sys_muf","","geoff");
  sys_muf_mean = h_sys_muf->GetMean();
  cout << "sys_muf mean = " << sys_muf_mean << endl;

  TH1D  *h_sys_murf = new TH1D("h_sys_murf","h_sys_murf",100,-5,5);
  ch_mean.Draw("sys_murf>>h_sys_murf","","geoff");
  sys_murf_mean = h_sys_murf->GetMean();
  cout << "sys_murf mean = " << sys_murf_mean << endl;

  TH1D  *h_sys_bctag = new TH1D("h_sys_bctag","h_sys_bctag",100,-5,5);
  ch_mean.Draw("sys_bctag>>h_sys_bctag","","geoff");
  sys_bctag_mean = h_sys_bctag->GetMean();
  cout << "sys_bctag mean = " << sys_bctag_mean << endl;

  TH1D  *h_sys_udsgtag = new TH1D("h_sys_udsgtag","h_sys_udsgtag",100,-5,5);
  ch_mean.Draw("sys_udsgtag>>h_sys_udsgtag","","geoff");
  sys_udsgtag_mean = h_sys_udsgtag->GetMean();
  cout << "sys_udsgtag mean = " << sys_udsgtag_mean << endl;

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
    norm_onefile(tonorm_files.at(i), outputdir, inputdir); 
  }
  return 0;
}
# endif
