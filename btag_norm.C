{
	//TString variable = "w_btag_dcsv";
	//TString variable = "w_pu";
	TString variable = "w_isr";
	TString year = "2017";
	TString inputdir = "/xrootd_user/yjeong/xrootd/nanoprocessing/"+year+"/processed/";
	TChain ch1("tree");
	TChain ch2("tree");
	TChain ch3("tree");
	TChain ch4("tree");
	TChain ch5("tree");

	ch1.Add(inputdir+"DAE57B29-E4ED-1449-9A80-D0904B0EAE6E_fatjetbaby_TTJets_TuneCP5.root");
	ch2.Add(inputdir+"DE3C24CB-E5C7-5D4C-9D87-5F5B70DCD859_fatjetbaby_TTJets_HT-600to800_TuneCP5.root");
	ch3.Add(inputdir+"E8833967-C655-8646-8DD1-0C606B25FD97_fatjetbaby_TTJets_HT-800to1200_TuneCP5.root");
	ch4.Add(inputdir+"E0CF6E16-38CC-044D-AA57-FADAE113F0BE_fatjetbaby_TTJets_HT-1200to2500_TuneCP5.root");
	ch5.Add(inputdir+"F3262E20-51A9-9940-B927-40BC5AA3BF2C_fatjetbaby_TTJets_HT-2500toInf_TuneCP5.root");// */

	/*ch1.Add(inputdir+"A88F976B-F9EE-A644-BB55-C8A8F8917AF0_fatjetbaby_TTJets_TuneCP5.root");
	ch2.Add(inputdir+"B2BDAC11-28FF-4E4A-B079-448EB02C5551_fatjetbaby_TTJets_HT-600to800_TuneCP5.root");
	ch3.Add(inputdir+"B420C4AE-01E1-DB44-A7E5-5AA131AC02D1_fatjetbaby_TTJets_HT-800to1200_TuneCP5.root");
	ch4.Add(inputdir+"D5854647-9F25-C144-89F7-80631314E647_fatjetbaby_TTJets_HT-1200to2500_TuneCP5.root");
	ch5.Add(inputdir+"68BE20D1-21FC-A142-AC46-C6208DBBA733_fatjetbaby_TTJets_HT-2500toInf_TuneCP5.root");// */
	TH1D *h1 = new TH1D("h1", "h1", 1, 0, 1);
	TH1D *h_isr1 = new TH1D("h_isr1", "h_isr1", 1, 0, 1);
	TH1D *h2 = new TH1D("h2", "h2", 1, 0, 1);
	TH1D *h_isr2 = new TH1D("h_isr2", "h_isr2", 1, 0, 1);
	TH1D *h3 = new TH1D("h3", "h3", 1, 0, 1);
	TH1D *h_isr3 = new TH1D("h_isr3", "h_isr3", 1, 0, 1);
	TH1D *h4 = new TH1D("h4", "h4", 1, 0, 1);
	TH1D *h_isr4 = new TH1D("h_isr4", "h_isr4", 1, 0, 1);
	TH1D *h5 = new TH1D("h5", "h5", 1, 0, 1);
	TH1D *h_isr5 = new TH1D("h_isr5", "h_isr5", 1, 0, 1);
	ch1.Draw("0.5>>h1", "1", "goff");
	ch1.Draw("0.5>>h_isr1", variable, "goff");
	ch2.Draw("0.5>>h2", "1", "goff");
	ch2.Draw("0.5>>h_isr2", variable, "goff");
	ch3.Draw("0.5>>h3", "1", "goff");
	ch3.Draw("0.5>>h_isr3", variable, "goff");
	ch4.Draw("0.5>>h4", "1", "goff");
	ch4.Draw("0.5>>h_isr4", variable, "goff");
	ch5.Draw("0.5>>h5", "1", "goff");
	ch5.Draw("0.5>>h_isr5", variable, "goff");
	cout <<variable<<" : "<< h1->Integral()/h_isr1->Integral() << endl;
	cout <<variable<<" : "<< h2->Integral()/h_isr2->Integral() << endl;
	cout <<variable<<" : "<< h3->Integral()/h_isr3->Integral() << endl;
	cout <<variable<<" : "<< h4->Integral()/h_isr4->Integral() << endl;
	cout <<variable<<" : "<< h5->Integral()/h_isr5->Integral() << endl;
}
