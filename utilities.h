#include <glob.h>
#include <fstream>
#include "TString.h" 
#include "TMath.h" 
#include "TH2F.h"
#include "TFile.h"

using namespace std;

std::vector<int> getLepSFBin(TH2F *hist, const bool elmu, const float pt, const float eta);

// list of files in a directory
std::vector<TString> globVector(const TString& pattern){
  glob_t glob_result;
  glob(pattern.Data(),GLOB_TILDE,NULL,&glob_result);
  std::vector<TString> files;
  for(unsigned int i=0;i<glob_result.gl_pathc;++i){
    files.push_back(TString(glob_result.gl_pathv[i]));
  }
  globfree(&glob_result);
  return files;
}

//get Lepton Scale Factor
//elmu: true for electron, false for muon.
std::vector<int> getLepSFBin(TH2F *hist, const bool elmu, const float pt, const float eta, TString year){
  vector<int> ret;
  int binx(0), biny(0);
  float pt_  = (float)pt;
  float eta_ = (float)eta;
  if(elmu){                                   // elmu: true(electron), false(muon)
    // electron - binx: pt, biny: eta
    pt_  = min(pt,(const float)499.999);
    eta_ = min(eta, (const float)2.499);
    binx = hist->GetXaxis()->FindBin(eta_);
    biny = hist->GetYaxis()->FindBin(pt_);
  }
  else{
    // muon - [PL] binx: pt,       biny: abs(eta)
    //      - [UL] binx: abs(eta), biny: pt
    pt_ = min(pt,(const float)119.999);
    eta_ = min(eta, (const float)2.399);
    if(year=="2016"||year=="2017"||year=="2018"){
      binx = hist->GetXaxis()->FindBin(pt_);
      biny = hist->GetYaxis()->FindBin(abs(eta_));
    }
    else if(year=="UL2016_preVFP"||year=="UL2016"||year=="UL2017"||year=="UL2018"){
      binx = hist->GetXaxis()->FindBin(abs(eta_));
      biny = hist->GetYaxis()->FindBin(pt_);
    }
  }
  ret.push_back(binx);
  ret.push_back(biny);
  return ret;
}

