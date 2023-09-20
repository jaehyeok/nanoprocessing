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
#include "TObjString.h"

#include "utilities.h"  

// compile
//  $ g++ skim.cpp  `root-config --cflags --glibs` -o skim.exe

using namespace std;

const bool DEBUG = false; 

// store mean here
float w_btag_dcsv_mean;
float sys_bctag_mean_0, sys_bctag_mean_1;
float sys_udsgtag_mean_0, sys_udsgtag_mean_1;
float sys_mur_mean_0, sys_mur_mean_1;
float sys_muf_mean_0, sys_muf_mean_1;
float sys_murf_mean_0, sys_murf_mean_1;
float sys_isr_mean_0, sys_isr_mean_1;
float sys_pu_mean_0, sys_pu_mean_1;
float sys_lep_mean_0, sys_lep_mean_1;

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
vector<float> vec_l1pre_nom; 
vector<float> vec_w_lep;
vector<float> vec_w_pu;

vector<float> vec_sys_isr;
vector<float> vec_sys_mur;
vector<float> vec_sys_muf;
vector<float> vec_sys_murf;
vector<float> vec_sys_bctag;
vector<float> vec_sys_udsgtag;
vector<float> vec_sys_pu;
vector<float> vec_sys_lep;

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
  float w_lep_ =1;
  float w_pu_ =1;
  vector<float> *sys_isr_=new vector<float>;
  vector<float> *sys_mur_=new vector<float>;
  vector<float> *sys_muf_=new vector<float>;
  vector<float> *sys_murf_=new vector<float>;
  vector<float> *sys_bctag_=new vector<float>;
  vector<float> *sys_udsgtag_=new vector<float>;
  vector<float> *sys_pu_=new vector<float>;
  vector<float> *sys_lep_=new vector<float>;

  ch.SetBranchAddress("w_btag_dcsv",   	 	&w_btag_dcsv_);
  ch.SetBranchAddress("w_isr",   		&w_isr_);
  ch.SetBranchAddress("w_lep",   		&w_lep_);
  ch.SetBranchAddress("w_pu",   		&w_pu_);
  ch.SetBranchAddress("w_lumi",   		&w_lumi_);
  ch.SetBranchAddress("l1pre_nom",   		&l1pre_nom_);
  ch.SetBranchAddress("weight",   		&weight_);

  ch.SetBranchAddress("sys_isr",   		&sys_isr_);
  ch.SetBranchAddress("sys_mur",   		&sys_mur_);
  ch.SetBranchAddress("sys_muf",   		&sys_muf_);
  ch.SetBranchAddress("sys_murf",   		&sys_murf_);
  ch.SetBranchAddress("sys_bctag",   		&sys_bctag_);
  ch.SetBranchAddress("sys_udsgtag",   		&sys_udsgtag_);
  ch.SetBranchAddress("sys_pu",   		&sys_pu_);
  ch.SetBranchAddress("sys_lep",   		&sys_lep_);

  for(Long64_t entry = 0; entry < ch.GetEntries(); ++entry)
  {
    ch.GetEntry(entry); 
    vec_w_btag_dcsv.push_back(w_btag_dcsv_);
    vec_w_isr.push_back(w_isr_);
    vec_w_lumi.push_back(w_lumi_);
    vec_l1pre_nom.push_back(l1pre_nom_);
    vec_w_lep.push_back(w_lep_);
    vec_w_pu.push_back(w_pu_);

    vec_sys_isr.insert(vec_sys_isr.end(), sys_isr_->begin(), sys_isr_->end());
    vec_sys_mur.insert(vec_sys_mur.end(), sys_mur_->begin(), sys_mur_->end());
    vec_sys_muf.insert(vec_sys_muf.end(), sys_muf_->begin(), sys_muf_->end());
    vec_sys_murf.insert(vec_sys_murf.end(), sys_murf_->begin(), sys_murf_->end());
    vec_sys_bctag.insert(vec_sys_bctag.end(), sys_bctag_->begin(), sys_bctag_->end());
    vec_sys_udsgtag.insert(vec_sys_udsgtag.end(), sys_udsgtag_->begin(), sys_udsgtag_->end());
    vec_sys_pu.insert(vec_sys_pu.end(), sys_pu_->begin(), sys_pu_->end());
    vec_sys_lep.insert(vec_sys_lep.end(), sys_lep_->begin(), sys_lep_->end());
  }
}

