// ------------------------------------
// Author : Jae Hyeok Yoo
//          jaehyeok@hep.ucsb.edu
// ------------------------------------
//
// * This code is composed of 3 blocks
//  
// Block 1 : Read information about PF candidates from a cfA ntuple   
//           and write that information on a text file  
// Block 2 : Read the text file from (1) and run Fastjet package. 
//           The information of the recontructed jets will be 
//           written on a text file 
// Block 3 : Read the text file from (2) and make new branches in 
//           the input cfA in (1) to store the information 
// 
// * Pre-requisites
//
// (1) Download(http://fastjet.fr) and set up Fastjet package
//    * version 3.0.6(http://fastjet.fr/repo/fastjet-3.0.6.tar.gz) 
//    * manual : http://fastjet.fr/repo/fastjet-doc-3.0.6.pdf
//      --> Chapter 2 has an instruction to set up the code
// (2) Turn off some of the printouts so that fastjet_example 
//     prints out only information of PF candidates, i.e., 
//     no table header, ... 
// 
// * To run this code, do
//   
//      $root -b -q makeJetP4_notext.C++\(\"cfA_QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6_Summer12_DR53X-PU_S10_START53_V7A-v1_AODSIM_UCSB2047_v71_f6_1_OWX_Test.root\",1.2,10\)
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
#include "TObjString.h"

#include "utilities.h"  

// compile
//  $ g++ skim.cpp  `root-config --cflags --glibs` -o skim.exe

using namespace std;

const bool DEBUG = false; 