std::vector<float> getLepSF(TFile *f, const bool elmu, const float pt, const float eta, const TString year){
  vector<float> ret;

  float ret_SF(0);
  float ret_err(0);
  float w_iso(0), w_id(0), err_iso(0), err_id(0);

  if(elmu){
    //electron
    vector<int> bin, bin_iso, bin_id;
    TH2F *SFiso, *SFid, *SF;
    if(year=="2016"||year=="2017"||year=="2018"){ 
      if(year=="2016"){ 
        SFiso = (TH2F*)f->Get("Run2016_CutBasedMediumNoIso94XV2");
        SFid  = (TH2F*)f->Get("Run2016_Mini");
      }
      else if(year=="2017"){
        SFiso = (TH2F*)f->Get("Run2017_CutBasedMediumNoIso94XV2");
        SFid  = (TH2F*)f->Get("Run2017_MVAVLooseTightIP2DMini");
      }
      else if(year=="2018"){
        SFiso = (TH2F*)f->Get("Run2018_CutBasedMediumNoIso94XV2");
        SFid  = (TH2F*)f->Get("Run2018_Mini");
      }
      bin_iso   = getLepSFBin(SFiso,elmu,pt,eta,year); 
      bin_id    = getLepSFBin(SFid,elmu,pt,eta,year); 
      w_iso     = SFiso->GetBinContent(bin_iso.at(0), bin_iso.at(1));
      w_id      = SFid->GetBinContent(bin_id.at(0),   bin_id.at(1));
      err_iso   = SFiso->GetBinError(bin_iso.at(0),   bin_iso.at(1));
      err_id    = SFid->GetBinError(bin_id.at(0),     bin_id.at(1));
      ret_SF    = w_iso*w_id;
      ret_err   = w_iso*w_id*sqrt(pow((err_iso/w_iso),2)+pow((err_id/w_id),2));
    }
    else if(year=="UL2016_preVFP"||year=="UL2016"||year=="UL2017"||year=="UL2018"){
      SF = (TH2F*)f->Get("EGamma_SF2D");
      bin = getLepSFBin(SF,elmu,pt,eta,year);
      ret_SF  = SF->GetBinContent(bin.at(0),bin.at(1));
      ret_err = SF->GetBinError(bin.at(0),bin.at(1));
    }
  }
  else{
    //muon
    vector<int> bin, bin_iso, bin_id;
    TH2F *SFiso, *SFid, *SF;
    if(year=="2016"){
      SF = (TH2F*)f->Get("SF");
      bin           = getLepSFBin(SF,elmu,pt,eta,year);
      ret_SF        = SF->GetBinContent(bin.at(0),bin.at(1));
      ret_err       = 0.03;
    }
    else if(year=="2017"||year=="2018"){
      SF = (TH2F*)f->Get("TnP_MC_NUM_MiniIso02Cut_DEN_MediumID_PAR_pt_eta");
      bin           = getLepSFBin(SF,elmu,pt,eta,year);
      ret_SF        = SF->GetBinContent(bin.at(0),bin.at(1));
      ret_err       = SF->GetBinError(bin.at(0),bin.at(1));
    }
    else if(year=="UL2016_preVFP"||year=="UL2016"||year=="UL2017"||year=="UL2018"){
      SF = (TH2F*)f->Get("NUM_RazorPass_DEN_genTracks_abseta_pt");
      bin = getLepSFBin(SF,elmu,pt,eta,year);
      ret_SF  = SF->GetBinContent(bin.at(0),bin.at(1));
      ret_err = SF->GetBinError(bin.at(0),bin.at(1));
    }
  }
  ret.push_back(ret_SF);
  ret.push_back(ret_err);

  return ret;
}

std::vector<TString> getFileListFromFile(const TString flist)
{
  vector<TString> files;  
  fstream fin(flist.Data());
  string line;
  cout<<flist<<endl;
  cout<<fin.is_open()<<endl;
  if(fin.is_open()) {
    while(fin.good()){
      getline(fin, line);
      if(line.find("#")!=string::npos ) continue;
      if(line!="") files.push_back(line);
      cout<<line<<endl; 
    }
  }
  fin.close();
  return files;
}

// deltaR
float deltaR(float deltaEta, float deltaPhi)
{ 
  return TMath::Sqrt(deltaEta*deltaEta+deltaPhi*deltaPhi);
}

float deltaR(float eta1, float eta2, float phi1, float phi2)
{ 
  return  deltaR(eta1-eta2, phi1-phi2);
}

// function that checks if a file exists
bool fexists(TString filename) {
	std::ifstream ifile(filename.Data());
	return (bool)ifile;
}

bool ProblematicEvent(TString inputfile, int event)
{
	if(inputfile.Contains("QCD_HT300to500") && (event==18175599 || event==210285751 || event==210368167 || event==39074182 || event==39076789 || event==453176473 || event==211199788 || event==307950415 || event==271562420 || event==430612207 )) return false;
	else if(inputfile.Contains("QCD_HT700to1000") && (event==522020269 || event==209665526 || event==191631428 || event==541547212 || event==520535617 || event==566676885 || event==77330664 || event==549571587 || event==370687549 || event==64164901 || event==169041749 || event==150278709 )) return false;
	else if(inputfile.Contains("QCD_HT1000to1500") && (event==192336035 || event==38179046 || event==38180511 || event==84518624 || event==41679647 )) return false;
	else if(inputfile.Contains("WZ_TuneCP5") && (event==3140552 || event==2587523 || event==2587541)) return false;
	else if(inputfile.Contains("TTZToLLNuNu_M-10") && event==4950593) return false;
	else if(inputfile.Contains("Run2016G") && event==9711159) return false;
	else return true;
}// */
