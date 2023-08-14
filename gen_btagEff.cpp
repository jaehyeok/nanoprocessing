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

using namespace std;





void gen_btagEff(TString process, TString year)
{
  TH1::SetDefaultSumw2();

  bool 		pass=true;
  vector<int> 	*jets_hflavor(0);
  vector<float> *jets_pt(0);
  vector<float> *jets_eta(0);
  vector<float> *jets_dcsvb(0);
  vector<bool> 	*jets_islep(0);
  vector<bool> 	*jets_id(0);


  TChain *ch = new TChain("tree");
  ch->Add("/data3/nanoprocessing/processed_230607/"+year+"/"+process+"/*.root");
  cout << "GetEntries: " << ch->GetEntries() << endl;

  ch->SetBranchAddress("jets_hflavor",	&jets_hflavor);
  ch->SetBranchAddress("jets_pt",	&jets_pt);
  ch->SetBranchAddress("jets_eta",	&jets_eta);
  ch->SetBranchAddress("jets_dcsvb",	&jets_dcsvb);
  ch->SetBranchAddress("jets_islep",	&jets_islep);
  ch->SetBranchAddress("jets_id",	&jets_id);
  ch->SetBranchAddress("pass",		&pass);

  TFile *outputfile = new TFile("Running/btagEfficiency_"+process+"_"+year+".root", "recreate", "btagEfficiency_"+process+"_"+year+".root");      // Name, option, Title
  outputfile->cd();

  map<TString, double> dcsv_med_WP;
  dcsv_med_WP["UL2016_preVFP"] = 0.6001;
  dcsv_med_WP["UL2016"] = 0.5847;
  dcsv_med_WP["UL2017"] = 0.4506;
  dcsv_med_WP["UL2018"] = 0.4168;

  // define cuts
  double eta_cuts_comb[2] 	 = {-2.4, 2.4};
  double eta_cuts_incl[2] 	 = {0., 2.4};
  double pt_cuts_comb[10] 	 = {20., 30., 50., 70., 100., 140., 200., 300., 600., 1000.,};
  double pt_cuts_incl[2] 	 = {20., 1000.};
  double pt_cuts_comb_central[2] = {20., 1000.};
  double flavor_cuts[4] 	 = {-0.5, 3.5, 4.5, 5.5};

  // define numerator and denominator histograms
  TH3D *numerator_comb, *numerator_comb_central, *numerator_incl;
  TH3D *denominator_comb, *denominator_comb_central, *denominator_incl;

  numerator_comb = new TH3D("btagEfficiency_medium_comb", "btagEfficiency_medium_comb", 1, eta_cuts_comb, 9, pt_cuts_comb, 3, flavor_cuts);
  numerator_comb_central = new TH3D("btagEfficiency_medium_comb_central", "btagEfficiency_medium_comb_central", 1, eta_cuts_comb, 1, pt_cuts_comb_central, 3, flavor_cuts);
  numerator_incl = new TH3D("btagEfficiency_medium_incl", "btagEfficiency_medium_incl", 1, eta_cuts_incl, 1, pt_cuts_incl, 3, flavor_cuts);

  denominator_comb = static_cast<TH3D*>(numerator_comb->Clone("btagEfficiency_denominator_comb"));
  denominator_comb_central = static_cast<TH3D*>(numerator_comb_central->Clone("btagEfficiency_denominator_comb_central"));
  denominator_incl = static_cast<TH3D*>(numerator_incl->Clone("btagEfficiency_denominator_incl"));


  int flavor=0;
  double pt=0, eta=0, dcsv=0;;
  double tot=0;

  // event loop
  for(int ievt=0; ievt<ch->GetEntries(); ievt++){

    pass=true;
    jets_hflavor->clear();
    jets_pt->clear();
    jets_eta->clear();
    jets_dcsvb->clear();
    jets_islep->clear();
    jets_id->clear();
    flavor=0, pt=0, eta=0, dcsv=0, tot=0;

    if(ievt%10000==0) cout << ievt << " / " << ch->GetEntries() << endl;

    ch->GetEntry(ievt);
    if(pass!=1) continue;
    for(int ijet=0; ijet<jets_hflavor->size(); ijet++) {

      flavor=0, pt=0, eta=0, dcsv=0;


      flavor = TMath::Abs(jets_hflavor->at(ijet));
      if(flavor==21) flavor=0;
//      pt = min(jets_pt->at(ijet), 999.99);
      if(jets_pt->at(ijet)>999.99) jets_pt->at(ijet)=999.98;
      pt = jets_pt->at(ijet);
      eta = TMath::Abs(jets_eta->at(ijet));
      dcsv = jets_dcsvb->at(ijet);

      if(jets_islep->at(ijet)==1) continue;
      if(pt<30) break;
      if(eta>2.4) continue;
      if(jets_id->at(ijet)!=1) continue;

      denominator_comb->Fill(eta, pt, flavor);
      denominator_comb_central->Fill(eta, pt, flavor);
      denominator_incl->Fill(eta, pt, flavor);
      if(dcsv > dcsv_med_WP[year]) { 
        numerator_comb->Fill(eta, pt, flavor);
        numerator_comb_central->Fill(eta, pt, flavor);
        numerator_incl->Fill(eta, pt, flavor);
      }
    }
  }

  cout << denominator_incl->GetBinContent(1,1,3) << endl;
  cout << denominator_comb_central->GetBinContent(1,1,3) << endl;
  for(int k=0; k<4; k++) {
    for(int i=0; i<2; i++) {
      for(int j=0; j<10; j++) {
        tot = tot + denominator_comb->GetBinContent(i,j,k);
      }
    }
  }
  cout << tot << endl;
  cout << numerator_comb->GetBinContent(1,4,3) << endl;
  cout << denominator_comb->GetBinContent(1,4,3) << endl;
  cout << numerator_incl->GetBinContent(1,1,3) << endl;
  cout << numerator_comb_central->GetBinContent(1,1,3) << endl;

//  TH3D *eff_comb, *eff_comb_central, *eff_incl;
//  eff_comb = static_cast<TH3D*>(numerator_comb->Clone("Efficiency_comb"));
//  eff_comb_central = static_cast<TH3D*>(numerator_comb_central->Clone("Efficiency_comb_central"));
//  eff_incl = static_cast<TH3D*>(numerator_incl->Clone("Efficiency_incl"));
  numerator_comb->Divide(denominator_comb);
  numerator_comb_central->Divide(denominator_comb_central);
  numerator_incl->Divide(denominator_incl);

  numerator_comb->Write();
  numerator_comb_central->Write();
  numerator_incl->Write();

  outputfile->Close();
  cout << "... transferring output file" << endl;
  cout << Form("... cp %s /home/bjhong/nanoprocessing/%s", outputfile->GetName(), outputfile->GetTitle()) << endl;
  gSystem->Exec(Form("cp %s /home/bjhong/nanoprocessing/%s", outputfile->GetName(), outputfile->GetTitle()));
  cout << Form("... rm %s", outputfile->GetName()) << endl;
  gSystem->Exec(Form("rm %s", outputfile->GetName()));
  cout << "finish generating btag eff file" << endl;
}

int main(int argc, char **argv)
{

  TString year, process;
  year    = argv[1];
  process = argv[2];
  gen_btagEff(process, year);

  return 0;








}