void skimfile(TString year, TString process, TString skim) 
{

  // ------------------------------------------------------ //
  // Before running this code, do                           //
  // python3 make_flist_processed.py [year] [mc/data]       //
  // for getting flist in nanoprocessing/flist/processed/   //
  // ------------------------------------------------------ //

  // skim cut
  TString skimcut="0";
  if(skim=="rpvfit") {
    skimcut="ht>1200&&mj12>500&&njets>=4";
  }
  else if(skim=="hem") {
    skimcut="ht>1200&&mj12>500&&njets>=4";
  }
  else if(skim=="trig") {
    skimcut="mj12>500&&njets>=4";
  }
  else if(skim=="rpvfitnbge0") {
    // Data
    if(process.Contains("Run")) skimcut = "ht>1200 && mj12>500 && njets>=4";
    // MC
    else if(process.Contains("TTJets_Tune")) skimcut="(sys_ht[0]+Sum$(leps_pt)>1200 || sys_ht[1]+Sum$(leps_pt)>1200 || ht+Sum$(leps_pt)>1200) && (sys_mj12[0]>500 || sys_mj12[1]>500 || mj12>500) && (sys_njets[0]>=4 || sys_njets[1]>=4 || njets>=4) && stitch_ht==1";
    else skimcut="(sys_ht[0]+Sum$(leps_pt)>1200 || sys_ht[1]+Sum$(leps_pt)>1200 || ht+Sum$(leps_pt)>1200) && (sys_mj12[0]>500 || sys_mj12[1]>500 || mj12>500) && (sys_njets[0]>=4 || sys_njets[1]>=4 || njets>=4)";
  }
  else if(skim=="dy") {
    // Data
    if(process.Contains("Run")) skimcut = "ht>1200 && mj12>500 && njets>=3 && nbm<=2 && nleps==2";
    // MC
    else if(process.Contains("TTJets_Tune")) skimcut="(sys_ht[0]+Sum$(leps_pt)>1200 || sys_ht[1]+Sum$(leps_pt)>1200 || ht+Sum$(leps_pt)>1200) && (sys_mj12[0]>500 || sys_mj12[1]>500 || mj12>500) && (sys_njets[0]>=3 || sys_njets[1]>=3 || njets>=3) && stitch_ht==1 && nleps==2";
    else skimcut="(sys_ht[0]+Sum$(leps_pt)>1200 || sys_ht[1]+Sum$(leps_pt)>1200 || ht+Sum$(leps_pt)>1200) && (sys_mj12[0]>500 || sys_mj12[1]>500 || mj12>500) && (sys_njets[0]>=3 || sys_njets[1]>=3 || njets>=3) && nleps==2";
  }
  else if(skim=="4top") {
    skimcut="(njets>=4) && (nbm>=1) && (mj12>150) && (ht>350)";
  }
  else if(skim=="qcdfake") {
    skimcut="(ht>1200) && (njets>=4) && (mj12<500)";
  }

  else {
    cout << "enter a correct skim parameter!" << endl;
  }
  
  // make output file
  vector<TString> files; files.clear();
  files = getFileListFromFile(Form("flist/processed/%s/flist_%s.txt", year.Data(), process.Data()));

  for(int i=0; i<files.size(); i++) {

    TChain *ch = new TChain("tree");
    ch->Add(files.at(i));

    // name output file
    TObjArray *tokens = files.at(i).Tokenize("/");
    TString filename = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries()-1)))->GetString();
    filename.ReplaceAll(".root", Form("_%s_%s.root", "skimmed", skim.Data()));

    TFile *outputfile = new TFile("/data3/nanoprocessing/Running/"+filename, "recreate");
    TTree *ctree = ch->CopyTree(skimcut);
    outputfile->cd();

    // do not make output file if entry of output file is zero
    int p = ctree->GetEntries();
    if(p==0) {
      cout << "GetEntries() = 0" << endl;
      outputfile->Close();
      cout << Form("rm /data3/nanoprocessing/Running/%s", filename.Data()) << endl;
      gSystem->Exec(Form("rm /data3/nanoprocessing/Running/%s", filename.Data()));

      continue;
    }
    if(ctree) ctree->Write();
    outputfile->Close();

    // move and remove files
    if(skim=="rpvfitnbge0") {
      if(process.Contains("Run")) { // data
        cout << "... transferring output file" << endl;
        cout << Form("... cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_241201_data/%s/%s", filename.Data(), year.Data(), process.Data()) << endl;
        gSystem->Exec(Form("cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_241201_data/%s/%s", filename.Data(), year.Data(), process.Data()));
        cout << Form("rm /data3/nanoprocessing/Running/%s", filename.Data()) << endl;
        gSystem->Exec(Form("rm /data3/nanoprocessing/Running/%s", filename.Data()));
      }
      else { // mc
        cout << "... transferring output file" << endl;
        cout << Form("... cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_241201/%s/%s", filename.Data(), year.Data(), process.Data()) << endl;
        gSystem->Exec(Form("cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_241201/%s/%s", filename.Data(), year.Data(), process.Data()));
        cout << Form("rm /data3/nanoprocessing/Running/%s", filename.Data()) << endl;
        gSystem->Exec(Form("rm /data3/nanoprocessing/Running/%s", filename.Data()));
      }
    }
    else if(skim=="dy"){
      if(process.Contains("Run")) { // data
        cout << "... transferring output file" << endl;
        cout << Form("... cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_230929_dy_data/%s/%s", filename.Data(), year.Data(), process.Data()) << endl;
        gSystem->Exec(Form("cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_230929_dy_data/%s/%s", filename.Data(), year.Data(), process.Data()));
        cout << Form("rm /data3/nanoprocessing/Running/%s", filename.Data()) << endl;
        gSystem->Exec(Form("rm /data3/nanoprocessing/Running/%s", filename.Data()));
      }
      else { // mc
        cout << "... transferring output file" << endl;
        cout << Form("... cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_230929_dy/%s/%s", filename.Data(), year.Data(), process.Data()) << endl;
        gSystem->Exec(Form("cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_230929_dy/%s/%s", filename.Data(), year.Data(), process.Data()));
        cout << Form("rm /data3/nanoprocessing/Running/%s", filename.Data()) << endl;
        gSystem->Exec(Form("rm /data3/nanoprocessing/Running/%s", filename.Data()));
      }
    }
    else if(skim=="4top") {
      if(process.Contains("Run")) { // data
        cout << "... transferring output file" << endl;
        cout << Form("... cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_231020_4top_data/%s/%s", filename.Data(), year.Data(), process.Data()) << endl;
        gSystem->Exec(Form("cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_231020_4top_data/%s/%s", filename.Data(), year.Data(), process.Data()));
        cout << Form("rm /data3/nanoprocessing/Running/%s", filename.Data()) << endl;
        gSystem->Exec(Form("rm /data3/nanoprocessing/Running/%s", filename.Data()));
      }
      else { // mc
        cout << "... transferring output file" << endl;
        cout << Form("... cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_231020_4top/%s/%s", filename.Data(), year.Data(), process.Data()) << endl;
        gSystem->Exec(Form("cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_231020_4top/%s/%s", filename.Data(), year.Data(), process.Data()));
        cout << Form("rm /data3/nanoprocessing/Running/%s", filename.Data()) << endl;
        gSystem->Exec(Form("rm /data3/nanoprocessing/Running/%s", filename.Data()));
      }
    }
    else if(skim=="qcdfake") {
      if(process.Contains("Run")) { // data
        cout << "... transferring output file" << endl;
        cout << Form("... cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_231020_qcdfake_data/%s/%s", filename.Data(), year.Data(), process.Data()) << endl;
        gSystem->Exec(Form("cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_231020_qcdfake_data/%s/%s", filename.Data(), year.Data(), process.Data()));
        cout << Form("rm /data3/nanoprocessing/Running/%s", filename.Data()) << endl;
        gSystem->Exec(Form("rm /data3/nanoprocessing/Running/%s", filename.Data()));
      }
      else { // mc
        cout << "... transferring output file" << endl;
        cout << Form("... cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_231020_qcdfake/%s/%s", filename.Data(), year.Data(), process.Data()) << endl;
        gSystem->Exec(Form("cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_231020_qcdfake/%s/%s", filename.Data(), year.Data(), process.Data()));
        cout << Form("rm /data3/nanoprocessing/Running/%s", filename.Data()) << endl;
        gSystem->Exec(Form("rm /data3/nanoprocessing/Running/%s", filename.Data()));
      }
    }
    else if(skim=="trig") {
      cout << "... transferring output file" << endl;
      cout << Form("... cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_231124_singlemu_data/%s/%s", filename.Data(), year.Data(), process.Data()) << endl;
      gSystem->Exec(Form("cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_231124_singlemu_data/%s/%s", filename.Data(), year.Data(), process.Data()));
      cout << Form("rm /data3/nanoprocessing/Running/%s", filename.Data()) << endl;
      gSystem->Exec(Form("rm /data3/nanoprocessing/Running/%s", filename.Data()));

    }
    else if(skim=="hem") {
      cout << "... transferring output file" << endl;
      cout << Form("... cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_241013_data_hem/%s/%s", filename.Data(), year.Data(), process.Data()) << endl;
      gSystem->Exec(Form("cp /data3/nanoprocessing/Running/%s /data3/nanoprocessing/skimmed_241013_data_hem/%s/%s", filename.Data(), year.Data(), process.Data()));
      cout << Form("rm /data3/nanoprocessing/Running/%s", filename.Data()) << endl;
      gSystem->Exec(Form("rm /data3/nanoprocessing/Running/%s", filename.Data()));

    }
      delete outputfile;
      cout << i << "th file is skimmed" << endl;

  }
}


# ifndef __CINT__  // the following code will be invisible for the interpreter
int main(int argc, char **argv)
//int main()
{
  int nthreads = 16;
//  ROOT::EnableImplicitMT(nthreads);

  TString year, process, skim;
  year	  = argv[1];
  process = argv[2];
  skim	  = argv[3];

  if(argc<4)
  {
    cout << " Please provide proper arguments" << endl;
    cout << "" << endl;
    cout << "   ./skim.exe [year] [process] [skim]" << endl; 
    cout << "" << endl;
    cout << " skim = rpvfitnbge0, dy" << endl;
    return 0;
  }
  else 
  {
    cout << " year 	 		: " << year << endl;
    cout << " process   		: " << process << endl;
    cout << " skim	   		: " << skim << endl;
  }

  skimfile(year, process, skim); 

  return 0;
}
# endif
