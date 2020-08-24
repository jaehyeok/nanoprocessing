{
	//gStyle->SetOptStat(0);
	TCanvas *c1, *c2, *c3;
	c1 = new TCanvas;
	TFile f("btagEfficiency/2017/btagEfficiency_TTZToQQ_TuneCP5.root");
	TH3 *h3 = (TH3*)f.Get("btagEfficiency_medium_comb");
	TH2 *h11 = h3->Project3D("yz");
	c1->cd();
	h11->SetTitle("btagEfficiency_medium_comb_v6");
	h11->Draw("colz text");

	c2 = new TCanvas;
	TFile f1("btagEfficiency/btagEfficiency_TTZToQQ_TuneCP5_2017.root");
	TH3 *h4 = (TH3*)f1.Get("btagEfficiency_medium_comb");
	TH2 *h22 = h4->Project3D("yz");
	c2->cd();
	h22->SetTitle("btagEfficiency_medium_comb_v7");
	h22->Draw("colz text");

	c3 = new TCanvas;
	TH2 *hratio;
	hratio = (TH2*)h11->Clone();
	hratio->Divide(h22);
	hratio->SetTitle("Division");
	c3->cd();
	hratio->Draw("text");
}
