#! /usr/bin/env python

import argparse
import ROOT
import array
import sys 

dcsv_med_WP = {"2016":0.6324, "2017":0.4941, "2018":0.4184}

def get_sample_tag_list(year):
	f = open("condor/samples/samples"+year+"_v7.txt",'r')
	lines = f.readlines()
	ret = []
	for line in lines :
		tag = line.split("_13TeV")[0]
		if tag.startswith('#'): continue
		#if not 'WW_' in tag : continue
		ret.append(tag)
	return ret

def process_name_list(process):
	ret = []
	if(process=="ttbar"):
		ret.append("TTJets")
	elif(process=="qcd"):
		ret.append("QCD_")
	elif(process=="wjets"):
		ret.append("WJetsToLNu")
		ret.append("WJetsToQQ")
	elif(process=="other"):
		ret.append("DYJetsToLL_M-50_HT-")
		ret.append("TTTT_")
		ret.append("TTWJetsToLNu_")
		ret.append("TTZToLLNuNu_")
		ret.append("WZ_")
		ret.append("ZZ_")
		ret.append("WW_")
		ret.append("WZZ_")
		ret.append("WWZ_")
		ret.append("ZZZ_")
		ret.append("WWW_")
	elif(process=="signal_m1000"):
		ret.append("mGluino1000")
	elif(process=="signal_m1100"):
		ret.append("mGluino1100")
	elif(process=="signal_m1200"):
		ret.append("mGluino1200")
	elif(process=="signal_m1300"):
		ret.append("mGluino1300")
	elif(process=="signal_m1400"):
		ret.append("mGluino1400")
	elif(process=="signal_m1500"):
		ret.append("mGluino1500")
	elif(process=="signal_m1600"):
		ret.append("mGluino1600")
	elif(process=="signal_m1700"):
		ret.append("mGluino1700")
	elif(process=="signal_m1800"):
		ret.append("mGluino1800")
	elif(process=="signal_m1900"):
		ret.append("mGluino1900")
	elif(process=="signal_m2000"):
		ret.append("mGluino2000")
	elif(process=="signal_m2100"):
		ret.append("mGluino2100")
	else:
		print "Invaild Process Name! \n Usable Process Names : qcd, ttbar, wjets, other, and test"
	return ret

def printProgress(iteration, total, prefix = '', suffix ='', decimals = 1, barLength=100):
	formatStr = "{0:."+str(decimals)+"f}"
	percent = formatStr.format(100*(iteration/float(total)))
	filledLength = int(round(barLength*iteration/float(total)))
	bar = "#"*filledLength+' '*(barLength-filledLength)
	sys.stdout.write('\r%s |%s| %s%s %s' %(prefix, bar,percent, '%', suffix)),
	if iteration == total : 
		sys.stdout.write('\n')
	sys.stdout.flush()