void copy_onefile(TString inputfile) 
{
  TChain ch("tree");
  ch.Add(inputfile);

  TObjArray *tokens = inputfile.Tokenize("/"); 
  TString filename = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries()-1)))->GetString();
  filename.ReplaceAll(".root", "_norm.root");

  TFile *newfile= new TFile("Running/"+filename, "recreate");
  // remove branches
  ch.SetBranchStatus("w_btag_dcsv", 0);
  ch.SetBranchStatus("w_isr", 	    0);
  ch.SetBranchStatus("weight", 	    0);
  ch.SetBranchStatus("w_lep", 	    0);
  ch.SetBranchStatus("w_pu", 	    0);

  ch.SetBranchStatus("sys_isr",		0);
  ch.SetBranchStatus("sys_bctag",	0);
  ch.SetBranchStatus("sys_udsgtag",	0);
  ch.SetBranchStatus("sys_pu",		0);
  ch.SetBranchStatus("sys_mur",		0);
  ch.SetBranchStatus("sys_muf",		0);
  ch.SetBranchStatus("sys_murf",	0);
  ch.SetBranchStatus("sys_lep",		0);

  TTree *ctree = ch.CopyTree(""); 
  newfile->cd();
  if(ctree) ctree->Write();
  newfile->Close();
}

