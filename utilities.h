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
//elmu is true for electron, false for muon.
//
std::vector<int> getLepSFBin(TH2F *hist, const bool elmu, const float pt, const float eta){
  vector<int> ret;
  int binx(0), biny(0);
  float pt_ = (float)pt;
  float eta_ = (float)eta;
  if(elmu){
    pt_ = min(pt,(const float)499.999);
    eta_ = min(eta, (const float)2.499);
    binx = hist->GetXaxis()->FindBin(eta_);
    biny = hist->GetYaxis()->FindBin(pt_);
  }
  else{
    pt_ = min(pt,(const float)119.999);
    eta_ = min(eta, (const float)2.399);
    binx = hist->GetXaxis()->FindBin(pt_);
    biny = hist->GetYaxis()->FindBin(abs(eta_));
  }
  ret.push_back(binx);
  ret.push_back(biny);
  return ret;
}

std::vector<float> getLepSF(TFile *f, const bool elmu, const float pt, const float eta){
  vector<float> ret;

  float ret_SF(0);
  float ret_err(0);

  if(elmu){    
    vector<int> bin_iso;
    vector<int> bin_id;
    TH2F *SFiso = (TH2F*)f->Get("Run2016_CutBasedMediumNoIso94XV2");
    TH2F *SFid  = (TH2F*)f->Get("Run2016_Mini");
    bin_iso     = getLepSFBin(SFiso,elmu,pt,eta); 
    bin_id      = getLepSFBin(SFid,elmu,pt,eta); 

    float w_iso   = SFiso->GetBinContent(bin_iso.at(0), bin_iso.at(1));
    float w_id    = SFid->GetBinContent(bin_id.at(0), bin_id.at(1));
    float err_iso = SFiso->GetBinError(bin_iso.at(0), bin_iso.at(1));
    float err_id  = SFid->GetBinError(bin_id.at(0), bin_id.at(1));

    ret_SF        = w_iso*w_id;
    ret_err       = w_iso*w_id*sqrt(pow((err_iso/w_iso),2)+pow((err_id/w_id),2));
  }
  else{
    vector<int> bin;
    TH2F *SF      = (TH2F*)f->Get("SF");
    bin           = getLepSFBin(SF,elmu,pt,eta);
    ret_SF        = SF->GetBinContent(bin.at(0),bin.at(1));
    ret_err       = 0.03;
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
	else return true;
}// */
