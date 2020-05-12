#include <typeinfo>

void set_histo_ratio(TH1F *histo_Ratio){
	histo_Ratio->SetMarkerStyle(20);
	histo_Ratio->SetMarkerSize(0.6);
	histo_Ratio->GetYaxis()->SetTitle("Ratio");
	histo_Ratio->GetYaxis()->SetTitleSize(0.17);
	histo_Ratio->GetYaxis()->SetTitleOffset(0.2);
	histo_Ratio->GetYaxis()->SetLabelSize(0.13);
	histo_Ratio->GetYaxis()->CenterTitle();
	histo_Ratio->GetYaxis()->SetNdivisions(6);
	histo_Ratio->GetXaxis()->SetLabelSize(0.13);
	histo_Ratio->GetXaxis()->SetTitleSize(0.2);
	histo_Ratio->SetAxisRange(0.5,1.5,"y");
}

void set_legend_style(TLegend *le){
	le->SetFillColor(0);
	le->SetLineColor(0);
	le->SetLineStyle(kSolid);
	le->SetLineWidth(1);
	le->SetFillStyle(1001);
	le->SetTextFont(42);
	le->SetTextSize(0.045);
}

void set_hist_style_1(TH1F *h_1){
	h_1->SetLineColor(1);
	h_1->SetLineStyle(1);
	h_1->SetLineWidth(1.2);
}

void set_hist_style_2(TH1F *h_2){
	h_2->SetLineColor(2);
	h_2->SetLineStyle(2);
	h_2->SetLineWidth(1);
}

void comparison_variable(){

	const int Sample_Num = 2;
	TFile *tfile[Sample_Num];
	TH1F *h[Sample_Num];
	TH1F *h1[Sample_Num];
	TH1F *h2[Sample_Num];
	TH1F *h3[Sample_Num];
	TTree *tree_[Sample_Num];
	TCanvas *c;
	c = new TCanvas;
	c->Divide(2,2);

	TLegend *l;
	TLegend *l1;
	TLegend *l2;
	TLegend *l3;

	l = new TLegend(0.65,0.54,0.75,0.8);
	set_legend_style(l);
	l1 = new TLegend(0.65,0.54,0.75,0.8);
	set_legend_style(l1);
	l2 = new TLegend(0.65,0.54,0.75,0.8);
	set_legend_style(l2);
	l3 = new TLegend(0.65,0.54,0.75,0.8);
	set_legend_style(l3);

	float ht, mj12, w_lumi, weight;
	int njets, nbm;
	double ymax = 0;
	double norm_1 = 1, norm_2 = 1, norm_3 = 1, norm_4 = 1;
	double norm_5 = 1, norm_6 = 1, norm_7 = 1, norm_8 = 1;

	TString inputdir, outputdir;
	inputdir = "/cms/scratch/yjeong/";
	outputdir = "/cms/scratch/yjeong/CMSSW_7_1_0/src/nanoprocessing/plots/";

	TString sample_name[Sample_Num] = {
		"mergedbaby__TTJets_Tune_skim_rpvfit_rpvmc_nfiles_60_ntruleps0",
		"TTJets_Tune"
	};

	for (int i=0; i < Sample_Num; i++){
		tfile[i] = new TFile(inputdir+sample_name[i]+".root");
	}

	for(int i=0; i<Sample_Num; i++){
		h[i] = new TH1F(Form("h_%d",i),Form("NJets_%d",i),10,0,18);
		h1[i] = new TH1F(Form("h1_%d",i),Form("mj12_%d",i),50,0,3500);
		h2[i] = new TH1F(Form("h2_%d",i),Form("nbm_%d",i),9,0,9);
		h3[i] = new TH1F(Form("h3_%d",i),Form("ht_%d",i),50,0,8000);

		tree_[i] = (TTree*)tfile[i]->Get("tree");

		//for(int j=0; j<tree[i]->GetEntries(); j++){
		for(int j=0; j<100000; j++){
			tree_[i]->GetEntry(j);
			tree_[i]->SetBranchAddress("ht",&ht);
			tree_[i]->SetBranchAddress("mj12",&mj12);
			tree_[i]->SetBranchAddress("w_lumi",&w_lumi);
			tree_[i]->SetBranchAddress("njets",&njets);
			tree_[i]->SetBranchAddress("nbm",&nbm);
			tree_[i]->SetBranchAddress("weight",&weight);

			h[i]->Fill(njets,w_lumi);//FIXME
			h1[i]->Fill(mj12,w_lumi);
			h2[i]->Fill(nbm,w_lumi);
			h3[i]->Fill(ht,w_lumi);
		}

		if(i==0){
			set_hist_style_1(h[i]);
			set_hist_style_1(h1[i]);
			set_hist_style_1(h2[i]);
			set_hist_style_1(h3[i]);
			norm_1 = h[i]->Integral();
			h[i]->Scale(1/norm_1);
			norm_2 = h1[i]->Integral();
			h1[i]->Scale(1/norm_2);
			norm_3 = h2[i]->Integral();
			h2[i]->Scale(1/norm_3);
			norm_4 = h3[i]->Integral();
			h3[i]->Scale(1/norm_4);
			l->AddEntry(h[i],"MiniAOD");
			l1->AddEntry(h1[i],"MiniAOD");
			l2->AddEntry(h2[i],"MiniAOD");
			l3->AddEntry(h3[i],"MiniAOD");
		}
		if(i==1){
			set_hist_style_2(h[i]);
			set_hist_style_2(h1[i]);
			set_hist_style_2(h2[i]);
			set_hist_style_2(h3[i]);
			norm_5 = h[i]->Integral();
			h[i]->Scale(1/norm_5);
			norm_6 = h1[i]->Integral();
			h1[i]->Scale(1/norm_6);
			norm_7 = h2[i]->Integral();
			h2[i]->Scale(1/norm_7);
			norm_8 = h3[i]->Integral();
			h3[i]->Scale(1/norm_8);
			l->AddEntry(h[i],"NanoAOD");
			l1->AddEntry(h1[i],"NanoAOD");
			l2->AddEntry(h2[i],"NanoAOD");
			l3->AddEntry(h3[i],"NanoAOD");
		}

		if(i==0){
			ymax = h[i]->GetMaximum();
			h[i]->SetMaximum(ymax*2);
			ymax = h1[i]->GetMaximum();
			h1[i]->SetMaximum(ymax*2);
			ymax = h2[i]->GetMaximum();
			h2[i]->SetMaximum(ymax*2);
			ymax = h3[i]->GetMaximum();
			h3[i]->SetMaximum(ymax*2);
		}
		c->cd(1);
		l->Draw();
		if(i==0)h[i]->Draw();
		else if(i>0)h[i]->Draw("same");

		c->cd(2);
		l1->Draw();
		if(i==0)h1[i]->Draw();
		else if(i>0)h1[i]->Draw("same");

		c->cd(3);
		l2->Draw();
		if(i==0)h2[i]->Draw();
		else if(i>0)h2[i]->Draw("same");

		c->cd(4);
		l3->Draw();
		if(i==0)h3[i]->Draw();
		else if(i>0)h3[i]->Draw("same");

	}
	c->SaveAs(outputdir+"w_lumi.png");//FIXME
	}