void norm_onefile(TString inputfile, TString year, TString process, TString skim)
{

  TObjArray *tokens = inputfile.Tokenize("/"); 
  TString filename = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries()-1)))->GetString();
  filename.ReplaceAll(".root", "_norm.root");

  TFile *file_new = new TFile("Running/"+filename, "update");
  file_new->cd();
  TTree *tree_new = (TTree*)file_new->Get("tree");


  vector<float> sys_mur;
  vector<float> sys_muf;
  vector<float> sys_murf;
  vector<float> sys_bctag;
  vector<float> sys_udsgtag;
  vector<float> sys_isr;
  vector<float> sys_pu;
  vector<float> sys_lep;

  float w_btag_dcsv=1., w_isr=1., weight=1., w_lep=1., frac1718=1., w_pu=1., frac16=1.;

  TBranch *b_frac1718, *b_frac16, *b_w_btag_dcsv, *b_w_isr, *b_weight, *b_w_lep, *b_w_pu;
  TBranch *b_sys_isr, *b_sys_mur, *b_sys_muf, *b_sys_murf, *b_sys_bctag, *b_sys_udsgtag, *b_sys_pu, *b_sys_lep;

  b_frac1718 	= tree_new->Branch("frac1718",&frac1718);
  b_frac16 	= tree_new->Branch("frac16",&frac16);
  b_w_btag_dcsv = tree_new->Branch("w_btag_dcsv", &w_btag_dcsv);
  b_w_isr 	= tree_new->Branch("w_isr", &w_isr);
  b_weight 	= tree_new->Branch("weight", &weight);
  b_w_lep 	= tree_new->Branch("w_lep", &w_lep);
  b_w_pu 	= tree_new->Branch("w_pu", &w_pu);

  b_sys_isr 	= tree_new->Branch("sys_isr",&sys_isr);
  b_sys_bctag 	= tree_new->Branch("sys_bctag",&sys_bctag);
  b_sys_udsgtag = tree_new->Branch("sys_udsgtag",&sys_udsgtag);
  b_sys_pu 	= tree_new->Branch("sys_pu",&sys_pu);
  b_sys_mur 	= tree_new->Branch("sys_mur",&sys_mur);
  b_sys_muf 	= tree_new->Branch("sys_muf",&sys_muf);
  b_sys_murf 	= tree_new->Branch("sys_murf",&sys_murf);
  b_sys_lep 	= tree_new->Branch("sys_lep",&sys_lep);


  for(Long64_t entry = 0; entry < tree_new->GetEntries(); ++entry)
  {	
    tree_new->GetEntry(entry); 
    w_btag_dcsv=vec_w_btag_dcsv.at(entry)/w_btag_dcsv_mean; 
    w_isr=vec_w_isr.at(entry)/w_isr_mean; 
    w_lep=vec_w_lep.at(entry)/w_lep_mean;
    w_pu=vec_w_pu.at(entry)/w_pu_mean;

    sys_mur.push_back(vec_sys_mur.at(2*entry)/sys_mur_mean_0);     //down
    sys_mur.push_back(vec_sys_mur.at(2*entry+1)/sys_mur_mean_1);   //up
    sys_muf.push_back(vec_sys_muf.at(2*entry)/sys_muf_mean_0);
    sys_muf.push_back(vec_sys_muf.at(2*entry+1)/sys_muf_mean_1);
    sys_murf.push_back(vec_sys_murf.at(2*entry)/sys_murf_mean_0);
    sys_murf.push_back(vec_sys_murf.at(2*entry+1)/sys_murf_mean_1);

    sys_bctag.push_back(vec_sys_bctag.at(2*entry)/sys_bctag_mean_0);
    sys_bctag.push_back(vec_sys_bctag.at(2*entry+1)/sys_bctag_mean_1);
    sys_udsgtag.push_back(vec_sys_udsgtag.at(2*entry)/sys_udsgtag_mean_0);
    sys_udsgtag.push_back(vec_sys_udsgtag.at(2*entry+1)/sys_udsgtag_mean_1);
    sys_pu.push_back(vec_sys_pu.at(2*entry)/sys_pu_mean_0);
    sys_pu.push_back(vec_sys_pu.at(2*entry+1)/sys_pu_mean_1);
    sys_lep.push_back(vec_sys_lep.at(2*entry)/sys_lep_mean_0);
    sys_lep.push_back(vec_sys_lep.at(2*entry+1)/sys_lep_mean_1);

    if(year == "2016"){
      sys_isr.push_back(vec_sys_isr.at(2*entry)/sys_isr_mean_0);
      sys_isr.push_back(vec_sys_isr.at(2*entry+1)/sys_isr_mean_1);
    }
    else{
      sys_isr.push_back(1);
      sys_isr.push_back(1);
    }
   
    // redefine weight
    weight = w_btag_dcsv * w_pu * w_lep * w_isr * vec_w_lumi.at(entry) * vec_l1pre_nom.at(entry);
    if(w_btag_dcsv==0 || w_pu==0 || w_lep==0 || w_isr==0 || 
       vec_w_lumi.at(entry)==0 || vec_l1pre_nom.at(entry)==0 ||
       isnan(w_btag_dcsv)==true || isnan(w_pu)==true || isnan(w_lep)==true || isnan(w_isr)==true) {
      cout << "One of the components of weight is nan or zero !!" << endl;
      weight=0;
    }

    if(year == "2016") {
      frac16   = 1;
      frac1718 = 1;
    }
    else if(year == "2017") {
      frac16   = 1;
      frac1718 = 41.5/(41.5+59.7);
    }
    else if(year == "2018") {
      frac16   = 1;
      frac1718 = 59.7/(41.5+59.7);
    }

    else if(year == "UL2016_preVFP") {
      frac16   = 19.5/(19.5+16.8);
      frac1718 = 1;
    }
    else if(year == "UL2016") {
      frac16   = 16.8/(19.5+16.8);
      frac1718 = 1;
    }
    else if(year == "UL2017") {
      frac16   = 1;
      frac1718 = 41.5/(41.5+59.8);
    }
    else if(year == "UL2018") {
      frac16   = 1;
      frac1718 = 59.8/(41.5+59.8);
    }

    b_frac16->Fill();
    b_frac1718->Fill();

    b_w_btag_dcsv->Fill(); 
    b_w_isr->Fill(); 
    b_weight->Fill(); 
    b_w_lep->Fill(); 
    b_w_pu->Fill(); 

    b_sys_mur->Fill();
    b_sys_muf->Fill();
    b_sys_murf->Fill();

    b_sys_bctag->Fill();
    b_sys_udsgtag->Fill();
    b_sys_isr->Fill();
    b_sys_pu->Fill();
    b_sys_lep->Fill();

    sys_isr.clear();
    sys_mur.clear();
    sys_muf.clear();
    sys_murf.clear();
    sys_bctag.clear();
    sys_udsgtag.clear();
    sys_pu.clear();
    sys_lep.clear();
  }

  //
  vec_w_btag_dcsv.clear(); 
  vec_w_isr.clear(); 
  vec_w_lumi.clear(); 
  vec_w_lep.clear(); 
  vec_w_pu.clear(); 

  vec_sys_isr.clear();
  vec_sys_mur.clear();
  vec_sys_muf.clear();
  vec_sys_murf.clear();
  vec_sys_bctag.clear();
  vec_sys_udsgtag.clear();
  vec_sys_pu.clear();
  vec_sys_lep.clear();

  //
  file_new->cd();
  tree_new->Write();
  file_new->Close();

  // copy and remove output file
  cout << "... transferring output file" << endl;

  if(process.Contains("Run")) { // data
    if(skim=="rpvfitnbge0") {
      cout << Form("... cp Running/%s /data3/nanoprocessing/norm_230904_rpvfitnbge0_data/%s/%s", filename.Data(), year.Data(), process.Data()) << endl;  
      gSystem->Exec(Form("cp Running/%s /data3/nanoprocessing/norm_230904_rpvfitnbge0_data/%s/%s", filename.Data(), year.Data(), process.Data()));
    }
    else if(skim=="dy") {
      cout << Form("... cp Running/%s /data3/nanoprocessing/norm_230904_dy_data/%s/%s", filename.Data(), year.Data(), process.Data()) << endl;  
      gSystem->Exec(Form("cp Running/%s /data3/nanoprocessing/norm_230904_dy_data/%s/%s", filename.Data(), year.Data(), process.Data()));
    }
    cout << Form("rm Running/%s", filename.Data()) << endl;
    gSystem->Exec(Form("rm Running/%s", filename.Data()));
  }
  else { // mc
    if(skim=="rpvfitnbge0") {
      cout << Form("... cp Running/%s /data3/nanoprocessing/norm_230904_rpvfitnbge0/%s/%s", filename.Data(), year.Data(), process.Data()) << endl;  
      gSystem->Exec(Form("cp Running/%s /data3/nanoprocessing/norm_230904_rpvfitnbge0/%s/%s", filename.Data(), year.Data(), process.Data()));
    }
    else if(skim=="dy") {
      cout << Form("... cp Running/%s /data3/nanoprocessing/norm_230904_dy/%s/%s", filename.Data(), year.Data(), process.Data()) << endl;  
      gSystem->Exec(Form("cp Running/%s /data3/nanoprocessing/norm_230904_dy/%s/%s", filename.Data(), year.Data(), process.Data()));
    }
    cout << Form("rm Running/%s", filename.Data()) << endl;
    gSystem->Exec(Form("rm Running/%s", filename.Data()));
  }
}


