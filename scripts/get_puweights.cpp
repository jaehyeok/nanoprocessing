#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream> // for stringstream
#include <map>

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
#include "TH3D.h"
#include "TLegend.h"

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


void get_puweights(TString year, TString process)
{
  TH1::SetDefaultSumw2();


  vector<TString> files;
  files = getFileListFromFile(Form("flist/%s/flist_%s.txt", year.Data(), process.Data()));
  TChain *ch = new TChain("Events");

  // TChain MC files
  for(int i=0; i<files.size(); i++) {
    ch->Add(files.at(i));
  }
  cout << "Number of files: " << files.size() << endl;
  cout << "GetEntries: " << ch->GetEntries() << endl;

  // read pileup from data
  TFile *f_pu_data_down, *f_pu_data_nominal, *f_pu_data_up;
  f_pu_data_down = new TFile("data/ultralegacy/pileup/"+year+"/pileup_"+year+"_down.root", "READ");
  f_pu_data_nominal = new TFile("data/ultralegacy/pileup/"+year+"/pileup_"+year+"_nominal.root", "READ");
  f_pu_data_up = new TFile("data/ultralegacy/pileup/"+year+"/pileup_"+year+"_up.root", "READ");
  cout << "pu_down file: " << f_pu_data_down->GetName() << endl;
  cout << "pu_nominal file: " << f_pu_data_nominal->GetName() << endl;
  cout << "pu_up file: " << f_pu_data_up->GetName() << endl;

  // get pileup distribution from data
  TH1D *pu_data_down, *pu_data_nominal, *pu_data_up;
  pu_data_down =    (TH1D*)f_pu_data_down->Get("pileup");
  pu_data_nominal = (TH1D*)f_pu_data_nominal->Get("pileup");
  pu_data_up =      (TH1D*)f_pu_data_up->Get("pileup");

  // get pileup distribution from MC
  TH1D *pu_mc = new TH1D("pu_mc", "pu_mc", 99, 0, 99);
  ch->Draw("Pileup_nTrueInt>>pu_mc", "", "norm");

  // normalize pileup of data
  pu_data_down->Scale(1./pu_data_down->Integral());
  pu_data_nominal->Scale(1./pu_data_nominal->Integral());
  pu_data_up->Scale(1./pu_data_up->Integral());
  
  // get pileup weight: dividing data by MC
  TH1D *w_pu_down, *w_pu_nominal, *w_pu_up;
  w_pu_down = static_cast<TH1D*>(pu_data_down->Clone("w_pu_down"));
  w_pu_nominal = static_cast<TH1D*>(pu_data_nominal->Clone("w_pu_nominal"));
  w_pu_up = static_cast<TH1D*>(pu_data_up->Clone("w_pu_up"));
  w_pu_down->Divide(pu_mc);
  w_pu_nominal->Divide(pu_mc);
  w_pu_up->Divide(pu_mc);

  TCanvas *c1 = new TCanvas("c1","c1",1200,600);
  TCanvas *c2 = new TCanvas("c2","c2",1200,600);
  // Draw pileup distributions of data and MC
//  c1->SetLogy();
  c1->cd();
  c1->SetName(process+"_"+year);
  pu_data_nominal->SetLineColor(kBlack);
  pu_mc->SetLineColor(kBlue);
  pu_data_nominal->Draw();
  pu_mc->Draw("same");

  // define TLegend
  TLegend *l1 = new TLegend(0.65,0.65,0.85,0.75);
  l1->AddEntry(pu_data_nominal, "data");
  l1->AddEntry(pu_mc, "mc");
  l1->SetTextSize(0.04);
  l1->Draw();

  // Draw pileup weight
  c2->cd();
  c2->SetName(process+"_"+year);
  w_pu_nominal->SetLineColor(kRed);
  w_pu_nominal->Draw();

  // define TLegend
  TLegend *l2 = new TLegend(0.65,0.65,0.85,0.75);
  l2->AddEntry(w_pu_nominal, "weight");
  l2->SetTextSize(0.04);
  l2->Draw();

  c1->Print("data/ultralegacy/pileup/"+year+"/pdf/pu_distribution_"+process+"_"+year+".pdf");
  c2->Print("data/ultralegacy/pileup/"+year+"/pdf/pu_weight_"+process+"_"+year+".pdf");

  // Save pileup reweight root file
  TFile *f_pu_mc;
  f_pu_mc = new TFile("data/ultralegacy/pileup/weight/"+year+"/pu_weight_"+process+"_"+year+".root","recreate");
  f_pu_mc->cd();
  w_pu_down->Write();
  w_pu_nominal->Write();
  w_pu_up->Write();

  f_pu_mc->Close();
}

int main(int argc, char **argv)
{
  TString year, process;
  year    = argv[1];
  process = argv[2];
/*
  if(year!="UL2016_preVFP"||year!="UL2016"||year!="UL2017"||year!="UL2018") {
    cout << "Please provide proper years" << endl;
    cout << "    year:   UL2016_preVFP, UL2016, UL2017, UL2018" << endl;
    return 0;
  }
*/
  get_puweights(year, process);



  return 0;
}
