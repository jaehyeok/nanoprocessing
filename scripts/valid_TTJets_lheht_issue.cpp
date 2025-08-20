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

void valid_DY_lheht(TString year)
{
  TH1::SetDefaultSumw2();

  TChain *ch_600to800 = new TChain("tree");
  TChain *ch_800to1200 = new TChain("tree");
  TChain *ch_1200to2500 = new TChain("tree");
  TChain *ch_2500toInf = new TChain("tree");

  if(year=="UL2016_preVFP") {
    ch_600to800->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/DYJetsToLL_M-50_HT-600to800_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11-v2/230000/*.root");
    ch_800to1200->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/DYJetsToLL_M-50_HT-800to1200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11-v2/230000/*.root");
    ch_1200to2500->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/DYJetsToLL_M-50_HT-1200to2500_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11-v2/2520000/*.root");
    ch_2500toInf->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/DYJetsToLL_M-50_HT-2500toInf_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11-v2/100000/*.root");
  }
  else if(year=="UL2016") {
    ch_600to800->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/DYJetsToLL_M-50_HT-600to800_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v2/40000/*.root");
    ch_800to1200->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/DYJetsToLL_M-50_HT-800to1200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v2/260000/*.root");
    ch_1200to2500->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/DYJetsToLL_M-50_HT-1200to2500_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v2/260000/*.root");
    ch_2500toInf->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/DYJetsToLL_M-50_HT-2500toInf_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v2/40000/*.root");
  }
  else if(year=="UL2017") {
    ch_600to800->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/DYJetsToLL_M-50_HT-600to800_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/70000/*.root");
    ch_800to1200->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/DYJetsToLL_M-50_HT-800to1200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/70000/*.root");
    ch_1200to2500->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/DYJetsToLL_M-50_HT-1200to2500_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/260000/*.root");
    ch_2500toInf->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/DYJetsToLL_M-50_HT-2500toInf_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/260000/*.root");
  }
  else if(year=="UL2018") {
    ch_600to800->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/DYJetsToLL_M-50_HT-600to800_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/70000/*.root");
    ch_800to1200->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/DYJetsToLL_M-50_HT-800to1200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/70000/*.root");
    ch_1200to2500->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/DYJetsToLL_M-50_HT-1200to2500_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/70000/*.root");
    ch_2500toInf->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/DYJetsToLL_M-50_HT-2500toInf_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/40000/*.root");
  }

  TH1D *h_600to800 = new TH1D("DYJetsToLL_M-50_HT-600to800", "DYJetsToLL_M-50_HT-600to800", 100, 0, 4000);
  TH1D *h_800to1200 = new TH1D("DYJetsToLL_M-50_HT-800to1200", "DYJetsToLL_M-50_HT-800to1200", 100, 0, 4000);
  TH1D *h_1200to2500 = new TH1D("DYJetsToLL_M-50_HT-1200to2500", "DYJetsToLL_M-50_HT-1200to2500", 100, 0, 4000);
  TH1D *h_2500toInf = new TH1D("DYJetsToLL_M-50_HT-2500toInf", "DYJetsToLL_M-50_HT-2500toInf", 100, 0, 4000);
  ch_600to800  ->Draw("min(lhe_ht, 3999.9999) >> DYJetsToLL_M-50_HT-600to800");
  ch_800to1200 ->Draw("min(lhe_ht, 3999.9999) >> DYJetsToLL_M-50_HT-800to1200");
  ch_1200to2500->Draw("min(lhe_ht, 3999.9999) >> DYJetsToLL_M-50_HT-1200to2500");
  ch_2500toInf ->Draw("min(lhe_ht, 3999.9999) >> DYJetsToLL_M-50_HT-2500toInf");

  auto *h_incl = new THStack("DYJetsToLL_M-50_Incl", "DYJetsToLL_M-50_Incl");
  h_incl->Add(h_600to800);
  h_incl->Add(h_800to1200);
  h_incl->Add(h_1200to2500);
  h_incl->Add(h_2500toInf);

  TLegend *leg = new TLegend(0.65,0.7,0.89,0.89);
  leg->AddEntry(h_600to800,   "DYJetsToLL_M-50_HT-600to800");
  leg->AddEntry(h_800to1200,  "DYJetsToLL_M-50_HT-800to1200");
  leg->AddEntry(h_1200to2500, "DYJetsToLL_M-50_HT-1200to2500");
  leg->AddEntry(h_2500toInf,  "DYJetsToLL_M-50_HT-2500toInf");

  TCanvas *c = new TCanvas("c","c", 3600, 2400);
  c->Divide(2,2);
  c->cd(1);
  h_600to800->SetMaximum(10000000.);
  h_600to800->SetMinimum(1.);
  h_600to800->SetLineWidth(2);
  h_600to800->Draw("hist");
  gPad->SetLogy();
  c->cd(2);
  h_800to1200->SetMaximum(10000000.);
  h_800to1200->SetMinimum(1.);
  h_800to1200->SetLineWidth(2);
  h_800to1200->Draw("hist");
  gPad->SetLogy();
  c->cd(3);
  h_1200to2500->SetMaximum(10000000.);
  h_1200to2500->SetMinimum(1.);
  h_1200to2500->SetLineWidth(2);
  h_1200to2500->Draw("hist");
  gPad->SetLogy();
  c->cd(4);
  h_2500toInf->SetMaximum(10000000.);
  h_2500toInf->SetMinimum(1.);
  h_2500toInf->SetLineWidth(2);
  h_2500toInf->Draw("hist");
  gPad->SetLogy();
  
  c->Print("TTJets_lheht_issue/DYJetsToLL_M-50_lheht_issue_"+year+".png");

  TCanvas *c_incl = new TCanvas("c_incl", "c_incl", 2400, 1200);
  c_incl->cd();
  h_600to800->SetFillColor(kRed-7);
  h_800to1200->SetFillColor(kBlue-7);
  h_1200to2500->SetFillColor(kGreen-7);
  h_2500toInf->SetFillColor(kGray+1);
  h_incl->SetMaximum(10000000.);
  h_incl->SetMinimum(1.);
  h_incl->Draw("hist");
  leg->SetTextSize(0.025);
  leg->Draw();
  gPad->SetLogy();

  c_incl->Print("TTJets_lheht_issue/DYJetsToLL_M-50_lheht_issue_incl_"+year+".png");


}


void valid_QCD_lheht(TString year)
{
  TH1::SetDefaultSumw2();

  TChain *ch_700to1000 = new TChain("tree");
  TChain *ch_1000to1500 = new TChain("tree");
  TChain *ch_1500to2000 = new TChain("tree");
  TChain *ch_2000toInf = new TChain("tree");

  if(year=="UL2016_preVFP") {
    ch_700to1000->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/QCD_HT700to1000_TuneCP5_PSWeights_13TeV-madgraph-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11-v1/270000/*.root");
    ch_1000to1500->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/QCD_HT1000to1500_TuneCP5_PSWeights_13TeV-madgraph-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11-v1/270000/*.root");
    ch_1500to2000->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/QCD_HT1500to2000_TuneCP5_PSWeights_13TeV-madgraph-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11-v1/270000/*.root");
    ch_2000toInf->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/QCD_HT2000toInf_TuneCP5_PSWeights_13TeV-madgraph-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11-v1/30000/*.root");
  }
  else if(year=="UL2016") {
    ch_700to1000->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/QCD_HT700to1000_TuneCP5_PSWeights_13TeV-madgraph-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v1/270000/*.root");
    ch_1000to1500->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/QCD_HT1000to1500_TuneCP5_PSWeights_13TeV-madgraph-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v1/270000/*.root");
    ch_1500to2000->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/QCD_HT1500to2000_TuneCP5_PSWeights_13TeV-madgraph-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v1/270000/*.root");
    ch_2000toInf->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/QCD_HT2000toInf_TuneCP5_PSWeights_13TeV-madgraph-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v1/270000/*.root");
  }
  else if(year=="UL2017") {
    ch_700to1000->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/QCD_HT700to1000_TuneCP5_PSWeights_13TeV-madgraph-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/270000/*.root");
    ch_1000to1500->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/QCD_HT1000to1500_TuneCP5_PSWeights_13TeV-madgraph-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/80000/*.root");
    ch_1500to2000->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/QCD_HT1500to2000_TuneCP5_PSWeights_13TeV-madgraph-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/40000/*.root");
    ch_2000toInf->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/QCD_HT2000toInf_TuneCP5_PSWeights_13TeV-madgraph-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/80000/*.root");
  }
  else if(year=="UL2018") {
    ch_700to1000->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/QCD_HT700to1000_TuneCP5_PSWeights_13TeV-madgraph-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v2/260000/*.root");
    ch_1000to1500->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/QCD_HT1000to1500_TuneCP5_PSWeights_13TeV-madgraph-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/810000/*.root");
    ch_1500to2000->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/QCD_HT1500to2000_TuneCP5_PSWeights_13TeV-madgraph-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/30000/*.root");
    ch_2000toInf->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/QCD_HT2000toInf_TuneCP5_PSWeights_13TeV-madgraph-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/30000/*.root");
  }

  TH1D *h_700to1000 = new TH1D("QCD_HT-700to1000", "QCD_HT-700to1000", 100, 0, 4000);
  TH1D *h_1000to1500 = new TH1D("QCD_HT-1000to1500", "QCD_HT-1000to1500", 100, 0, 4000);
  TH1D *h_1500to2000 = new TH1D("QCD_HT-1500to2000", "QCD_HT-1500to2000", 100, 0, 4000);
  TH1D *h_2000toInf = new TH1D("QCD_HT-2000toInf", "QCD_HT-2000toInf", 100, 0, 4000);
  ch_700to1000  ->Draw("min(lhe_ht, 3999.9999) >> QCD_HT-700to1000");
  ch_1000to1500 ->Draw("min(lhe_ht, 3999.9999) >> QCD_HT-1000to1500");
  ch_1500to2000->Draw("min(lhe_ht, 3999.9999)  >> QCD_HT-1500to2000");
  ch_2000toInf ->Draw("min(lhe_ht, 3999.9999)  >> QCD_HT-2000toInf");

  auto *h_incl = new THStack("QCD_Incl", "QCD_Incl");
  h_incl->Add(h_700to1000);
  h_incl->Add(h_1000to1500);
  h_incl->Add(h_1500to2000);
  h_incl->Add(h_2000toInf);

  TLegend *leg = new TLegend(0.65,0.7,0.87,0.89);
  leg->AddEntry(h_700to1000,   "QCD_HT-700to1000");
  leg->AddEntry(h_1000to1500,  "QCD_HT-1000to1500");
  leg->AddEntry(h_1500to2000, "QCD_HT-1500to2000");
  leg->AddEntry(h_2000toInf,  "QCD_HT-2000toInf");

  TCanvas *c = new TCanvas("c","c", 3600, 2400);
  c->Divide(2,2);
  c->cd(1);
  h_700to1000->SetMaximum(10000000.);
  h_700to1000->SetMinimum(1.);
  h_700to1000->SetLineWidth(2);
  h_700to1000->Draw("hist");
  gPad->SetLogy();
  c->cd(2);
  h_1000to1500->SetMaximum(10000000.);
  h_1000to1500->SetMinimum(1.);
  h_1000to1500->SetLineWidth(2);
  h_1000to1500->Draw("hist");
  gPad->SetLogy();
  c->cd(3);
  h_1500to2000->SetMaximum(10000000.);
  h_1500to2000->SetMinimum(1.);
  h_1500to2000->SetLineWidth(2);
  h_1500to2000->Draw("hist");
  gPad->SetLogy();
  c->cd(4);
  h_2000toInf->SetMaximum(10000000.);
  h_2000toInf->SetMinimum(1.);
  h_2000toInf->SetLineWidth(2);
  h_2000toInf->Draw("hist");
  gPad->SetLogy();
  
  c->Print("TTJets_lheht_issue/QCD_lheht_issue_"+year+".png");

  TCanvas *c_incl = new TCanvas("c_incl", "c_incl", 2400, 1200);
  c_incl->cd();
  h_700to1000->SetFillColor(kRed-7);
  h_1000to1500->SetFillColor(kBlue-7);
  h_1500to2000->SetFillColor(kGreen-7);
  h_2000toInf->SetFillColor(kGray+1);
  h_incl->SetMaximum(10000000.);
  h_incl->SetMinimum(1.);
  h_incl->Draw("hist");
  leg->SetTextSize(0.025);
  leg->Draw();
  gPad->SetLogy();

  c_incl->Print("TTJets_lheht_issue/QCD_lheht_issue_incl_"+year+".png");



}


void valid_WJets_lheht(TString year)
{
  TH1::SetDefaultSumw2();

  TChain *ch_600to800 = new TChain("tree");
  TChain *ch_800to1200 = new TChain("tree");
  TChain *ch_1200to2500 = new TChain("tree");
  TChain *ch_2500toInf = new TChain("tree");

  if(year=="UL2016_preVFP") {
    ch_600to800->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11_ext1-v2/2520000/*.root");
    ch_800to1200->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11_ext1-v2/2520000/*.root");
    ch_1200to2500->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11_ext1-v2/2520000/*.root");
    ch_2500toInf->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11_ext1-v2/40000/*.root");
  }
  else if(year=="UL2016") {
    ch_600to800->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v1/70000/*.root");
    ch_800to1200->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v1/70000/*.root");
    ch_1200to2500->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v1/260000/*.root");
    ch_2500toInf->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v2/280000/*.root");
  }
  else if(year=="UL2017") {
    ch_600to800->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/280000/*.root");
    ch_800to1200->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v3/40000/*.root");
    ch_1200to2500->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/70000/*.root");
    ch_2500toInf->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v2/40000/*.root");
  }
  else if(year=="UL2018") {
    ch_600to800->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/70000/*.root");
    ch_800to1200->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/70000/*.root");
    ch_1200to2500->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/2540000/*.root");
    ch_2500toInf->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v2/80000/*.root");
  }

  TH1D *h_600to800 = new TH1D("WJetsToLNu_HT-600to800", "WJetsToLNu_HT-600to800", 100, 0, 4000);
  TH1D *h_800to1200 = new TH1D("WJetsToLNu_HT-800to1200", "WJetsToLNu_HT-800to1200", 100, 0, 4000);
  TH1D *h_1200to2500 = new TH1D("WJetsToLNu_HT-1200to2500", "WJetsToLNu_HT-1200to2500", 100, 0, 4000);
  TH1D *h_2500toInf = new TH1D("WJetsToLNu_HT-2500toInf", "WJetsToLNu_HT-2500toInf", 100, 0, 4000);
  ch_600to800  ->Draw("min(lhe_ht, 3999.9999)   >> WJetsToLNu_HT-600to800");
  ch_800to1200 ->Draw("min(lhe_ht, 3999.9999)  >> WJetsToLNu_HT-800to1200");
  ch_1200to2500->Draw("min(lhe_ht, 3999.9999) >> WJetsToLNu_HT-1200to2500");
  ch_2500toInf ->Draw("min(lhe_ht, 3999.9999)  >> WJetsToLNu_HT-2500toInf");

  auto *h_incl = new THStack("WJetsToLNu_Incl", "WJetsToLNu_Incl");
  h_incl->Add(h_600to800);
  h_incl->Add(h_800to1200);
  h_incl->Add(h_1200to2500);
  h_incl->Add(h_2500toInf);

  TLegend *leg = new TLegend(0.65,0.7,0.87,0.89);
  leg->AddEntry(h_600to800,   "WJetsToLNu_HT-600to800");
  leg->AddEntry(h_800to1200,  "WJetsToLNu_HT-800to1200");
  leg->AddEntry(h_1200to2500, "WJetsToLNu_HT-1200to2500");
  leg->AddEntry(h_2500toInf,  "WJetsToLNu_HT-2500toInf");

  TCanvas *c = new TCanvas("c","c", 3600, 2400);
  c->Divide(2,2);
  c->cd(1);
  h_600to800->SetMaximum(10000000.);
  h_600to800->SetMinimum(1.);
  h_600to800->SetLineWidth(2);
  h_600to800->Draw("hist");
  gPad->SetLogy();
  c->cd(2);
  h_800to1200->SetMaximum(10000000.);
  h_800to1200->SetMinimum(1.);
  h_800to1200->SetLineWidth(2);
  h_800to1200->Draw("hist");
  gPad->SetLogy();
  c->cd(3);
  h_1200to2500->SetMaximum(10000000.);
  h_1200to2500->SetMinimum(1.);
  h_1200to2500->SetLineWidth(2);
  h_1200to2500->Draw("hist");
  gPad->SetLogy();
  c->cd(4);
  h_2500toInf->SetMaximum(10000000.);
  h_2500toInf->SetMinimum(1.);
  h_2500toInf->SetLineWidth(2);
  h_2500toInf->Draw("hist");
  gPad->SetLogy();
  
  c->Print("TTJets_lheht_issue/WJetsToLNu_lheht_issue_"+year+".png");

  TCanvas *c_incl = new TCanvas("c_incl", "c_incl", 2400, 1200);
  c_incl->cd();
  h_600to800->SetFillColor(kRed-7);
  h_800to1200->SetFillColor(kBlue-7);
  h_1200to2500->SetFillColor(kGreen-7);
  h_2500toInf->SetFillColor(kGray+1);
  h_incl->SetMaximum(10000000.);
  h_incl->SetMinimum(1.);
  h_incl->Draw("hist");
  leg->SetTextSize(0.025);
  leg->Draw();
  gPad->SetLogy();

  c_incl->Print("TTJets_lheht_issue/WJetsToLNu_lheht_issue_incl_"+year+".png");



}


void valid_TTJets_lheht(TString year)
{
  TH1::SetDefaultSumw2();

  TChain *ch_600to800 = new TChain("tree");
  TChain *ch_800to1200 = new TChain("tree");
  TChain *ch_1200to2500 = new TChain("tree");
  TChain *ch_2500toInf = new TChain("tree");
  TChain *ch_inclusive = new TChain("tree");

  if(year=="UL2016_preVFP") {
    //ch_600to800->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/TTJets_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11-v1/50000/*.root");
    //ch_800to1200->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/TTJets_HT-800to1200_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11-v1/2520000/*.root");
    //ch_1200to2500->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/TTJets_HT-1200to2500_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11-v1/50000/*.root");
    //ch_2500toInf->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP/TTJets_HT-2500toInf_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_preVFP_v11-v1/2520000/*.root");
    ch_600to800->Add("/data3/nanoprocessing/recalculate_lheht/UL2016_preVFP/TTJets_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
    ch_800to1200->Add("/data3/nanoprocessing/recalculate_lheht/UL2016_preVFP/TTJets_HT-800to1200_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
    ch_1200to2500->Add("/data3/nanoprocessing/recalculate_lheht/UL2016_preVFP/TTJets_HT-1200to2500_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
    ch_2500toInf->Add("/data3/nanoprocessing/recalculate_lheht/UL2016_preVFP/TTJets_HT-2500toInf_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
    ch_inclusive->Add("/data3/nanoprocessing/recalculate_lheht/UL2016_preVFP/TTJets_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
  }
  else if(year=="UL2016") {
    //ch_600to800->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/TTJets_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v1/70000/*.root");
    //ch_800to1200->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/TTJets_HT-800to1200_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v1/50000/*.root");
    //ch_1200to2500->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/TTJets_HT-1200to2500_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v1/2530000/*.root");
    //ch_2500toInf->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2016/TTJets_HT-2500toInf_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v1/50000/*.root");
    ch_600to800->Add("/data3/nanoprocessing/recalculate_lheht/UL2016/TTJets_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
    ch_800to1200->Add("/data3/nanoprocessing/recalculate_lheht/UL2016/TTJets_HT-800to1200_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
    ch_1200to2500->Add("/data3/nanoprocessing/recalculate_lheht/UL2016/TTJets_HT-1200to2500_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
    ch_2500toInf->Add("/data3/nanoprocessing/recalculate_lheht/UL2016/TTJets_HT-2500toInf_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
    ch_inclusive->Add("/data3/nanoprocessing/recalculate_lheht/UL2016/TTJets_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
  }
  else if(year=="UL2017") {
    //ch_600to800->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/TTJets_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/2530000/*.root");
    //ch_800to1200->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/TTJets_HT-800to1200_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/50000/*.root");
    //ch_1200to2500->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/TTJets_HT-1200to2500_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/50000/*.root");
    //ch_2500toInf->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2017/TTJets_HT-2500toInf_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/50000/*.root");
    ch_600to800->Add("/data3/nanoprocessing/recalculate_lheht/UL2017/TTJets_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
    ch_800to1200->Add("/data3/nanoprocessing/recalculate_lheht/UL2017/TTJets_HT-800to1200_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
    ch_1200to2500->Add("/data3/nanoprocessing/recalculate_lheht/UL2017/TTJets_HT-1200to2500_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
    ch_2500toInf->Add("/data3/nanoprocessing/recalculate_lheht/UL2017/TTJets_HT-2500toInf_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
    ch_inclusive->Add("/data3/nanoprocessing/recalculate_lheht/UL2017/TTJets_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
  }
  else if(year=="UL2018") {
    //ch_600to800->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/TTJets_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/40000/*.root");
    //ch_800to1200->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/TTJets_HT-800to1200_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/50000/*.root");
    //ch_1200to2500->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/TTJets_HT-1200to2500_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/50000/*.root");
    //ch_2500toInf->Add("/data3/nanoprocessing/processed_230731_elecutBased/UL2018/TTJets_HT-2500toInf_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/50000/*.root");
    ch_600to800->Add("/data3/nanoprocessing/recalculate_lheht/UL2018/TTJets_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
    ch_800to1200->Add("/data3/nanoprocessing/recalculate_lheht/UL2018/TTJets_HT-800to1200_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
    ch_1200to2500->Add("/data3/nanoprocessing/recalculate_lheht/UL2018/TTJets_HT-1200to2500_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
    ch_2500toInf->Add("/data3/nanoprocessing/recalculate_lheht/UL2018/TTJets_HT-2500toInf_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
    ch_inclusive->Add("/data3/nanoprocessing/recalculate_lheht/UL2018/TTJets_TuneCP5_13TeV-madgraphMLM-pythia8/*.root");
  }

  // Define histograms for lhe_ht
  TH1D *h_lhe_ht_600to800 = new TH1D("lhe_ht_TTJets_HT-600to800", "lhe_ht_TTJets_HT-600to800", 100, 0, 4000);
  TH1D *h_lhe_ht_800to1200 = new TH1D("lhe_ht_TTJets_HT-800to1200", "lhe_ht_TTJets_HT-800to1200", 100, 0, 4000);
  TH1D *h_lhe_ht_1200to2500 = new TH1D("lhe_ht_TTJets_HT-1200to2500", "lhe_ht_TTJets_HT-1200to2500", 100, 0, 4000);
  TH1D *h_lhe_ht_2500toInf = new TH1D("lhe_ht_TTJets_HT-2500toInf", "lhe_ht_TTJets_HT-2500toInf", 100, 0, 4000);
  TH1D *h_lhe_ht_inclusive = new TH1D("lhe_ht_TTJets_TuneCP5", "lhe_ht_TTJets_TuneCP5", 100, 0, 4000);
  TH1D *h_lhe_ht_inclusive_0to600 = new TH1D("lhe_ht_TTJets_TuneCP5_0to600", "lhe_ht_TTJets_TuneCP5_0to600", 100, 0, 4000);
  // Draw lhe_ht
  ch_600to800  ->Draw("min(lhe_ht, 3999.9999) >> lhe_ht_TTJets_HT-600to800",   "w_lumi");
  ch_800to1200 ->Draw("min(lhe_ht, 3999.9999) >> lhe_ht_TTJets_HT-800to1200",  "w_lumi");
  ch_1200to2500->Draw("min(lhe_ht, 3999.9999) >> lhe_ht_TTJets_HT-1200to2500", "w_lumi");
  ch_2500toInf ->Draw("min(lhe_ht, 3999.9999) >> lhe_ht_TTJets_HT-2500toInf",  "w_lumi");
  ch_inclusive ->Draw("min(lhe_ht, 3999.9999) >> lhe_ht_TTJets_TuneCP5",       "w_lumi");
  ch_inclusive ->Draw("min(lhe_ht, 3999.9999) >> lhe_ht_TTJets_TuneCP5_0to600",       "w_lumi*(lhe_ht<600)");

  // calculate fraction out of range
  float f_lhe_ht_600to800, f_lhe_ht_800to1200, f_lhe_ht_1200to2500, f_lhe_ht_2500toInf, f_lhe_ht_inclusive;
  f_lhe_ht_600to800 = static_cast<float>(ch_600to800    ->GetEntries("lhe_ht<600 || lhe_ht>800")) / static_cast<float>(ch_600to800->GetEntries());
  f_lhe_ht_800to1200 = static_cast<float>(ch_800to1200  ->GetEntries("lhe_ht<800 || lhe_ht>1200")) / static_cast<float>(ch_800to1200->GetEntries());
  f_lhe_ht_1200to2500 = static_cast<float>(ch_1200to2500->GetEntries("lhe_ht<1200 || lhe_ht>2500")) / static_cast<float>(ch_1200to2500->GetEntries());
  f_lhe_ht_2500toInf = static_cast<float>(ch_2500toInf  ->GetEntries("lhe_ht<2500")) / static_cast<float>(ch_2500toInf->GetEntries());
  cout << "f_lhe_ht_600to800: " << f_lhe_ht_600to800 << endl;
  cout << "f_lhe_ht_800to1200: " << f_lhe_ht_800to1200 << endl;
  cout << "f_lhe_ht_1200to2500: " << f_lhe_ht_1200to2500 << endl;
  cout << "f_lhe_ht_2500toInf: " << f_lhe_ht_2500toInf << endl;

  // fill histogram
  auto *h_lhe_ht_stack = new THStack("lhe_ht_TTJets_stack", "lhe_ht_TTJets_stack");
  h_lhe_ht_stack->Add(h_lhe_ht_600to800);
  h_lhe_ht_stack->Add(h_lhe_ht_800to1200);
  h_lhe_ht_stack->Add(h_lhe_ht_1200to2500);
  h_lhe_ht_stack->Add(h_lhe_ht_2500toInf);
  h_lhe_ht_stack->Add(h_lhe_ht_inclusive_0to600);

  TLegend *leg_lhe_ht = new TLegend(0.68,0.5,0.88,0.73);
  leg_lhe_ht->AddEntry(h_lhe_ht_600to800,   	   "TTJets_HT-600to800");
  leg_lhe_ht->AddEntry(h_lhe_ht_800to1200,  	   "TTJets_HT-800to1200");
  leg_lhe_ht->AddEntry(h_lhe_ht_1200to2500, 	   "TTJets_HT-1200to2500");
  leg_lhe_ht->AddEntry(h_lhe_ht_2500toInf,  	   "TTJets_HT-2500toInf");
  leg_lhe_ht->AddEntry(h_lhe_ht_inclusive_0to600,  "TTJets_TuneCP5_0to600");
  leg_lhe_ht->AddEntry(h_lhe_ht_inclusive,  	   "TTJets_TuneCP5");

  TCanvas *c_lhe_ht = new TCanvas("c_lhe_ht","c_lhe_ht", 3600, 2400);
  c_lhe_ht->Divide(2,2);
  c_lhe_ht->cd(1);
  h_lhe_ht_600to800->SetMaximum(2000.);
  h_lhe_ht_600to800->SetMinimum(0.1);
  h_lhe_ht_600to800->SetLineWidth(2);
  h_lhe_ht_600to800->Draw("hist");
  gPad->SetLogy();
  c_lhe_ht->cd(2);
  h_lhe_ht_800to1200->SetMaximum(2000.);
  h_lhe_ht_800to1200->SetMinimum(0.1);
  h_lhe_ht_800to1200->SetLineWidth(2);
  h_lhe_ht_800to1200->Draw("hist");
  gPad->SetLogy();
  c_lhe_ht->cd(3);
  h_lhe_ht_1200to2500->SetMaximum(200.);
  h_lhe_ht_1200to2500->SetMinimum(0.1);
  h_lhe_ht_1200to2500->SetLineWidth(2);
  h_lhe_ht_1200to2500->Draw("hist");
  gPad->SetLogy();
  c_lhe_ht->cd(4);
  h_lhe_ht_2500toInf->SetMaximum(2.);
  h_lhe_ht_2500toInf->SetMinimum(0.01);
  h_lhe_ht_2500toInf->SetLineWidth(2);
  h_lhe_ht_2500toInf->Draw("hist");
  gPad->SetLogy();
  
  c_lhe_ht->Print("TTJets_lheht_issue/lhe_ht_TTJets_individual_"+year+".png");


  TCanvas* c_lhe_ht_incl = new TCanvas("c_lhe_ht_incl", "c_lhe_ht_incl", 1800, 1200);
  c_lhe_ht_incl->cd();
  h_lhe_ht_inclusive->SetMaximum(1000000.);
  h_lhe_ht_inclusive->SetMinimum(0.1);
  h_lhe_ht_inclusive->SetLineWidth(2);
  h_lhe_ht_inclusive->Draw("hist");
  gPad->SetLogy();

  c_lhe_ht_incl->Print("TTJets_lheht_issue/lhe_ht_TTJets_TuneCP5_"+year+".png");


  TCanvas *c_lhe_ht_stack = new TCanvas("c_lhe_ht_stack", "c_lhe_ht_stack", 2400, 1200);
  c_lhe_ht_stack->cd();
  h_lhe_ht_600to800->SetFillColor(kRed-7);
  h_lhe_ht_800to1200->SetFillColor(kBlue-7);
  h_lhe_ht_1200to2500->SetFillColor(kGreen-7);
  h_lhe_ht_2500toInf->SetFillColor(kGray+1);
  h_lhe_ht_inclusive_0to600->SetFillColor(kCyan+2);
  h_lhe_ht_stack->SetMaximum(1000000.);
  h_lhe_ht_stack->SetMinimum(0.1);
  h_lhe_ht_stack->Draw("hist");
  h_lhe_ht_inclusive->Draw("same e");
  leg_lhe_ht->SetTextSize(0.025);
  leg_lhe_ht->Draw();
  gPad->SetLogy();

  c_lhe_ht_stack->Print("TTJets_lheht_issue/lhe_ht_TTJets_stack_"+year+".png");

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
  //valid_WJets_lheht(year);
  //valid_QCD_lheht(year);
  //valid_DY_lheht(year);


  return 0;

}
