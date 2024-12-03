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
  vector<bool>  *jets_hem(0);


  TChain *ch = new TChain("tree");
  //ch->Add("/data3/nanoprocessing/processed_230915_muonsf/"+year+"/"+process+"/*.root");
  ch->Add("/data3/nanoprocessing/processed_241029_signal/"+year+"/"+process+"/*.root");
  cout << "GetEntries: " << ch->GetEntries() << endl;

  ch->SetBranchAddress("jets_hflavor",	&jets_hflavor);
  ch->SetBranchAddress("jets_pt",	&jets_pt);
  ch->SetBranchAddress("jets_eta",	&jets_eta);
  ch->SetBranchAddress("jets_dcsvb",	&jets_dcsvb);
  ch->SetBranchAddress("jets_islep",	&jets_islep);
  ch->SetBranchAddress("jets_id",	&jets_id);
  ch->SetBranchAddress("pass",		&pass);
  ch->SetBranchAddress("jets_hem",      &jets_hem);

  TFile *outputfile = new TFile("Running/btagEfficiency_"+process+"_"+year+".root", "recreate", "btagEfficiency_"+process+"_"+year+".root");      // Name, option, Title
  outputfile->cd();

  map<TString, double> dcsv_med_WP;
  dcsv_med_WP["UL2016_preVFP"] = 0.6001;             // b-jet eff.: 66.7%
  dcsv_med_WP["UL2016"] = 0.5847;                    // b-jet eff.: 66.9%
  dcsv_med_WP["UL2017"] = 0.4506;                    // b-jet eff.: 71.7%
  dcsv_med_WP["UL2018"] = 0.4168;                    // b-jet eff.: 72.9%

  // define cuts
/*
  double eta_cuts_comb[2] 	 = {-2.4, 2.4};
  double eta_cuts_incl[2] 	 = {0., 2.4};
  double pt_cuts_comb[10] 	 = {20., 30., 50., 70., 100., 140., 200., 300., 600., 1000.,};
  double pt_cuts_incl[2] 	 = {20., 1000.};
  double pt_cuts_comb_central[2] = {20., 1000.};
  double flavor_cuts[4] 	 = {-0.5, 3.5, 4.5, 5.5};
*/

  double eta_cuts[13] 	 = {0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4};
  double pt_cuts[10] 	 = {30., 50., 70., 100., 140., 200., 300., 600., 1000.};
  double flavor_cuts[4]	 = {-0.5, 3.5, 4.5, 5.5};

  // define numerator and denominator histograms
  TH3D* numerator;
  TH3D* denominator;

  numerator = new TH3D("btagEfficiency_medium", "btagEfficiency_medium", 12, eta_cuts, 8, pt_cuts, 3, flavor_cuts);
  denominator = static_cast<TH3D*>(numerator->Clone("btagEfficiency_denominator"));

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
    jets_hem->clear();
    flavor=0, pt=0, eta=0, dcsv=0, tot=0;

    if(ievt%10000==0) cout << ievt << " / " << ch->GetEntries() << endl;

    ch->GetEntry(ievt);
    if(pass!=1) continue;
    for(int ijet=0; ijet<jets_hflavor->size(); ijet++) {

      flavor=0, pt=0, eta=0, dcsv=0;


      flavor = TMath::Abs(jets_hflavor->at(ijet));
      if(flavor==21) flavor=0;
      if(jets_pt->at(ijet)>999.99) jets_pt->at(ijet)=999.98;
      pt = jets_pt->at(ijet);
      eta = TMath::Abs(jets_eta->at(ijet));
      dcsv = jets_dcsvb->at(ijet);

      if(jets_islep->at(ijet)==1) continue;
      if(pt<30) break;
      if(eta>2.4) continue;
      if(jets_id->at(ijet)!=1) continue;
      if(jets_hem->at(ijet)==1) continue;

      denominator->Fill(eta, pt, flavor);
      if(dcsv > dcsv_med_WP[year]) { 
        numerator->Fill(eta, pt, flavor);
      }
    }
  }

  numerator->Divide(denominator);
  numerator->Write();

  outputfile->Close();
  cout << "... transferring output file" << endl;
  cout << Form("... cp %s /home/bjhong/nanoprocessing/btagEfficiency/ultralegacy/ptetabinned/%s/%s", outputfile->GetName(), year.Data(), outputfile->GetTitle()) << endl;
  gSystem->Exec(Form("cp %s /home/bjhong/nanoprocessing/btagEfficiency/ultralegacy/ptetabinned/%s/%s", outputfile->GetName(), year.Data(), outputfile->GetTitle()));
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
