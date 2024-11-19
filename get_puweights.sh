#!/usr/bin/sh

year=$1
echo ${year}
for process in DYJetsToLL_M-50_HT-100to200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8 DYJetsToLL_M-50_HT-1200to2500_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8 DYJetsToLL_M-50_HT-200to400_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8 DYJetsToLL_M-50_HT-2500toInf_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8 DYJetsToLL_M-50_HT-400to600_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8 DYJetsToLL_M-50_HT-600to800_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8 DYJetsToLL_M-50_HT-70to100_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8 DYJetsToLL_M-50_HT-800to1200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8 QCD_HT1000to1500_TuneCP5_PSWeights_13TeV-madgraph-pythia8 QCD_HT100to200_TuneCP5_PSWeights_13TeV-madgraph-pythia8 QCD_HT1500to2000_TuneCP5_PSWeights_13TeV-madgraph-pythia8 QCD_HT2000toInf_TuneCP5_PSWeights_13TeV-madgraph-pythia8 QCD_HT200to300_TuneCP5_PSWeights_13TeV-madgraph-pythia8 QCD_HT300to500_TuneCP5_PSWeights_13TeV-madgraph-pythia8 QCD_HT500to700_TuneCP5_PSWeights_13TeV-madgraph-pythia8 QCD_HT50to100_TuneCP5_PSWeights_13TeV-madgraph-pythia8 QCD_HT700to1000_TuneCP5_PSWeights_13TeV-madgraph-pythia8 ST_s-channel_4f_hadronicDecays_TuneCP5_13TeV-amcatnlo-pythia8 ST_s-channel_4f_leptonDecays_TuneCP5_13TeV-amcatnlo-pythia8 ST_t-channel_antitop_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin-pythia8 ST_t-channel_top_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin-pythia8 ST_tW_antitop_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8 ST_tW_top_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8 ttHJetTobb_M125_TuneCP5_13TeV_amcatnloFXFX_madspin_pythia8 TTJets_HT-1200to2500_TuneCP5_13TeV-madgraphMLM-pythia8 TTJets_HT-2500toInf_TuneCP5_13TeV-madgraphMLM-pythia8 TTJets_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8 TTJets_HT-800to1200_TuneCP5_13TeV-madgraphMLM-pythia8 TTJets_TuneCP5_13TeV-madgraphMLM-pythia8 TTTT_TuneCP5_13TeV-amcatnlo-pythia8 TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8 TTWJetsToQQ_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8 TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8 TTZToQQ_TuneCP5_13TeV-amcatnlo-pythia8 WJetsToLNu_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8 WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8 WJetsToLNu_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8 WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8 WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8 WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8 WJetsToLNu_HT-70To100_TuneCP5_13TeV-madgraphMLM-pythia8 WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8 WW_TuneCP5_13TeV-pythia8 WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8 WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8 WZ_TuneCP5_13TeV-pythia8 WZZ_TuneCP5_13TeV-amcatnlo-pythia8 ZZ_TuneCP5_13TeV-pythia8 ZZZ_TuneCP5_13TeV-amcatnlo-pythia8
do
	./get_puweights.exe ${year} ${process}
done
