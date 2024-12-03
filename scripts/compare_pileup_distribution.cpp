#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "TROOT.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TLegend.h"
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

int main(int argc, char **argv)
{
  TString year, sample, syst;
  year		= argv[1];

  TFile *f1_down, *f2_down, *f1_nominal, *f2_nominal, *f1_up, *f2_up;

  if(year=="UL2016_preVFP") {
    f1_down = new TFile("/home/bjhong/nanoprocessing/pileup/UL2016_preVFP/PileupHistogram-goldenJSON-13tev-2016-preVFP-66000ub-99bins.root","READ");
    f2_down = new TFile("/home/bjhong/nanoprocessing/pileup/UL2016_preVFP/pileup_UL2016_preVFP_down.root","READ");
    f1_nominal = new TFile("/home/bjhong/nanoprocessing/pileup/UL2016_preVFP/PileupHistogram-goldenJSON-13tev-2016-preVFP-69200ub-99bins.root","READ");
    f2_nominal = new TFile("/home/bjhong/nanoprocessing/pileup/UL2016_preVFP/pileup_UL2016_preVFP_nominal.root","READ");
    f1_up = new TFile("/home/bjhong/nanoprocessing/pileup/UL2016_preVFP/PileupHistogram-goldenJSON-13tev-2016-preVFP-72400ub-99bins.root","READ");
    f2_up = new TFile("/home/bjhong/nanoprocessing/pileup/UL2016_preVFP/pileup_UL2016_preVFP_up.root","READ");
  }
  else if(year=="UL2016") {
    f1_down = new TFile("/home/bjhong/nanoprocessing/pileup/UL2016/PileupHistogram-goldenJSON-13tev-2016-postVFP-66000ub-99bins.root","READ");
    f2_down = new TFile("/home/bjhong/nanoprocessing/pileup/UL2016/pileup_UL2016_down.root","READ");
    f1_nominal = new TFile("/home/bjhong/nanoprocessing/pileup/UL2016/PileupHistogram-goldenJSON-13tev-2016-postVFP-69200ub-99bins.root","READ");
    f2_nominal = new TFile("/home/bjhong/nanoprocessing/pileup/UL2016/pileup_UL2016_nominal.root","READ");
    f1_up = new TFile("/home/bjhong/nanoprocessing/pileup/UL2016/PileupHistogram-goldenJSON-13tev-2016-postVFP-72400ub-99bins.root","READ");
    f2_up = new TFile("/home/bjhong/nanoprocessing/pileup/UL2016/pileup_UL2016_up.root","READ");
  }
  else if(year=="UL2017") {
    f1_down = new TFile("/home/bjhong/nanoprocessing/pileup/UL2017/PileupHistogram-goldenJSON-13tev-2017-66000ub-99bins.root","READ");
    f2_down = new TFile("/home/bjhong/nanoprocessing/pileup/UL2017/pileup_UL2017_down.root","READ");
    f1_nominal = new TFile("/home/bjhong/nanoprocessing/pileup/UL2017/PileupHistogram-goldenJSON-13tev-2017-69200ub-99bins.root","READ");
    f2_nominal = new TFile("/home/bjhong/nanoprocessing/pileup/UL2017/pileup_UL2017_nominal.root","READ");
    f1_up = new TFile("/home/bjhong/nanoprocessing/pileup/UL2017/PileupHistogram-goldenJSON-13tev-2017-72400ub-99bins.root","READ");
    f2_up = new TFile("/home/bjhong/nanoprocessing/pileup/UL2017/pileup_UL2017_up.root","READ");
  }
  else if(year=="UL2018") {
    f1_down = new TFile("/home/bjhong/nanoprocessing/pileup/UL2018/PileupHistogram-goldenJSON-13tev-2018-66000ub-99bins.root","READ");
    f2_down = new TFile("/home/bjhong/nanoprocessing/pileup/UL2018/pileup_UL2018_down.root","READ");
    f1_nominal = new TFile("/home/bjhong/nanoprocessing/pileup/UL2018/PileupHistogram-goldenJSON-13tev-2018-69200ub-99bins.root","READ");
    f2_nominal = new TFile("/home/bjhong/nanoprocessing/pileup/UL2018/pileup_UL2018_nominal.root","READ");
    f1_up = new TFile("/home/bjhong/nanoprocessing/pileup/UL2018/PileupHistogram-goldenJSON-13tev-2018-72400ub-99bins.root","READ");
    f2_up = new TFile("/home/bjhong/nanoprocessing/pileup/UL2018/pileup_UL2018_up.root","READ");
  }

  TH1D *pileup1_down, *pileup2_down, *pileup1_nominal, *pileup2_nominal, *pileup1_up, *pileup2_up;

  pileup1_down = (TH1D*)f1_down->Get("pileup");
  pileup2_down = (TH1D*)f2_down->Get("pileup");
  pileup1_nominal = (TH1D*)f1_nominal->Get("pileup");
  pileup2_nominal = (TH1D*)f2_nominal->Get("pileup");
  pileup1_up = (TH1D*)f1_up->Get("pileup");
  pileup2_up = (TH1D*)f2_up->Get("pileup");

  TH1D *p1_down = (TH1D*)pileup1_down->Clone();
  TH1D *p2_down = (TH1D*)pileup2_down->Clone();
  TH1D *p1_nominal = (TH1D*)pileup1_nominal->Clone();
  TH1D *p2_nominal = (TH1D*)pileup2_nominal->Clone();
  TH1D *p1_up = (TH1D*)pileup1_up->Clone();
  TH1D *p2_up = (TH1D*)pileup2_up->Clone();

  pileup1_down->SetLineColor(kRed);
  pileup2_down->SetLineColor(kBlack);
  pileup1_nominal->SetLineColor(kRed);
  pileup2_nominal->SetLineColor(kBlack);
  pileup1_up->SetLineColor(kRed);
  pileup2_up->SetLineColor(kBlack);

  TLegend *l1 = new TLegend(0.65,0.65,0.85,0.75);
  l1->AddEntry(pileup1_down, "from /afs/");
  l1->AddEntry(pileup2_down, "from pileupCalc.py");
  TLegend *l2 = new TLegend(0.65,0.65,0.85,0.75);
  l2->AddEntry(pileup1_down, "down from /afs/");
  l2->AddEntry(pileup1_nominal, "nominal from /afs/");
  l2->AddEntry(pileup1_up, "up from /afs/");
  TLegend *l3 = new TLegend(0.65,0.65,0.85,0.75);
  l3->AddEntry(pileup2_down, "down from pileupCalc.py");
  l3->AddEntry(pileup2_nominal, "nominal from pileupCalc.py");
  l3->AddEntry(pileup2_up, "up from pileupCalc.py");

  TCanvas *c1 = new TCanvas("c1","c1",1200,600);
  TCanvas *c2 = new TCanvas("c2","c2",1200,600);
  TCanvas *c3 = new TCanvas("c3","c3",1200,600);
  TCanvas *c4 = new TCanvas("c4","c4",1200,600);
  TCanvas *c5 = new TCanvas("c5","c5",1200,600);
  c1->cd();
  pileup1_down->Draw();
  pileup2_down->Draw("same");
  l1->Draw();
  c2->cd();
  pileup1_nominal->Draw();
  pileup2_nominal->Draw("same");
  l1->Draw();
  c3->cd();
  pileup1_up->Draw();
  pileup2_up->Draw("same");
  l1->Draw();
  c4->cd();
  p1_down->SetLineColor(kBlue);
  p1_nominal->SetLineColor(kBlack);
  p1_up->SetLineColor(kRed);
  p1_down->Draw();
  p1_nominal->Draw("same");
  p1_up->Draw("same");
  l2->Draw();
  c5->cd();
  p2_down->SetLineColor(kBlue);
  p2_nominal->SetLineColor(kBlack);
  p2_up->SetLineColor(kRed);
  p2_down->Draw();
  p2_nominal->Draw("same");
  p2_up->Draw("same");
  l3->Draw();
  
  c1->Print("pileup_"+year+"_down.pdf");
  c2->Print("pileup_"+year+"_nominal.pdf");
  c3->Print("pileup_"+year+"_up.pdf");
  c4->Print("pileup_"+year+"_afs.pdf");
  c5->Print("pileup_"+year+"_pileupCalc.pdf");

  f1_down->Close();
  f2_down->Close();
  f1_nominal->Close();
  f2_nominal->Close();
  f1_up->Close();
  f2_up->Close();
  










}