def gen_btagEff(out_file_path, docuts, process, year):
	print process
	ROOT.TH1.SetDefaultSumw2()

	c = ROOT.TChain("tree")
	c.Add("/xrootd_user/yjeong/xrootd/nanoprocessing/"+year+"/processed/*"+process+"*.root")
	print(c.GetEntries())

	dcsv_med = ("medium", dcsv_med_WP[year])

	eta_cuts_comb = [-2.4, 2.4]
	eta_cuts_incl = [0., 2.4]
	pt_cuts_comb = [20., 30., 50., 70., 100., 140., 200., 300., 600., 1000.,]
	pt_cuts_incl = [20., 1000.]
	pt_cuts_comb_central = [20., 1000.]
	flavor_cuts = [-0.5, 3.5, 4.5, 5.5]

	numerator_comb = ROOT.TH3D("btagEfficiency_"+dcsv_med[0]+"_comb", "btagEfficiency_"+dcsv_med[0]+"_comb", len(eta_cuts_comb)-1, array.array('d', eta_cuts_comb), len(pt_cuts_comb)-1, array.array('d', pt_cuts_comb), len(flavor_cuts)-1, array.array('d', flavor_cuts))
	numerator_comb_central = ROOT.TH3D("btagEfficiency_"+dcsv_med[0]+"_comb_central", "btagEfficiency_"+dcsv_med[0]+"_comb_central", len(eta_cuts_comb)-1, array.array('d', eta_cuts_comb), len(pt_cuts_comb_central)-1, array.array('d', pt_cuts_comb_central), len(flavor_cuts)-1, array.array('d', flavor_cuts))
	numerator_incl = ROOT.TH3D("btagEfficiency_"+dcsv_med[0]+"_incl", "btagEfficiency_"+dcsv_med[0]+"_incl", len(eta_cuts_incl)-1, array.array('d', eta_cuts_incl), len(pt_cuts_incl)-1, array.array('d', pt_cuts_incl), len(flavor_cuts)-1, array.array('d', flavor_cuts))

	denominator_comb = numerator_comb.Clone("btagEfficiency_denominator_comb")
	denominator_comb_central = numerator_comb_central.Clone("btagEfficiency_denominator_comb_central")
	denominator_incl = numerator_incl.Clone("btagEfficiency_denominator_incl")

	entry = 0
	num_entries = c.GetEntries()
	for event in c : 
		if entry % (num_entries/10000) == 0 :
			printProgress(entry, num_entries, 'Progress:', 'Complete', 2, 50)
		entry = entry + 1 
		if(not getattr(c,"pass")) : continue
		for ijet in range(len(c.jets_hflavor)):
			flavor = abs(c.jets_hflavor[ijet]) 
			if (flavor == 21) : flavor = 0
			pt     = c.jets_pt[ijet]
			pt     = min(pt,999.99)
			eta    = abs(c.jets_eta[ijet])
			dcsv   = c.jets_dcsvb[ijet]
			if (c.jets_islep[ijet]) : continue
			if (pt<30) : continue
			if (eta>2.4) : continue
			if (not c.jets_id[ijet]) : continue
			denominator_comb.Fill(eta,pt,flavor)
			denominator_comb_central.Fill(eta,pt,flavor)
			denominator_incl.Fill(eta,pt,flavor)
			if (dcsv > dcsv_med[1]):
				numerator_comb.Fill(eta,pt,flavor)
				numerator_comb_central.Fill(eta,pt,flavor)
				numerator_incl.Fill(eta,pt,flavor)
	print '\n'
	print denominator_incl.GetBinContent(1,1,3)
	print denominator_comb_central.GetBinContent(1,1,3)
	tot =0
	for k in range(1,len(flavor_cuts)):
		for i in range(1,len(eta_cuts_comb)):
			for j in range(1,len(pt_cuts_comb)):
				tot = tot + denominator_comb.GetBinContent(i,j,k)
	print tot
	print numerator_comb.GetBinContent(1,4,3)
	print denominator_comb.GetBinContent(1,4,3)
	print numerator_incl.GetBinContent(1,1,3)
	print numerator_comb_central.GetBinContent(1,1,3)
	eff_comb = numerator_comb.Clone("Efficiency_comb")
	eff_comb_central = numerator_comb_central.Clone("Efficiency_comb_central")
	eff_incl = numerator_incl.Clone("Efficiency_incl")
	ROOT.TH3D.Divide(numerator_comb, denominator_comb)
	ROOT.TH3D.Divide(numerator_comb_central, denominator_comb_central)
	ROOT.TH3D.Divide(numerator_incl, denominator_incl)
	out_file = ROOT.TFile(out_file_path.replace(".root","_"+process+"_"+str(year)+".root"), "recreate");
#	eff_comb.Write()
#	eff_comb_central.Write()
#	eff_incl.Write()
	numerator_comb.Write()
	numerator_comb_central.Write()
	numerator_incl.Write()
	doc = ROOT.TNamed("Documentation : this file contains a parametrization in (eta,pt,flavor) for deep CSVv2 b-tagger","");
	doc.Write()
	out_file.Close()

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description = "Computes b-tagging efficiency as a fucntion of pT, eta, and flavor", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	parser.add_argument("-o", "--out_file", metavar="OUT_FILE", default="btagEfficiency.root", help="Save efficiencies to %(metavar)s")
	parser.add_argument("-c", "--docuts", action="store_true", help="Use all available events, applying only basic filters")
	parser.add_argument("-y", "--year", action="store", default="2016",help="Select the year that you want to make trigger efficiency")
#	parser.add_argument("-p", "--process", action="store", default="*StealthSHH_2t4b_mStop*",help="make process")

	args = parser.parse_args()

	print args.docuts
	list_tags = get_sample_tag_list(args.year)
	if args.docuts : 
		gen_btagEff(args.out_file, args.docuts, "WWW_4F_TuneCUETP8M1", args.year)
	else :
		for process in list_tags:
			gen_btagEff(args.out_file, args.docuts, process, args.year)
