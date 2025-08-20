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

void valid_v7v9_processed(TString year, TString process)
{
//  gStyle->SetLineScalePS(1);
  TH1::SetDefaultSumw2();
  // before running this codes, you should make flist/[year]/flist_[sample].txt file by running 'make_flist.py'
  
  TString year_v7;
  if(year=="UL2016_preVFP") year_v7 = "2016";
  else if(year=="UL2016")   year_v7 = "2016";
  else if(year=="UL2017")   year_v7 = "2017";
  else if(year=="UL2018")   year_v7 = "2018";

  // TChain for v7 and v7
  int idx = process.Index("_TuneCP5");
  TString process_v7 = process.Copy();
  process_v7 = process_v7.Remove(idx);
  cout << "process: " << process << endl;

  TChain *ch_v7 = new TChain("tree");
  TChain *ch_v9 = new TChain("tree");
  if(process=="ST_s-channel") {
    cout << "process name of v7 and v9 are different" << endl;
    if(year_v7=="2016") ch_v7->Add("/data2/nanoprocessing/"+year_v7+"/processed_0317/*ST_s-channel_4f_InclusiveDecays*");
    else if(year_v7=="2017"||year_v7=="2018") ch_v7->Add("/data2/nanoprocessing/"+year_v7+"/processed_0317/*ST_s-channel_4f_leptonDecays*");

    vector<TString> files1, files2;
    files1 = getFileListFromFile(Form("flist/processed/%s/flist_ST_s-channel_4f_hadronicDecays_TuneCP5_13TeV-amcatnlo-pythia8.txt", year.Data()));
    files2 = getFileListFromFile(Form("flist/processed/%s/flist_ST_s-channel_4f_leptonDecays_TuneCP5_13TeV-amcatnlo-pythia8.txt", year.Data()));
    for(int i=0; i<files1.size(); i++) {
      ch_v9->Add(files1.at(i));
    }
    for(int i=0; i<files2.size(); i++) {
      ch_v9->Add(files2.at(i));
    }
  }
  else if(process=="TTJets_TuneCP5_13TeV-madgraphMLM-pythia8") {
    cout << "process name of v7 and v9 are different" << endl;
    ch_v7->Add("/data2/nanoprocessing/"+year_v7+"/processed_0317/*TTJets_Tune*");

    vector<TString> files;
    files = getFileListFromFile(Form("flist/processed/%s/flist_%s.txt", year.Data(), process.Data()));
    for(int i=0; i<files.size(); i++) {
      ch_v9->Add(files.at(i));
    }
  }
  else if(process=="ttHJetTobb_M125_TuneCP5_13TeV_amcatnloFXFX_madspin_pythia8") {
    cout << "process name of v7 and v9 are different" << endl;
    ch_v7->Add("/data3/bjhong/process_230322/after_btag/"+year_v7+"/*.root");

    vector<TString> files;
    files = getFileListFromFile(Form("flist/processed/%s/flist_%s.txt", year.Data(), process.Data()));
    for(int i=0; i<files.size(); i++) {
      ch_v9->Add(files.at(i));
    }
  }
  else if(process=="WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8") {
    cout << "process name of v7 and v9 are different" << endl;
    ch_v7->Add("/data2/nanoprocessing/"+year_v7+"/processed_0317/*WWZ_Tune*");
    if(year_v7=="2017") ch_v7->Add("/data2/nanoprocessing/"+year_v7+"/processed_0317/*WWZ_4F_Tune*");

    vector<TString> files;
    files = getFileListFromFile(Form("flist/processed/%s/flist_%s.txt", year.Data(), process.Data()));
    for(int i=0; i<files.size(); i++) {
      ch_v9->Add(files.at(i));
    }
  }
  else if(process=="ST_t-channel_top_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin-pythia8") {
    cout << "process name of v7 and v9 are different" << endl;
    ch_v7->Add("/data2/nanoprocessing/"+year_v7+"/processed_0317/*ST_t-channel_top_4f_inclusiveDecays*");
    if(year_v7=="2018") ch_v7->Add("/data2/nanoprocessing/"+year_v7+"/processed_0317/*ST_t-channel_top_4f_InclusiveDecays*");

    vector<TString> files;
    files = getFileListFromFile(Form("flist/processed/%s/flist_%s.txt", year.Data(), process.Data()));
    for(int i=0; i<files.size(); i++) {
      ch_v9->Add(files.at(i));
    }
  }
  else if(process=="ST_t-channel_antitop_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin-pythia8") {
    cout << "process name of v7 and v9 are different" << endl;
    ch_v7->Add("/data2/nanoprocessing/"+year_v7+"/processed_0317/*ST_t-channel_antitop_4f_inclusiveDecays*");
    if(year_v7=="2018") ch_v7->Add("/data2/nanoprocessing/"+year_v7+"/processed_0317/*ST_t-channel_antitop_4f_InclusiveDecays*");

    vector<TString> files;
    files = getFileListFromFile(Form("flist/processed/%s/flist_%s.txt", year.Data(), process.Data()));
    for(int i=0; i<files.size(); i++) {
      ch_v9->Add(files.at(i));
    }
  }
  else {
    ch_v7->Add("/data2/nanoprocessing/"+year_v7+"/processed_0317/*_"+process_v7+"*");

    vector<TString> files;
    files = getFileListFromFile(Form("flist/processed/%s/flist_%s.txt", year.Data(), process.Data()));
    for(int i=0; i<files.size(); i++) {
      ch_v9->Add(files.at(i));
    }
  }

  cout << "[v7] GetEntries: " << ch_v7->GetEntries() << endl;
  cout << "[v9] GetEntries: " << ch_v9->GetEntries() << endl;

  // define histogram
  TH1D *nleps_v7 = new TH1D("nleps_v7"	, "nleps;nleps;Events"	, 5, 0, 5);
  TH1D *njets_v7 = new TH1D("njets_v7"	, "njets;njets;Events"	, 11, 0, 11);
  TH1D *nbm_v7 	 = new TH1D("nbm_v7"	, "nbm;nbm;Events"	, 5, 0, 5);
  TH1D *ht_v7    = new TH1D("ht_v7"	, "ht;ht;Events"	, 40, 0, 4000);
  TH1D *mj12_v7  = new TH1D("mj12_v7"	, "mj12;mj12;Events"	, 3, 500, 1400);

  TH1D *nleps_v9 = new TH1D("nleps_v9"	, "nleps_v9"	, 5, 0, 5);
  TH1D *njets_v9 = new TH1D("njets_v9"	, "njets_v9"	, 11, 0, 11);
  TH1D *nbm_v9 	 = new TH1D("nbm_v9"	, "nbm_v9"	, 5, 0, 5);
  TH1D *ht_v9    = new TH1D("ht_v9"	, "ht_v9"	, 40, 0, 4000);
  TH1D *mj12_v9  = new TH1D("mj12_v9"	, "mj12_v9"	, 3, 500, 1400);
 
  // draw histogram
  ch_v7->Draw("min(nleps, 4.999)>>nleps_v7"   , "pass", "");
  ch_v7->Draw("min(njets, 10.999)>>njets_v7"  , "pass", "");
  ch_v7->Draw("min(nbm, 4.999)>>nbm_v7"       , "pass", "");
  ch_v7->Draw("min(ht, 3999.999)>>ht_v7"      , "pass", "");
  ch_v7->Draw("min(mj12, 1399.999)>>mj12_v7"  , "pass", "");

  ch_v9->Draw("min(nleps, 4.999)>>nleps_v9"   , "pass", "");
  ch_v9->Draw("min(njets, 10.999)>>njets_v9"  , "pass", "");
  ch_v9->Draw("min(nbm, 4.999)>>nbm_v9"       , "pass", "");
  ch_v9->Draw("min(ht, 3999.999)>>ht_v9"      , "pass", "");
  ch_v9->Draw("min(mj12, 1399.999)>>mj12_v9"  , "pass", "");

  // make clone
  TH1D *nleps_v7_clone = (TH1D*)nleps_v7->Clone();
  TH1D *njets_v7_clone = (TH1D*)njets_v7->Clone();
  TH1D *nbm_v7_clone   = (TH1D*)nbm_v7->Clone();
  TH1D *ht_v7_clone    = (TH1D*)ht_v7->Clone();
  TH1D *mj12_v7_clone  = (TH1D*)mj12_v7->Clone();

  TH1D *nleps_v9_clone = (TH1D*)nleps_v9->Clone();
  TH1D *njets_v9_clone = (TH1D*)njets_v9->Clone();
  TH1D *nbm_v9_clone   = (TH1D*)nbm_v9->Clone();
  TH1D *ht_v9_clone    = (TH1D*)ht_v9->Clone();
  TH1D *mj12_v9_clone  = (TH1D*)mj12_v9->Clone();

  // set configuration of histogram
    // nleps
  nleps_v7->SetStats(0);
  nleps_v7->SetMinimum(1);
  nleps_v7->SetLineColor(kRed); nleps_v9->SetLineColor(kBlue);
  nleps_v7->GetXaxis()->SetNdivisions(5); nleps_v7->GetXaxis()->SetLabelOffset(999); nleps_v7->GetXaxis()->SetLabelSize(0);
  nleps_v7->GetYaxis()->SetTitleSize(0.05); nleps_v7->GetYaxis()->SetTitleOffset(0.8);
  nleps_v7_clone->SetStats(0);
  nleps_v7_clone->SetTitle("");
  nleps_v7_clone->GetXaxis()->SetNdivisions(5); nleps_v7_clone->GetXaxis()->SetLabelSize(0.1); nleps_v7_clone->GetXaxis()->SetLabelOffset(0.025);
  nleps_v7_clone->GetXaxis()->SetTitleOffset(1.2); nleps_v7_clone->GetXaxis()->SetTitleSize(0.1);
  nleps_v7_clone->GetYaxis()->SetRangeUser(0.1,1.9); nleps_v7_clone->GetYaxis()->SetNdivisions(505); nleps_v7_clone->GetYaxis()->SetLabelSize(0.08);
  nleps_v7_clone->GetYaxis()->SetTitle("v7 / v9"); nleps_v7_clone->GetYaxis()->SetTitleSize(0.1); nleps_v7_clone->GetYaxis()->SetTitleOffset(0.4);
    // njets
  njets_v7->SetStats(0);
  njets_v7->SetMinimum(1);
  njets_v7->SetLineColor(kRed); njets_v9->SetLineColor(kBlue);
  njets_v7->GetXaxis()->SetNdivisions(11); njets_v7->GetXaxis()->SetLabelOffset(999); njets_v7->GetXaxis()->SetLabelSize(0);
  njets_v7->GetYaxis()->SetTitleSize(0.05); njets_v7->GetYaxis()->SetTitleOffset(0.8);
  njets_v7_clone->SetStats(0);
  njets_v7_clone->SetTitle("");
  njets_v7_clone->GetXaxis()->SetNdivisions(11); njets_v7_clone->GetXaxis()->SetLabelSize(0.1); njets_v7_clone->GetXaxis()->SetLabelOffset(0.025);
  njets_v7_clone->GetXaxis()->SetTitleOffset(1.2); njets_v7_clone->GetXaxis()->SetTitleSize(0.1);
  njets_v7_clone->GetYaxis()->SetRangeUser(0.1,1.9); njets_v7_clone->GetYaxis()->SetNdivisions(505); njets_v7_clone->GetYaxis()->SetLabelSize(0.08);
  njets_v7_clone->GetYaxis()->SetTitle("v7 / v9"); njets_v7_clone->GetYaxis()->SetTitleSize(0.1); njets_v7_clone->GetYaxis()->SetTitleOffset(0.4);
    // nbm
  nbm_v7->SetStats(0);
  nbm_v7->SetMinimum(1);
  nbm_v7->SetLineColor(kRed); nbm_v9->SetLineColor(kBlue);
  nbm_v7->GetXaxis()->SetNdivisions(5); nbm_v7->GetXaxis()->SetLabelOffset(999); nbm_v7->GetXaxis()->SetLabelSize(0);
  nbm_v7->GetYaxis()->SetTitleSize(0.05); nbm_v7->GetYaxis()->SetTitleOffset(0.8);
  nbm_v7_clone->SetStats(0);
  nbm_v7_clone->SetTitle("");
  nbm_v7_clone->GetXaxis()->SetNdivisions(5); nbm_v7_clone->GetXaxis()->SetLabelSize(0.1); nbm_v7_clone->GetXaxis()->SetLabelOffset(0.025);
  nbm_v7_clone->GetXaxis()->SetTitleOffset(1.2); nbm_v7_clone->GetXaxis()->SetTitleSize(0.1);
  nbm_v7_clone->GetYaxis()->SetRangeUser(0.1,1.9); nbm_v7_clone->GetYaxis()->SetNdivisions(505); nbm_v7_clone->GetYaxis()->SetLabelSize(0.08);
  nbm_v7_clone->GetYaxis()->SetTitle("v7 / v9"); nbm_v7_clone->GetYaxis()->SetTitleSize(0.1); nbm_v7_clone->GetYaxis()->SetTitleOffset(0.4);
    // ht
  ht_v7->SetStats(0);
  ht_v7->SetMinimum(1);
  ht_v7->SetLineColor(kRed); ht_v9->SetLineColor(kBlue);
  ht_v7->GetXaxis()->SetNdivisions(510); ht_v7->GetXaxis()->SetLabelOffset(999); ht_v7->GetXaxis()->SetLabelSize(0);
  ht_v7->GetYaxis()->SetTitleSize(0.05); ht_v7->GetYaxis()->SetTitleOffset(0.8);
  ht_v7_clone->SetStats(0);
  ht_v7_clone->SetTitle("");
  ht_v7_clone->GetXaxis()->SetNdivisions(510); ht_v7_clone->GetXaxis()->SetLabelSize(0.1); ht_v7_clone->GetXaxis()->SetLabelOffset(0.025);
  ht_v7_clone->GetXaxis()->SetTitleOffset(1.2); ht_v7_clone->GetXaxis()->SetTitleSize(0.1);
  ht_v7_clone->GetYaxis()->SetRangeUser(0.1,1.9); ht_v7_clone->GetYaxis()->SetNdivisions(505); ht_v7_clone->GetYaxis()->SetLabelSize(0.08);
  ht_v7_clone->GetYaxis()->SetTitle("v7 / v9"); ht_v7_clone->GetYaxis()->SetTitleSize(0.1); ht_v7_clone->GetYaxis()->SetTitleOffset(0.4);
    // mj12
  mj12_v7->SetStats(0);
  mj12_v7->SetMinimum(1);
  mj12_v7->SetLineColor(kRed); mj12_v9->SetLineColor(kBlue);
  mj12_v7->GetXaxis()->SetNdivisions(510); mj12_v7->GetXaxis()->SetLabelOffset(999); mj12_v7->GetXaxis()->SetLabelSize(0);
  mj12_v7->GetYaxis()->SetTitleSize(0.05); mj12_v7->GetYaxis()->SetTitleOffset(0.8);
  mj12_v7_clone->SetStats(0);
  mj12_v7_clone->SetTitle("");
  mj12_v7_clone->GetXaxis()->SetNdivisions(510); mj12_v7_clone->GetXaxis()->SetLabelSize(0.1); mj12_v7_clone->GetXaxis()->SetLabelOffset(0.025);
  mj12_v7_clone->GetXaxis()->SetTitleOffset(1.2); mj12_v7_clone->GetXaxis()->SetTitleSize(0.1);
  mj12_v7_clone->GetYaxis()->SetRangeUser(0.1,1.9); mj12_v7_clone->GetYaxis()->SetNdivisions(505); mj12_v7_clone->GetYaxis()->SetLabelSize(0.08);
  mj12_v7_clone->GetYaxis()->SetTitle("v7 / v9"); mj12_v7_clone->GetYaxis()->SetTitleSize(0.1); mj12_v7_clone->GetYaxis()->SetTitleOffset(0.4);


  // define TCanvas and TPad (top panel and bottom panel)
  TCanvas *c = new TCanvas("c", "c", 3600, 2400);
  c->Divide(3,2);
  TPad *tpad, *bpad;
//  TPad *tpad = new TPad("tpad","tpad", 0., 0.3, 1., 1.);
//  tpad->SetBottomMargin(0.04);
//  TPad *bpad = new TPad("bpad","bpad", 0., 0., 1., 0.305);
//  bpad->SetTopMargin(0.);
//  bpad->SetBottomMargin(0.3);
//  bpad->SetFillStyle(4000);
  // define TLine for bottom (ratio) panel
  TLine *line;
  // define TLegend

  // Draw histogram in TCanvas
    // nleps
  c->cd(1);
  tpad = new TPad("tpad","tpad", 0., 0.3, 1., 1.);
  tpad->SetBottomMargin(0.04); tpad->Draw(); tpad->cd();
  gPad->SetLogy();
  nleps_v7->Draw("hist");
  nleps_v9->Scale(1.*nleps_v7_clone->Integral()/nleps_v9_clone->Integral());
  nleps_v9->Draw("same hist");
  TLegend *leg1 = new TLegend(0.15, 0.1, 0.48, 0.25);
  leg1->AddEntry(nleps_v7, Form("MC_v7 [%lld]", ch_v7->GetEntries()));
  leg1->AddEntry(nleps_v9, Form("MC_v9 [%lld]", ch_v9->GetEntries()));
  leg1->SetTextSize(0.04);
  leg1->Draw();
  c->cd(1);
  bpad = new TPad("bpad","bpad", 0., 0., 1., 0.305);
  bpad->SetTopMargin(0.); bpad->SetBottomMargin(0.3); bpad->SetFillStyle(4000);
  bpad->Draw(); bpad->cd();
  nleps_v7_clone->Divide(nleps_v9);
  nleps_v7_clone->SetMarkerStyle(20);
  nleps_v7_clone->Draw("hist ex0 p");
  line = new TLine(0., 1., 5., 1.);
  line->SetLineColor(1); line->SetLineWidth(1); line->SetLineStyle(2);
  line->Draw("same");
    // njets
  c->cd(2);
  tpad = new TPad("tpad","tpad", 0., 0.3, 1., 1.);
  tpad->SetBottomMargin(0.04); tpad->Draw(); tpad->cd();
  gPad->SetLogy();
  njets_v7->Draw("hist");
  njets_v9->Scale(1.*njets_v7_clone->Integral()/njets_v9_clone->Integral());
  njets_v9->Draw("same hist");
 // TLegend *leg2 = new TLegend(0.15, 0.1, 0.48, 0.25);
 // leg2->AddEntry(nleps_v7, Form("MC_v7 [%lld]", ch_v7->GetEntries()));
 // leg2->AddEntry(nleps_v9, Form("MC_v9 [%lld]", ch_v9->GetEntries()));
 // leg2->SetTextSize(0.04);
 // leg2->Draw();
  c->cd(2);
  bpad = new TPad("bpad","bpad", 0., 0., 1., 0.305);
  bpad->SetTopMargin(0.); bpad->SetBottomMargin(0.3); bpad->SetFillStyle(4000);
  bpad->Draw(); bpad->cd();
  njets_v7_clone->Divide(njets_v9);
  njets_v7_clone->SetMarkerStyle(20);
  njets_v7_clone->Draw("hist ex0 p");
  line = new TLine(0., 1., 11., 1.);
  line->SetLineColor(1); line->SetLineWidth(1); line->SetLineStyle(2);
  line->Draw("same");
    // nbm
  c->cd(3);
  tpad = new TPad("tpad","tpad", 0., 0.3, 1., 1.);
  tpad->SetBottomMargin(0.04); tpad->Draw(); tpad->cd();
  gPad->SetLogy();
  nbm_v7->Draw("hist");
  nbm_v9->Scale(1.*nbm_v7_clone->Integral()/nbm_v9_clone->Integral());
  nbm_v9->Draw("same hist");
 // TLegend *leg3 = new TLegend(0.15, 0.1, 0.48, 0.25);
 // leg3->AddEntry(nleps_v7, Form("MC_v7 [%lld]", ch_v7->GetEntries()));
 // leg3->AddEntry(nleps_v9, Form("MC_v9 [%lld]", ch_v9->GetEntries()));
 // leg3->SetTextSize(0.04);
 // leg3->Draw();
  c->cd(3);
  bpad = new TPad("bpad","bpad", 0., 0., 1., 0.305);
  bpad->SetTopMargin(0.); bpad->SetBottomMargin(0.3); bpad->SetFillStyle(4000);
  bpad->Draw(); bpad->cd();
  nbm_v7_clone->Divide(nbm_v9);
  nbm_v7_clone->SetMarkerStyle(20);
  nbm_v7_clone->Draw("hist ex0 p");
  line = new TLine(0., 1., 5., 1.);
  line->SetLineColor(1); line->SetLineWidth(1); line->SetLineStyle(2);
  line->Draw("same");
    // ht
  c->cd(4);
  tpad = new TPad("tpad","tpad", 0., 0.3, 1., 1.);
  tpad->SetBottomMargin(0.04); tpad->Draw(); tpad->cd();
  gPad->SetLogy();
  ht_v7->Draw("hist");
  ht_v9->Scale(1.*ht_v7_clone->Integral()/ht_v9_clone->Integral());
  ht_v9->Draw("same hist");
 // TLegend *leg4 = new TLegend(0.15, 0.1, 0.48, 0.25);
 // leg4->AddEntry(nleps_v7, Form("MC_v7 [%lld]", ch_v7->GetEntries()));
 // leg4->AddEntry(nleps_v9, Form("MC_v9 [%lld]", ch_v9->GetEntries()));
 // leg4->SetTextSize(0.04);
 // leg4->Draw();
  c->cd(4);
  bpad = new TPad("bpad","bpad", 0., 0., 1., 0.305);
  bpad->SetTopMargin(0.); bpad->SetBottomMargin(0.3); bpad->SetFillStyle(4000);
  bpad->Draw(); bpad->cd();
  ht_v7_clone->Divide(ht_v9);
  ht_v7_clone->SetMarkerStyle(20);
  ht_v7_clone->Draw("hist ex0 p");
  line = new TLine(0., 1., 4000., 1.);
  line->SetLineColor(1); line->SetLineWidth(1); line->SetLineStyle(2);
  line->Draw("same");
    // mj12
  c->cd(5);
  tpad = new TPad("tpad","tpad", 0., 0.3, 1., 1.);
  tpad->SetBottomMargin(0.04); tpad->Draw(); tpad->cd();
  gPad->SetLogy();
  mj12_v7->Draw("hist");
  mj12_v9->Scale(1.*mj12_v7_clone->Integral()/mj12_v9_clone->Integral());
  mj12_v9->Draw("same hist");
 // TLegend *leg5 = new TLegend(0.15, 0.1, 0.48, 0.25);
 // leg5->AddEntry(nleps_v7, Form("MC_v7 [%lld]", ch_v7->GetEntries()));
 // leg5->AddEntry(nleps_v9, Form("MC_v9 [%lld]", ch_v9->GetEntries()));
 // leg5->SetTextSize(0.04);
 // leg5->Draw();
  c->cd(5);
  bpad = new TPad("bpad","bpad", 0., 0., 1., 0.305);
  bpad->SetTopMargin(0.); bpad->SetBottomMargin(0.3); bpad->SetFillStyle(4000);
  bpad->Draw(); bpad->cd();
  mj12_v7_clone->Divide(mj12_v9);
  mj12_v7_clone->SetMarkerStyle(20);
  mj12_v7_clone->Draw("hist ex0 p");
  line = new TLine(500., 1., 1400., 1.);
  line->SetLineColor(1); line->SetLineWidth(1); line->SetLineStyle(2);
  line->Draw("same");


  c->Print("valid_v7v9/"+year+"/"+process+"_"+year+".png");
  //c->Print("valid_v7v9/"+year+"/"+process+"_"+year+".pdf");




}

int main(int argc, char **argv)
{
  TString year, process;
  year	  = argv[1];
  process = argv[2]; // process should not include Tune version (due to difference b/w v7 and v9)

  valid_v7v9_processed(year, process);

  return 0;
}
