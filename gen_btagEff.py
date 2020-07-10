#! /usr/bin/env python

import argparse
import ROOT
import array
import sys 

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
	elif(process=="test"):
		ret.append("mGluino2000")
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


def gen_btagEff(out_file_path, docuts, process):
	print process
	ROOT.TH1.SetDefaultSumw2()
	l_procname = process_name_list(process)

	c = ROOT.TChain("tree","tree")
	for procname in l_procname:
		c.Add("/xrootd_user/yjeong/xrootd/nanoprocessing/2016/processed_200708/*"+procname+"*.root")

	dcsv_med = ("medium", 0.6324)

	eta_cuts = [0., 0.8, 1.6, 2.4]
	pt_cuts = [20., 30., 50., 70., 100., 1.e4]
	flavor_cuts = [-0.5, 3.5, 4.5, 5.5]

	numerator = ROOT.TH3D("btagEfficiency_"+dcsv_med[0], "btagEfficiency_"+dcsv_med[0], len(eta_cuts)-1, array.array('d', eta_cuts), len(pt_cuts)-1, array.array('d', pt_cuts), len(flavor_cuts)-1, array.array('d', flavor_cuts))

	denominator = numerator.Clone("btagEfficiency_denominator")

	entry = 0
	num_entries = c.GetEntries()

	for event in c : 
		if entry % (num_entries/10000) == 0 :
			printProgress(entry, num_entries, 'Progress:', 'Complete', 2, 50)
		entry = entry + 1 
		if ( process == "qcd" or process == "ttbar") :
			if not (c.stitch_ht and getattr(c,"pass")): continue
		elif not (getattr(c,"pass")) : continue
		if docuts : 
			if not (c.nleps == 1 and c.ht > 1200 and c.mj12 > 500 and c.njets >= 4 ) : 
				continue
		for ijet in xrange(len(c.jets_dcsvb)):
			if (c.jets_islep[ijet]) : continue
			flavor = abs(c.jets_hflavor[ijet]) 
			if (flavor == 21) : flavor = 0
			pt     = c.jets_pt[ijet]
			eta    = abs(c.jets_eta[ijet])
			dcsv   = c.jets_dcsvb[ijet]

			denominator.Fill(eta,pt,flavor)
			if (dcsv > dcsv_med[1]):
				numerator.Fill(eta,pt,flavor)

	eff = numerator.Clone("Efficiency")
	eff = ROOT.TH3D.Divide(numerator, denominator)
	out_file = ROOT.TFile(out_file_path.replace(".root","_"+process+".root"), "recreate");
	numerator.Write()
	doc = ROOT.TNamed("Documentation : this file contains a parametrization in (eta,pt,flavor) for deep CSVv2 b-tagger","");
	doc.Write()
	out_file.Close()

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description = "Computes b-tagging efficiency as a fucntion of pT, eta, and flavor", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	parser.add_argument("-o", "--out_file", metavar="OUT_FILE", default="btagEfficiency.root", help="Save efficiencies to %(metavar)s")
	parser.add_argument("-c", "--docuts", action="store_true", help="Use all available events, applying only basic filters")
	parser.add_argument("-p", "--process", action="store", default="test",help="Select the process which we want to produce")
	args = parser.parse_args()

	gen_btagEff(args.out_file, args.docuts, args.process)






