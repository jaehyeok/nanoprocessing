#! /usr/bin/env python

import ROOT
import array
import sys

def compare_root():
	process = ["qcd","wjets","other"]
	g = ROOT.TFile("btagEfficiency_ttbar.root","READ")
	h1 = g.Get("btagEfficiency_medium")
	for p_name in process:
		f = ROOT.TFile("btagEfficiency_"+p_name+".root","READ")
		print p_name
		h = f.Get("btagEfficiency_medium")
		print h.GetBinContent(1,3,2)
		dv = h.Clone("btagEfficiency_dv")
		dv.Divide(h,h1)
		dv_val = []
		dv_err = []
		dv_str = []
		dv_xax = ["0 to 0.8", "0.8 to 1.6", "1.6 to 2.4"]
		dv_yax = ["20 to 30", "30 to 50", "70 to 100", "100 to 1000"]
		dv_zax = ["light quarks","charm quark","heavy quark"]
		for i in dv_xax:
			for j in dv_yax:
				for k in dv_zax:
					dv_err.append(dv.GetBinError(dv_xax.index(i)+1,dv_yax.index(j)+1,dv_zax.index(k)+1))
					dv_val.append('{0:.3f}'.format(dv.GetBinContent(dv_xax.index(i)+1,dv_yax.index(j)+1,dv_zax.index(k)+1)))
					dv_str.append("eta : "+i+"/ pt : "+j+"/ quark : "+k)
		for value in dv_str :
			print value+" - \n"+str(dv_val[dv_str.index(value)])+'+-'+str(dv_err[dv_str.index(value)])
	raw_input("")

if __name__=="__main__":
	compare_root()