# ifndef __CINT__  // the following code will be invisible for the interpreter
int main(int argc, char **argv)
{
//  ROOT::EnableImplicitMT(8);
  bool useCondor = false;
  
  TString year, process, skim;
  year    = argv[1];
  process = argv[2];
  skim    = argv[3];

  if(argc<4) {
    cout << " Please provide proper arguments" << endl;
    cout << "" << endl;
    cout << "   ./norm_weights.exe [year] [process]" << endl; 
    cout << "" << endl;
    cout << "   [year]: UL2016_preVFP, UL2016, UL2017, UL2018" << endl;
    cout << "   [skim]: rpvfitnbge0, dy" << endl;
    return 0;
  }
  
  cout << " year         : " << year << endl;
  cout << " process      : " << process << endl;
  cout << " skim         : " << skim << endl;
  
  vector<TString> prenorm_files; prenorm_files.clear();
  if(skim=="rpvfitnbge0") prenorm_files = getFileListFromFile(Form("flist/skimmed/%s/flist_%s_rpvfitnbge0.txt", year.Data(), process.Data()));
  else if(skim=="dy") prenorm_files = getFileListFromFile(Form("flist/skimmed/%s/flist_%s_dy.txt", year.Data(), process.Data()));
  else {
    cout << "please provide proper argument for skim" << endl;
    return 0;
  }


  // get mean
  TChain ch_mean("tree");	
  for(int i=0; i<prenorm_files.size(); i++)
  {
    cout << "TChain: " << prenorm_files.at(i) << endl;
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
  
//  TH1D  *h_weight_over_w_lumi = new TH1D("h_weight_over_w_lumi","h_weight_over_w_lumi",100,-5,5);
//  ch_mean.Draw("weight/(w_lumi*l1pre_nom)>>h_weight_over_w_lumi","","geoff");
//  weight_over_w_lumi_mean = h_weight_over_w_lumi->GetMean();
//  cout << "weight/(w_lumi mean*l1pre_nom) = " << weight_over_w_lumi_mean << endl;

  TH1D  *h_w_lep = new TH1D("h_w_lep","h_w_lep",100,-5,5);
  ch_mean.Draw("w_lep>>h_w_lep","","geoff");
  w_lep_mean = h_w_lep->GetMean();
  cout << "w_lep mean = " << w_lep_mean << endl;

  TH1D  *h_w_pu = new TH1D("h_w_pu","h_w_pu",100,-5,5);
  ch_mean.Draw("w_pu>>h_w_pu","","geoff");
  w_pu_mean = h_w_pu->GetMean();
  cout << "w_pu mean = " << w_pu_mean << endl;

  //----

  TH1D  *h_sys_mur_0 = new TH1D("h_sys_mur_0","h_sys_mur_0",100,-5,5);
  ch_mean.Draw("sys_mur[0]>>h_sys_mur_0","","geoff");
  sys_mur_mean_0 = h_sys_mur_0->GetMean();
  cout << "sys_mur mean down = " << sys_mur_mean_0 << endl;

  TH1D  *h_sys_mur_1 = new TH1D("h_sys_mur_1","h_sys_mur_1",100,-5,5);
  ch_mean.Draw("sys_mur[1]>>h_sys_mur_1","","geoff");
  sys_mur_mean_1 = h_sys_mur_1->GetMean();
  cout << "sys_mur mean up = " << sys_mur_mean_1 << endl;

  //----

  TH1D  *h_sys_muf_0 = new TH1D("h_sys_muf_0","h_sys_muf_0",100,-5,5);
  ch_mean.Draw("sys_muf[0]>>h_sys_muf_0","","geoff");
  sys_muf_mean_0 = h_sys_muf_0->GetMean();
  cout << "sys_muf mean down = " << sys_muf_mean_0 << endl;

  TH1D  *h_sys_muf_1 = new TH1D("h_sys_muf_1","h_sys_muf_1",100,-5,5);
  ch_mean.Draw("sys_muf[1]>>h_sys_muf_1","","geoff");
  sys_muf_mean_1 = h_sys_muf_1->GetMean();
  cout << "sys_muf mean up = " << sys_muf_mean_1 << endl;

  //----

  TH1D  *h_sys_murf_0 = new TH1D("h_sys_murf_0","h_sys_murf_0",100,-5,5);
  ch_mean.Draw("sys_murf[0]>>h_sys_murf_0","","geoff");
  sys_murf_mean_0 = h_sys_murf_0->GetMean();
  cout << "sys_murf mean down = " << sys_murf_mean_0 << endl;

  TH1D  *h_sys_murf_1 = new TH1D("h_sys_murf_1","h_sys_murf_1",100,-5,5);
  ch_mean.Draw("sys_murf[1]>>h_sys_murf_1","","geoff");
  sys_murf_mean_1 = h_sys_murf_1->GetMean();
  cout << "sys_murf mean up = " << sys_murf_mean_1 << endl;

  //----

  TH1D  *h_sys_bctag_0 = new TH1D("h_sys_bctag_0","h_sys_bctag_0",100,-5,5);
  ch_mean.Draw("sys_bctag[0]>>h_sys_bctag_0","","geoff");
  sys_bctag_mean_0 = h_sys_bctag_0->GetMean();
  cout << "sys_bctag mean down = " << sys_bctag_mean_0 << endl;

  TH1D  *h_sys_bctag_1 = new TH1D("h_sys_bctag_1","h_sys_bctag_1",100,-5,5);
  ch_mean.Draw("sys_bctag[1]>>h_sys_bctag_1","","geoff");
  sys_bctag_mean_1 = h_sys_bctag_1->GetMean();
  cout << "sys_bctag mean up = " << sys_bctag_mean_1 << endl;

  //----

  TH1D  *h_sys_udsgtag_0 = new TH1D("h_sys_udsgtag_0","h_sys_udsgtag_0",100,-5,5);
  ch_mean.Draw("sys_udsgtag[0]>>h_sys_udsgtag_0","","geoff");
  sys_udsgtag_mean_0 = h_sys_udsgtag_0->GetMean();
  cout << "sys_udsgtag mean down = " << sys_udsgtag_mean_0 << endl;

  TH1D  *h_sys_udsgtag_1 = new TH1D("h_sys_udsgtag_1","h_sys_udsgtag_1",100,-5,5);
  ch_mean.Draw("sys_udsgtag[1]>>h_sys_udsgtag_1","","geoff");
  sys_udsgtag_mean_1 = h_sys_udsgtag_1->GetMean();
  cout << "sys_udsgtag mean up = " << sys_udsgtag_mean_1 << endl;

  //----

  TH1D  *h_sys_isr_0 = new TH1D("h_sys_isr_0","h_sys_isr_0",100,-5,5);
  ch_mean.Draw("sys_isr[0]>>h_sys_isr_0","","geoff");
  sys_isr_mean_0 = h_sys_isr_0->GetMean();
  cout << "sys_isr mean down = " << sys_isr_mean_0 << endl;

  TH1D  *h_sys_isr_1 = new TH1D("h_sys_isr_1","h_sys_isr_1",100,-5,5);
  ch_mean.Draw("sys_isr[1]>>h_sys_isr_1","","geoff");
  sys_isr_mean_1 = h_sys_isr_1->GetMean();
  cout << "sys_isr mean up = " << sys_isr_mean_1 << endl;

  //----

  TH1D  *h_sys_pu_0 = new TH1D("h_sys_pu_0","h_sys_pu_0",100,-5,5);
  ch_mean.Draw("sys_pu[0]>>h_sys_pu_0","","geoff");
  sys_pu_mean_0 = h_sys_pu_0->GetMean();
  cout << "sys_pu mean down = " << sys_pu_mean_0 << endl;

  TH1D  *h_sys_pu_1 = new TH1D("h_sys_pu_1","h_sys_pu_1",100,-5,5);
  ch_mean.Draw("sys_pu[1]>>h_sys_pu_1","","geoff");
  sys_pu_mean_1 = h_sys_pu_1->GetMean();
  cout << "sys_pu mean up = " << sys_pu_mean_1 << endl;

  //----

  TH1D  *h_sys_lep_0 = new TH1D("h_sys_lep_0","h_sys_lep_0",100,-5,5);
  ch_mean.Draw("sys_lep[0]>>h_sys_lep_0","","geoff");
  sys_lep_mean_0 = h_sys_lep_0->GetMean();
  cout << "sys_lep mean down = " << sys_lep_mean_0 << endl;

  TH1D  *h_sys_lep_1 = new TH1D("h_sys_lep_1","h_sys_lep_1",100,-5,5);
  ch_mean.Draw("sys_lep[1]>>h_sys_lep_1","","geoff");
  sys_lep_mean_1 = h_sys_lep_1->GetMean();
  cout << "sys_lep mean up = " << sys_lep_mean_1 << endl;

  // 
  // Process 
  // 

  // get list of files in a directory
  cout << "processing " << prenorm_files.size() << " files" << endl;

  // process files one by one 
  for(int i=0; i<prenorm_files.size(); i++) {
    save_weights(prenorm_files.at(i));
        cout << "save_weights is finished" << endl;
    copy_onefile(prenorm_files.at(i)); 
	cout << "copy_onefile is finished" << endl;
    norm_onefile(prenorm_files.at(i), year, process, skim); 
	cout << "norm_onefile is finished" << endl;
  }
  return 0;
}
# endif
