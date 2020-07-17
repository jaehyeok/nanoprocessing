#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<sstream>

#include"jetTools.h"
#include"TROOT.h"
#include"TMath.h"
#include"TFile.h"
#include"TTree.h"
#include"TSystem.h"
#include"TH2F.h"
#include"TH1F.h"
#include"TEllipse.h"
#include"TCanvas.h"
#include"TString.h"
#include"TLorentzVector.h"
#include"TChain.h"
#include"TRandom.h"

#include"BTagCalibrationStandalone.h"
#include"mcTools.h"
#include"lepTools.h"
#include"inJSON.h"


int main(){
	string csvfile = "data/DeepCSV_2016LegacySF_V1.csv";
//	string csvfile = "data/DeepCSV_toy.csv";

	BTagCalibration calib("DeepCSV",csvfile);
	BTagCalibrationReader calibreader(BTagEntry::OP_MEDIUM, "central",{"up","down"});
	calibreader.load(calib, BTagEntry::FLAV_B,    "comb");
	calibreader.load(calib, BTagEntry::FLAV_C,    "comb");
	calibreader.load(calib, BTagEntry::FLAV_UDSG, "incl");

	TFile *f_bef = new TFile("btagEfficiency_qcd.root","READ"); 
	gRandom->SetSeed(time(0));

	TH1F *w_btag = new TH1F("w_btag","w_btag",30,0.9,1.1);
	TH1F *w_btag_up = new TH1F("w_btag","w_btag",30,0.9,1.1);
	TH1F *w_btag_down = new TH1F("w_btag","w_btag",30,0.9,1.1);
	TH1F *w_ltag_up = new TH1F("w_btag","w_btag",30,0.9,1.1);
	TH1F *w_ltag_down = new TH1F("w_btag","w_btag",30,0.9,1.1);

	float csv;
/*
	float w_btag_dcsv(1), w_bctag_up(1), w_bctag_down(1);

	float rand_pt = 49;
	float rand_eta = 1.4;
	int rand_flav = 5;
	csv = 0.699;

	w_btag_dcsv = w_btag_dcsv*getBtagWeight(f_bef, calibreader, rand_pt, rand_eta, rand_flav, csv, 0.6321);
	w_bctag_up = w_bctag_up*getBtagWeight(f_bef, calibreader, rand_pt, rand_eta, rand_flav, csv, 0.6321, "up_hf");
	w_bctag_down = w_bctag_down*getBtagWeight(f_bef, calibreader, rand_pt, rand_eta, rand_flav, csv, 0.6321, "down_hf");
	cout<<w_btag_dcsv<<"||"<<w_bctag_up<<"||"<<w_bctag_down<<endl;
// */
	
 	for(int i=0; i<2000000 ; i++){
		float w_btag_dcsv(1), w_bctag_up(1), w_bctag_down(1);
		float w_lftag_up(1), w_lftag_down(1);
		Int_t rand_njets = max(1,min(100,(int)gRandom->Exp(10)));
		if(i%100000==0)cout<<rand_njets<<endl;
		for(int j=0; j<rand_njets; j++){
			Double_t rand_pt = max(30.,min(1000.,gRandom->Gaus(1300./rand_njets,TMath::Sqrt(1300./rand_njets)))); 
			Double_t rand_eta = gRandom->Uniform(2.39); 
			Int_t rand_flav = gRandom->Integer(6);
			if(rand_flav<4) rand_flav=0;

			Int_t rand_rand = gRandom->Integer(100);

			if(rand_flav<5 && rand_rand&100==101) csv = 0.699;
			else if(rand_flav==5 && rand_rand==101) csv = 0.622;
			else if(rand_flav<5) csv = 0.611;
			else csv = 0.698;


			w_btag_dcsv = w_btag_dcsv*getBtagWeight(f_bef, calibreader, rand_pt, rand_eta, rand_flav, csv, 0.6321);
			w_bctag_up = w_bctag_up*getBtagWeight(f_bef, calibreader, rand_pt, rand_eta, rand_flav, csv, 0.6321, "up_hf");
			w_bctag_down = w_bctag_down*getBtagWeight(f_bef, calibreader, rand_pt, rand_eta, rand_flav, csv, 0.6321, "down_hf");
			w_lftag_up = w_lftag_up*getBtagWeight(f_bef, calibreader, rand_pt, rand_eta, rand_flav, csv, 0.6321, "up_lf");
			w_lftag_down = w_lftag_down*getBtagWeight(f_bef, calibreader, rand_pt, rand_eta, rand_flav, csv, 0.6321, "down_lf");

		}
		w_btag->Fill(w_btag_dcsv);
		w_btag_up->Fill(w_bctag_up);
		w_btag_down->Fill(w_bctag_down);
		w_ltag_up->Fill(w_lftag_up);
		w_ltag_down->Fill(w_lftag_down);
		if(i%100000==0)cout<<w_btag_dcsv<<"||"<<w_bctag_up<<"||"<<w_bctag_down<<endl;
	}
	TCanvas *c1 = new TCanvas("c","c",1600,800);
	c1->Divide(2,1);
	c1->cd(1);
	gPad->SetLogy();
	w_btag->SetStats(0);
	w_btag->SetMaximum(700000);
	w_btag->Draw("hist");

	w_btag_up->SetLineColor(kRed);
	w_btag_up->Draw("same hist");
	w_btag_down->SetLineColor(kBlack);
	w_btag_down->Draw("same hist");
	c1->cd(2);
	gPad->SetLogy();
	w_btag->SetStats(0);
	w_btag->Draw("hist");
	w_ltag_up->SetLineColor(kRed);
	w_ltag_up->Draw("same hist");
	w_ltag_down->SetLineColor(kBlack);
	w_ltag_down->Draw("same hist");

	cout<<w_btag->GetMean()<<"||"<<w_btag_up->GetMean()<<"||"<<w_btag_down->GetMean()<<endl;

	c1->Print("test.pdf");
// */
}

