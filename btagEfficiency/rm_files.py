#!/usr/bin/env python

import os
import ROOT
from ROOT import TH2D, TH2F, TH3D, TH3F, TCanvas, TFile

file_list = os.listdir("./")
file_list_root = [file_ for file_ in file_list if file_.endswith(".root") and (file_.endswith("_2016.root") or file_.endswith("_2017.root") or file_.endswith("_2018.root"))]
i=0
for file_ in file_list_root : 
	i = i + 1
	print(str(i)+'/'+str(len(file_list_root)))	
	f = TFile.Open(file_,'read')
	h3_comb = f.Get('btagEfficiency_medium_comb')
	h3_comb_cent = f.Get('btagEfficiency_medium_comb_central')
	h3_incl = f.Get('btagEfficiency_medium_incl')
	h2_comb = h3_comb.Project3D("yz")
	h2_comb_cent = h3_comb_cent.Project3D("yz")
	h2_incl = h3_incl.Project3D("yz")
	bc1 = h2_comb.GetBinContent(1,2)
	bc2 = h2_comb.GetBinContent(2,2)
	bc3 = h2_comb.GetBinContent(3,2)
	#if not (bc1 == 0 or bc2 == 0 or bc3 == 0) : continue
	c = TCanvas("c","c",2400,800)
	c.Divide(3,1)
	c.cd(1)
	h2_comb.SetStats(0)
	h2_comb.Draw("colz text")
	c.cd(2)
	h2_comb_cent.SetStats(0)
	h2_comb_cent.Draw("colz text")
	c.cd(3)
	h2_incl.SetStats(0)
	h2_incl.Draw("colz text")
	c.Print('pdf/'+file_.replace('.root','.pdf'))
	f.Close()
