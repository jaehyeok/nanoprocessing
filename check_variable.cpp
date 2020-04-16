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

void set_legend_style(TLegend *l1){
	l1->SetFillColor(0);
	l1->SetLineColor(0);
	l1->SetLineStyle(kSolid);
	l1->SetLineWidth(1);
	l1->SetFillStyle(1001);
	l1->SetTextFont(42);
	l1->SetTextSize(0.045);
}

void check_variable(){
	//TString inputdir_1 = "/xrootd_user/jaehyeok/xrootd/2016v4/2019_12_10/processed/";
	//TString inputdir_2 = "/xrootd_user/yjeong/xrootd/nanoprocessing/2016/processed/";

	TString inputdir = "/cms/scratch/yjeong/";

	//TString sample_name_1 = "4809C572-5167-514C-9186-FF01CBCA4C9C_fatjetbaby_";//version4
	TString sample_name_1 = "69F3B720-0CF6-EE4E-BAA3-E332F9DBD180_fatjetbaby_";//version5
	TString sample_name_2 = "25AFDB82-AB3F-EB48-92BB-70BA8EEFD469_fatjetbaby_";//version6

	tag_name = "TT_TuneCUETP8M2T4";

	TString outputdir = "/cms/scratch/yjeong/CMSSW_7_1_0/src/nanoprocessing/plots/";

	TTree *mytree_1;
	TTree *mytree_2;
	TFile *tfile_1;
	TFile *tfile_2;

	tfile_1 = new TFile(inputdir+sample_name_1+tag_name+".root");
	mytree_1 = (TTree*)tfile_1->Get("tree");
	tfile_2 = new TFile(inputdir+sample_name_2+tag_name+".root");
	mytree_2 = (TTree*)tfile_2->Get("tree");

	TObjArray *blist;
	blist = mytree_2->GetListOfBranches();
	//const int nBranch = blist->GetEntries();
	blist->Print();
	cout<< blist->GetEntries() <<endl;//56
	cout<< typeid(blist->GetEntries()).name() <<endl;
	cout<<"version4 entries: "<<mytree_1->GetEntries()<<endl;
	cout<<"version5 entries: "<<mytree_2->GetEntries()<<endl;
	const double nBranch = 56;
	//const int nBranch__ = blist->GetEntries();

	TH1F *h1[nBranch];
	TH1F *h2[nBranch];
	TH1F *heff[nBranch];
	TCanvas *c_[nBranch];

	TPad *plotpad_[nBranch];
	TPad *ratiopad_[nBranch];
	TLegend *l_[nBranch];

	float xmin[nBranch] = {0,};
	float xmax[nBranch] = {0,};
	float ymax = 0;
	int bin = 101;
	TString var_name;

	double norm_1 = 1;
	double norm_2 = 1;
	float a=0,b=0;
	//float w_lumi_ = 0.1;//seperated
	//float w_lumi_ = 0.05;//seperated 
	//float w_lumi_ = 0.02;//seperated // x_range: 0.04
	//float w_lumi_ = 0.01;//one line // x_range: 0.02 //float: seperated
	//float w_lumi_ = 0.009;//one line //float: seperated
	//float w_lumi_ = 0.008;//seperated
	//float w_lumi_ = 0.007;//seperated
	//float w_lumi_ = 0.006;//seperated
	//float w_lumi_ = 0.005;//one line // x_range: 0.01
	//float w_lumi_ = 0.001;//seperated
	//float w_lumi_ = 0.0001;//seperated 
	//float w_lumi_ = 0.00003;//serperated
	float w_lumi_ = 1.2;//serperated
	//float w_lumi_ = 1;//one line

	float x_min[nBranch] = 0;
	float x_max[nBranch] = 0;

	for(int j=0; j<nBranch; j++){
		l_[j] = new TLegend(0.65,0.54,0.75,0.80);

		var_name = blist->At(j)->GetName();
		cout<<"Name: "<<var_name<< endl;

		xmax[j] = mytree_1->GetMaximum(blist->At(j)->GetName());
		xmin[j] = mytree_1->GetMinimum(blist->At(j)->GetName());

		if(xmax[j]==0 && xmin[j]==0) {a=3; b=-3;}
		else if(xmax[j]-xmin[j]==0) {a=1; b=-1;}
		if(var_name.Contains("pt")) {a=700; b=-1;}
		if(var_name.Contains("pdgid")) {a=15; b=-15;}
		if(var_name.Contains("jets_m")) {a=50; b=-1;}
		/*if(j==15){a=15;b=-15;}//leps_pdgID
		else if(j==33 || j==47 || j==44 || j==18 || j==12 || j==24){a=1000;b=0;}//jets_pt, fjets_m, fjets_pt, mus_pt, leps_pt, els_pt
		else if(j==29 || j==30 || j==22){a=10;b=0;}//els_miniso, els_reliso, mus_miniso
		else if(j==36){a=250;b=0;}//jets_m
		else if(j==6 || j==7 || j==39) b=0;//w_btag_csv, w_btag_dcsv, jets_csv
		else if(j==8) {a=w_lumi_;b=-w_lumi_;}//w_lumi*/

		x_max[j] = xmax[j]+a;
		x_min[j] = xmin[j]+b;

		//if(j==9) cout<<"bin_size: "<<(xmax[j]-xmin[j])/bin<<endl;
		cout<<"xmin :"<<x_min[j]<<", xmax :"<<x_max[j]<<endl;

		c_[j] = new TCanvas;
		plotpad_[j] = new TPad(Form("title_%d",j),Form(""),0.02,0.3,0.98,0.98);
		ratiopad_[j] = new TPad(Form("title_%d",j),Form(""),0.02,0.1,0.98,0.35);
		//plotpad_[j]->SetLogy();
		plotpad_[j]->Draw();
		ratiopad_[j]->Draw();

		h1[j] = new TH1F(Form("h1_%d",j),tag_name,bin,x_min[j],x_max[j]);
		mytree_1->Project(Form("h1_%d",j),blist->At(j)->GetName());
		h2[j] = new TH1F(Form("h2_%d",j),tag_name,bin,x_min[j],x_max[j]);
		mytree_2->Project(Form("h2_%d",j),blist->At(j)->GetName());
		plotpad_[j]->cd();

		if(j!=15){
			norm_1 = h1[j]->Integral();
			h1[j]->Scale(1/norm_1);
			norm_2 = h2[j]->Integral();
			h2[j]->Scale(1/norm_2);
		}

		ymax = h2[j]->GetMaximum();

		h1[j]->SetLineColor(kRed);
		h1[j]->SetLineStyle(7);
		h1[j]->SetLineWidth(3);
		h2[j]->SetLineColor(kBlue);

		l_[j]->AddEntry(h1[j],"NanoAODv5");
		l_[j]->AddEntry(h2[j],"NanoAODv6");

		set_legend_style(l_[j]);

		h1[j]->SetMaximum(ymax*2);
		h1[j]->Draw();
		h2[j]->Draw("same");

		l_[j]->Draw();
		ratiopad_[j]->cd();
		heff[j] = new TH1F(Form("heff_%d",j),Form(""),bin,x_min[j],x_max[j]);
		set_histo_ratio(heff[j]);
		heff[j]->GetXaxis()->SetTitle(blist->At(j)->GetName());
		heff[j]->Divide(h2[j],h1[j]);
		heff[j]->Draw("e");
		c_[j]->SaveAs(outputdir+tag_name+"_"+blist->At(j)->GetName()+".png");
	}
}
