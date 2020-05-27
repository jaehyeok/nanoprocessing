void isr_norm(int mass = 1000)
{
	TChain ch("tree");
	ch.Add(Form("/xrootd_user/yjeong/xrootd/nanoprocessing/2016/processed/*SMS-T1tbs_RPV_mGluino%i*", mass));
	TH1D *h = new TH1D("h", "h", 1, 0, 1);
	TH1D *h_isr = new TH1D("h_isr", "h_isr", 1, 0, 1);
	ch.Draw("0.5>>h", "1", "goff");
	ch.Draw("0.5>>h_isr", "w_isr", "goff");
	cout <<"mass "<<mass<<": "<< h->Integral()/h_isr->Integral() << endl;
}
