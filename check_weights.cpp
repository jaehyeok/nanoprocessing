// ------------------------------------
// Author : Jae Hyeok Yoo
//          jaehyeokyoo@korea.ac.kr
// ------------------------------------
//

#include <cmath>
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


void check_weights(TString year, TString process, TString skim)
{


  vector<TString> files; files.clear();
  if(skim=="rpvfitnbge0") files = getFileListFromFile(Form("flist/norm/%s/flist_%s_rpvfitnbge0.txt", year.Data(), process.Data()));
  else if(skim=="dy") files = getFileListFromFile(Form("flist/norm/%s/flist_%s_dy.txt", year.Data(), process.Data()));
  else if(skim=="qcdfake") files = getFileListFromFile(Form("flist/norm/%s/flist_%s_qcdfake.txt", year.Data(), process.Data()));

  TChain *ch = new TChain("tree");
  for(int i=0; i<files.size(); i++) {
    ch->Add(files.at(i));
  }

  float weight=0;
  bool pass=0;
  ch->SetBranchAddress("weight", &weight);
  ch->SetBranchAddress("pass",	 &pass);

  //event loop
  for(int ievt=0; ievt<ch->GetEntries(); ievt++) {
    if(ievt%1000==0) cout << ievt << " / " << ch->GetEntries() << endl;
    ch->GetEntry(ievt);
    if((weight>1.5 || weight<-1) && pass==1) cout << "Entry: " << ievt << "/" << ch->GetEntries() << "    ||    weight: " << weight << endl;
    if(isnan(weight) == true) cout << "Entry: " << ievt << "/" << ch->GetEntries() << "    ||    weight: " << weight << endl;
    if(weight == TMath::Log(-1)) cout << "Entry: " << ievt << "/" << ch->GetEntries() << "    ||    weight: " << weight << endl;
  }
}


# ifndef __CINT__  // the following code will be invisible for the interpreter
int main(int argc, char **argv)
{
  
  TString year, process, skim;
  year    = argv[1];
  process = argv[2];
  skim	  = argv[3];

  if(argc<4) {
    cout << " Please provide proper arguments" << endl;
    cout << "" << endl;
    cout << "   ./check_weights.exe [year] [process] [skim]" << endl; 
    cout << "" << endl;
    cout << "   [year]: UL2016_preVFP, UL2016, UL2017, UL2018" << endl;
    cout << "   [skim]: rpvfitnbge0, dy" << endl;
    return 0;
  }
  
  cout << " year         : " << year << endl;
  cout << " process      : " << process << endl;
  cout << " skim         : " << skim << endl;
  
  check_weights(year, process, skim);

  return 0;
}
# endif
