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
//gStyle->SetOptStat(0);

void valid_TTJets_lheht(TString year)
{
  TH1::SetDefaultSumw2();

  TChain *ch_incl = new TChain("tree");

  if(year=="UL2016_preVFP") {
    ch_incl->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/TTJets_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11-v1/40000/*.root");
  }
  else if(year=="UL2016") {
    ch_incl->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/TTJets_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v1/2820000/*.root");
  }
  else if(year=="UL2017") {
    ch_incl->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/TTJets_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/40000/*.root");
  }
  else if(year=="UL2018") {
    ch_incl->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/TTJets_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/2820000/*.root");
  }
  else if(year=="2016") {
    ch_incl->Add("/data2/nanoprocessing/2016/processed_0317/*TTJets_Tune*.root");
  }
  else if(year=="2017") {
    ch_incl->Add("/data2/nanoprocessing/2017/processed_0317/*TTJets_Tune*.root");
  }
  else if(year=="2018") {
    ch_incl->Add("/data2/nanoprocessing/2018/processed_0317/*TTJets_Tune*.root");
  }

  // Define histograms for lhe_ht
  TH1D *h_incl = new TH1D("lhe_ht_Inclusive_TTJets", "lhe_ht_Inclusive_TTJets", 100, 0, 4000);
  // Draw lhe_ht
  ch_incl->Draw("min(lhe_ht, 3999.9999) >> lhe_ht_Inclusive_TTJets", "w_lumi");

  // fill histogram

  TLegend *leg_lhe_ht = new TLegend(0.65,0.7,0.87,0.89);
  leg_lhe_ht->AddEntry(h_incl,   "lhe_ht_Inclusive_TTJets");

  TCanvas *c_lhe_ht = new TCanvas("c_lhe_ht","c_lhe_ht", 3600, 2400);
  h_incl->SetMaximum(1000000.);
  h_incl->SetMinimum(0.1);
  h_incl->SetLineWidth(2);
  h_incl->Draw("hist");
//  leg_lhe_ht->Draw();
  gPad->SetLogy();
  
  c_lhe_ht->Print("TTJets_lheht_issue/lhe_ht_TTJets_"+year+"_incl.png");

/*
  //////////////////////// ht ////////////////////////

  // Define histograms for ht
  TH1D *h_ht_600to800 = new TH1D("ht_TTJets_HT-600to800", "ht_TTJets_HT-600to800", 100, 0, 4000);
  TH1D *h_ht_800to1200 = new TH1D("ht_TTJets_HT-800to1200", "ht_TTJets_HT-800to1200", 100, 0, 4000);
  TH1D *h_ht_1200to2500 = new TH1D("ht_TTJets_HT-1200to2500", "ht_TTJets_HT-1200to2500", 100, 0, 4000);
  TH1D *h_ht_2500toInf = new TH1D("ht_TTJets_HT-2500toInf", "ht_TTJets_HT-2500toInf", 100, 0, 4000);
  // Draw ht
  ch_600to800  ->Draw("min(ht, 3999.9999) >> ht_TTJets_HT-600to800",   "w_lumi");
  ch_800to1200 ->Draw("min(ht, 3999.9999) >> ht_TTJets_HT-800to1200",  "w_lumi");
  ch_1200to2500->Draw("min(ht, 3999.9999) >> ht_TTJets_HT-1200to2500", "w_lumi");
  ch_2500toInf ->Draw("min(ht, 3999.9999) >> ht_TTJets_HT-2500toInf",  "w_lumi");

  // calculate fraction out of range
  float f_ht_600to800, f_ht_800to1200, f_ht_1200to2500, f_ht_2500toInf;
  f_ht_600to800 = static_cast<float>(ch_600to800  ->GetEntries("ht<600 || ht>800")) / static_cast<float>(ch_600to800->GetEntries());
  f_ht_800to1200 = static_cast<float>(ch_800to1200  ->GetEntries("ht<800 || ht>1200")) / static_cast<float>(ch_800to1200->GetEntries());
  f_ht_1200to2500 = static_cast<float>(ch_1200to2500  ->GetEntries("ht<1200 || ht>2500")) / static_cast<float>(ch_1200to2500->GetEntries());
  f_ht_2500toInf = static_cast<float>(ch_2500toInf  ->GetEntries("ht<2500")) / static_cast<float>(ch_2500toInf->GetEntries());
  cout << "f_ht_600to800: " << f_ht_600to800 << endl;
  cout << "f_ht_800to1200: " << f_ht_800to1200 << endl;
  cout << "f_ht_1200to2500: " << f_ht_1200to2500 << endl;
  cout << "f_ht_2500toInf: " << f_ht_2500toInf << endl;

  // fill histogram
  auto *h_ht_incl = new THStack("ht_TTJets_Incl", "ht_TTJets_Incl");
  h_ht_incl->Add(h_ht_600to800);
  h_ht_incl->Add(h_ht_800to1200);
  h_ht_incl->Add(h_ht_1200to2500);
  h_ht_incl->Add(h_ht_2500toInf);

  TLegend *leg_ht = new TLegend(0.65,0.7,0.87,0.89);
  leg_ht->AddEntry(h_ht_600to800,   "ht_TTJets_HT-600to800");
  leg_ht->AddEntry(h_ht_800to1200,  "ht_TTJets_HT-800to1200");
  leg_ht->AddEntry(h_ht_1200to2500, "ht_TTJets_HT-1200to2500");
  leg_ht->AddEntry(h_ht_2500toInf,  "ht_TTJets_HT-2500toInf");

  TCanvas *c_ht = new TCanvas("c_ht","c_ht", 3600, 2400);
  c_ht->Divide(2,2);
  c_ht->cd(1);
  h_ht_600to800->SetMaximum(1000.);
  h_ht_600to800->SetMinimum(1.);
  h_ht_600to800->SetLineWidth(2);
  h_ht_600to800->Draw("hist");
  gPad->SetLogy();
  c_ht->cd(2);
  h_ht_800to1200->SetMaximum(100.);
  h_ht_800to1200->SetMinimum(1.);
  h_ht_800to1200->SetLineWidth(2);
  h_ht_800to1200->Draw("hist");
  gPad->SetLogy();
  c_ht->cd(3);
  h_ht_1200to2500->SetMaximum(20.);
  h_ht_1200to2500->SetMinimum(1.);
  h_ht_1200to2500->SetLineWidth(2);
  h_ht_1200to2500->Draw("hist");
  gPad->SetLogy();
  c_ht->cd(4);
  h_ht_2500toInf->SetMaximum(1.);
  h_ht_2500toInf->SetMinimum(0.01);
  h_ht_2500toInf->SetLineWidth(2);
  h_ht_2500toInf->Draw("hist");
  gPad->SetLogy();
  
  c_ht->Print("TTJets_lheht_issue/ht_TTJets_"+year+".png");

  TCanvas *c_ht_incl = new TCanvas("c_ht_incl", "c_ht_incl", 2400, 1200);
  c_ht_incl->cd();
  h_ht_600to800->SetFillColor(kRed-7);
  h_ht_800to1200->SetFillColor(kBlue-7);
  h_ht_1200to2500->SetFillColor(kGreen-7);
  h_ht_2500toInf->SetFillColor(kGray+1);
  h_ht_incl->SetMaximum(2000.);
  h_ht_incl->SetMinimum(1.);
  h_ht_incl->Draw("hist");
  leg_ht->SetTextSize(0.025);
  leg_ht->Draw();
  gPad->SetLogy();

  c_ht_incl->Print("TTJets_lheht_issue/ht_TTJets_incl_"+year+".png");
*/



}

int main(int argc, char **argv)
{

  TString year;
  year = argv[1];
  valid_TTJets_lheht(year);


  return 0;

}
