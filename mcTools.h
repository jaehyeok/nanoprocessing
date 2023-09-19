#include<vector> 
#include "TString.h"

using namespace std; 

float getXsec(TString process) 
{
  // cross sections are in pb
  float xsec = -0.001; 

  // ttbar //TTJets_HT kFactor = 1.625 LO+NLO/LO
  // https://twiki.cern.ch/twiki/bin/view/LHCPhysics/TtbarNNLO
  if(process.Contains("TTJets_Tune"))			xsec = 833.9;
  if(process.Contains("TTJets_HT-600to800_Tune"))	xsec = 2.61537118;
  if(process.Contains("TTJets_HT-800to1200_Tune"))	xsec = 1.07722318;
  if(process.Contains("TTJets_HT-1200to2500_Tune"))	xsec = 0.194972521;
  if(process.Contains("TTJets_HT-2500toInf_Tune"))	xsec = 0.0023234211;
 
  // wjets
  if(process.Contains("WJetsToLNu_HT-70To100_Tune"))        xsec=1264.;
  if(process.Contains("WJetsToLNu_HT-100To200_Tune"))       xsec=1256.;
  if(process.Contains("WJetsToLNu_HT-200To400_Tune"))       xsec=335.5;
  if(process.Contains("WJetsToLNu_HT-400To600_Tune"))       xsec=45.25;
  if(process.Contains("WJetsToLNu_HT-600To800_Tune"))       xsec=10.97;
  if(process.Contains("WJetsToLNu_HT-800To1200_Tune"))      xsec=4.933;
  if(process.Contains("WJetsToLNu_HT-1200To2500_Tune"))     xsec=1.16;
  if(process.Contains("WJetsToLNu_HT-2500ToInf_Tune"))      xsec=0.02627;

  // QCD 
  if(process.Contains("QCD_HT50to100_Tune"))  	xsec = 186100000.;
  if(process.Contains("QCD_HT100to200_Tune"))   xsec = 23630000.;
  if(process.Contains("QCD_HT200to300_Tune"))   xsec = 1554000.;
  if(process.Contains("QCD_HT300to500_Tune"))   xsec = 325000.;
  if(process.Contains("QCD_HT500to700_Tune"))   xsec = 30350.;
  if(process.Contains("QCD_HT700to1000_Tune"))  xsec = 6403.;
  if(process.Contains("QCD_HT1000to1500_Tune")) xsec = 1117.;
  if(process.Contains("QCD_HT1500to2000_Tune")) xsec = 108.4;
  if(process.Contains("QCD_HT2000toInf_Tune"))  xsec = 21.93;

  // DY // kFactor = 1.23, LO+NLO/LO
  float DY_kFactor = 1.23;
  if(process.Contains("DYJetsToLL_M-50_HT-70to100_Tune"))       xsec = 169.9;
  if(process.Contains("DYJetsToLL_M-50_HT-100to200_Tune"))      xsec = 161.1;
  if(process.Contains("DYJetsToLL_M-50_HT-200to400_Tune"))      xsec = 48.66;
  if(process.Contains("DYJetsToLL_M-50_HT-400to600_Tune"))      xsec = 6.968;
  if(process.Contains("DYJetsToLL_M-50_HT-600to800_Tune"))      xsec = 1.743;
  if(process.Contains("DYJetsToLL_M-50_HT-800to1200_Tune"))     xsec = 0.8052;
  if(process.Contains("DYJetsToLL_M-50_HT-1200to2500_Tune"))    xsec = 0.1933;
  if(process.Contains("DYJetsToLL_M-50_HT-2500toInf_Tune"))     xsec = 0.003468;
  
  // single top 
  // https://twiki.cern.ch/twiki/bin/view/LHCPhysics/SingleTopRefXsec: outdated
  // https://twiki.cern.ch/twiki/bin/view/LHCPhysics/SingleTopNNLORef
  if(process.Contains("ST_s-channel_4f_leptonDecays_Tune"))		xsec = 6.839*3*0.1086;
  if(process.Contains("ST_s-channel_4f_hadronicDecays_Tune"))		xsec = 6.839*0.6742;
  if(process.Contains("ST_s-channel_4f_InclusiveDecays"))		xsec = 6.839;
  if(process.Contains("ST_t-channel_top_4f_InclusiveDecays_Tune"))	xsec = 134.2;
  if(process.Contains("ST_t-channel_antitop_4f_InclusiveDecays_Tune"))	xsec = 80.0;
  if(process.Contains("ST_tW_top_5f_inclusiveDecays_Tune"))		xsec = 39.65; 
  if(process.Contains("ST_tW_antitop_5f_inclusiveDecays_Tune"))		xsec = 39.65;
  
  // diboson 
  // https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns: outdated
  // https://twiki.cern.ch/twiki/bin/viewauth/CMS/StandardModelCrossSectionsat13TeV
  if(process.Contains("WW_Tune"))   								xsec = 118.7;
  if(process.Contains("WZ_Tune") && !process.Contains("WWZ"))   				xsec = 47.13; 
  if(process.Contains("ZZ_Tune") && !process.Contains("ZZZ") && !process.Contains("WZZ"))       xsec = 16.523;
  
  // triboson 
  if(process.Contains("WWW_4F_Tune")) 				       xsec = 0.2086;
  if(process.Contains("WWZ_Tune") || process.Contains("WWZ_4F_Tune"))  xsec = 0.1651; 
  if(process.Contains("WZZ_Tune"))  				       xsec = 0.05565; 
  if(process.Contains("ZZZ_Tune") || process.Contains("ZZZ_4f_Tune"))  xsec = 0.01398; 
 
  // ttV: need inclusive samples
  if(process.Contains("TTZToQQ_Tune"))                xsec = 0.5297;
  if(process.Contains("TTZToLLNuNu_M-10_Tune"))       xsec = 0.2529;
  if(process.Contains("TTWJetsToQQ_Tune"))            xsec = 0.4062;
  if(process.Contains("TTWJetsToLNu_Tune"))           xsec = 0.2043;
//  if(process.Contains("ttHTobb_M125_TuneCP5"))           xsec = 0.3040; //FIXME 221125
  if(process.Contains("ttHJetTobb_M125"))             xsec = 0.2934; //230319
 
  // 4top
  if(process.Contains("TTTT_Tune"))                   xsec = 0.009103;
  
	// RPV signal 
	// Taken from https://twiki.cern.ch/twiki/bin/view/LHCPhysics/SUSYCrossSections13TeVgluglu
	// Note that numbers are different from babymaker 
	// It seems that cross section numbers changed 
  if(process.Contains("mGluino1000"))                  xsec = 0.385E+00;
  if(process.Contains("mGluino1100"))                  xsec = 0.191E+00;
  if(process.Contains("mGluino1200"))                  xsec = 0.985E-01;
  if(process.Contains("mGluino1300"))                  xsec = 0.522E-01;
  if(process.Contains("mGluino1400"))                  xsec = 0.284E-01;
  if(process.Contains("mGluino1500"))                  xsec = 0.157E-01;
  if(process.Contains("mGluino1600"))                  xsec = 0.887E-02;
  if(process.Contains("mGluino1700"))                  xsec = 0.507E-02;
  if(process.Contains("mGluino1800"))                  xsec = 0.293E-02;
  if(process.Contains("mGluino1900"))                  xsec = 0.171E-02;
  if(process.Contains("mGluino2000"))                  xsec = 0.101E-02;
  if(process.Contains("mGluino2100"))                  xsec = 0.598E-03;
  if(process.Contains("mGluino2200"))                  xsec = 0.356E-03;

  //GluGluToNeuNeu
  if(process.Contains("2T2B2S_M-1200"))			xsec = 0.985E-01;
  if(process.Contains("2T2B2S_M-1600"))			xsec = 0.887E-02;
  if(process.Contains("2T2B2S_M-2400"))			xsec = 0.128E-03;
  if(process.Contains("2T2B2S_M-3000"))			xsec = 0.621E-05;

  // StealthSHH_mStop
  if(process.Contains("StealthSHH_2t4b_mStop-300_mSo-100_Tune")) xsec = 0.1E+02;
  if(process.Contains("StealthSHH_2t4b_mStop-350_mSo-100_Tune")) xsec = 0.443E+01;
  if(process.Contains("StealthSHH_2t4b_mStop-400_mSo-100_Tune")) xsec = 0.215E+01;
  if(process.Contains("StealthSHH_2t4b_mStop-450_mSo-100_Tune")) xsec = 0.111E+01;
  if(process.Contains("StealthSHH_2t4b_mStop-500_mSo-100_Tune")) xsec = 0.609E+00;
  if(process.Contains("StealthSHH_2t4b_mStop-550_mSo-100_Tune")) xsec = 0.347E+00;
  if(process.Contains("StealthSHH_2t4b_mStop-600_mSo-100_Tune")) xsec = 0.205E+00;
  if(process.Contains("StealthSHH_2t4b_mStop-650_mSo-100_Tune")) xsec = 0.125E+00;
  if(process.Contains("StealthSHH_2t4b_mStop-700_mSo-100_Tune")) xsec = 0.783E-01;
  if(process.Contains("StealthSHH_2t4b_mStop-750_mSo-100_Tune")) xsec = 0.5E-01;
  if(process.Contains("StealthSHH_2t4b_mStop-800_mSo-100_Tune")) xsec = 0.326E-01;
  if(process.Contains("StealthSHH_2t4b_mStop-850_mSo-100_Tune")) xsec = 0.216E-01;
  if(process.Contains("StealthSHH_2t4b_mStop-900_mSo-100_Tune")) xsec = 0.145E-01;
  if(process.Contains("StealthSHH_2t4b_mStop-950_mSo-100_Tune")) xsec = 0.991E-02;
  if(process.Contains("StealthSHH_2t4b_mStop-1000_mSo-100_Tune")) xsec = 0.683E-02;
  if(process.Contains("StealthSHH_2t4b_mStop-1050_mSo-100_Tune")) xsec = 0.476E-02;
  if(process.Contains("StealthSHH_2t4b_mStop-1100_mSo-100_Tune")) xsec = 0.335E-02;
  if(process.Contains("StealthSHH_2t4b_mStop-1150_mSo-100_Tune")) xsec = 0.238E-02;
  if(process.Contains("StealthSHH_2t4b_mStop-1200_mSo-100_Tune")) xsec = 0.17E-02;
  if(process.Contains("StealthSHH_2t4b_mStop-1250_mSo-100_Tune")) xsec = 0.122E-02;
  if(process.Contains("StealthSHH_2t4b_mStop-1300_mSo-100_Tune")) xsec = 0.887E-03;
  if(process.Contains("StealthSHH_2t4b_mStop-1350_mSo-100_Tune")) xsec = 0.646E-03;
  if(process.Contains("StealthSHH_2t4b_mStop-1400_mSo-100_Tune")) xsec = 0.473E-03;

  cout << "cross section for " << process << " =  " <<  xsec*1000 <<  " fb" << endl; 
  
  return xsec*1000;
}

float getPUweight(TFile *f, TString year, int ntrupv_mean, int syst)
{
  TH1D *w_pu_down, *w_pu_nominal, *w_pu_up;

  w_pu_down = (TH1D*)f->Get("w_pu_down");
  w_pu_nominal = (TH1D*)f->Get("w_pu_nominal");
  w_pu_up = (TH1D*)f->Get("w_pu_up");

  if(ntrupv_mean>=100) ntrupv_mean=99;

  if(syst==-1) return w_pu_down->GetBinContent(ntrupv_mean+1);
  else if(syst==0) return w_pu_nominal->GetBinContent(ntrupv_mean+1);
  else if(syst==1) return w_pu_up->GetBinContent(ntrupv_mean+1);
  else return 1;

}
/*
float getPUweight(TString samplename, TString year, int ntrupv_mean, int syst) // syst=-1 0 1 (down nominal up)
{ 
  vector<float> w_pu_down;
  vector<float> w_pu_nom;
  vector<float> w_pu_up;
  
  vector<TString> bad_pu17_datasets = {"TTJets_SingleLeptFromT",
                       "DYJetsToLL_M-50_HT-1200to2500",
                       "QCD_HT300to500",
                       "QCD_HT700to1000",
                       "TTZToLLNuNu_M-10",
                       "TTWJetsToLNu",
                       "ZZ"};
  bool bad_pu17 = false;

  // obtained with scripts/get_puweights.py
  if(year=="2016"||year=="UL2016_preVFP"||year=="UL2016") {
    w_pu_down  = {3.510e-01, 1.057e+00, 1.197e+00, 1.063e+00, 1.231e+00, 1.306e+00, 9.260e-01, 
      7.621e-01, 1.089e+00, 1.333e+00, 1.486e+00, 1.532e+00, 1.496e+00, 1.501e+00, 1.498e+00, 1.443e+00, 
      1.366e+00, 1.296e+00, 1.224e+00, 1.163e+00, 1.122e+00, 1.094e+00, 1.065e+00, 1.039e+00, 1.019e+00, 
      1.008e+00, 9.968e-01, 9.833e-01, 9.749e-01, 9.592e-01, 9.145e-01, 8.753e-01, 8.075e-01, 7.335e-01, 
      6.515e-01, 5.645e-01, 4.674e-01, 3.758e-01, 2.889e-01, 2.153e-01, 1.504e-01, 1.005e-01, 6.458e-02, 
      3.955e-02, 2.399e-02, 1.386e-02, 7.572e-03, 4.198e-03, 2.220e-03, 1.189e-03, 6.461e-04, 3.844e-04, 
      2.714e-04, 2.435e-04, 2.917e-04, 4.006e-04, 5.506e-04, 7.688e-04, 1.132e-03, 1.466e-03, 2.504e-03, 
      3.381e-03, 3.440e-03, 4.134e-03, 4.689e-03, 3.913e-03, 3.534e-03, 2.909e-03, 2.829e-03, 2.439e-03, 
      1.932e-03, 1.705e-03, 1.486e-03, 1.236e-03, 8.924e-04};
  
    w_pu_nom = {3.388e-01, 8.277e-01, 1.139e+00, 9.310e-01, 1.105e+00, 1.187e+00, 8.008e-01, 
      4.921e-01, 7.396e-01, 8.757e-01, 9.640e-01, 1.075e+00, 1.124e+00, 1.176e+00, 1.203e+00, 1.207e+00, 
      1.199e+00, 1.180e+00, 1.141e+00, 1.094e+00, 1.062e+00, 1.053e+00, 1.052e+00, 1.049e+00, 1.049e+00, 
      1.060e+00, 1.072e+00, 1.081e+00, 1.098e+00, 1.111e+00, 1.094e+00, 1.086e+00, 1.042e+00, 9.848e-01, 
      9.114e-01, 8.254e-01, 7.184e-01, 6.120e-01, 5.038e-01, 4.064e-01, 3.105e-01, 2.293e-01, 1.643e-01, 
      1.131e-01, 7.764e-02, 5.108e-02, 3.192e-02, 2.030e-02, 1.229e-02, 7.443e-03, 4.404e-03, 2.610e-03, 
      1.558e-03, 9.718e-04, 7.345e-04, 6.810e-04, 7.393e-04, 9.318e-04, 1.332e-03, 1.731e-03, 3.006e-03, 
      4.147e-03, 4.321e-03, 5.327e-03, 6.203e-03, 5.317e-03, 4.938e-03, 4.182e-03, 4.186e-03, 3.719e-03, 
      3.035e-03, 2.763e-03, 2.486e-03, 2.135e-03, 1.594e-03};
  
    w_pu_up  = {3.301e-01, 6.518e-01, 1.078e+00, 8.173e-01, 1.005e+00, 1.069e+00, 7.303e-01, 
      3.452e-01, 4.988e-01, 6.007e-01, 6.320e-01, 7.344e-01, 8.268e-01, 9.128e-01, 9.610e-01, 9.885e-01, 
      1.024e+00, 1.051e+00, 1.048e+00, 1.025e+00, 1.002e+00, 9.996e-01, 1.015e+00, 1.036e+00, 1.057e+00, 
      1.087e+00, 1.120e+00, 1.153e+00, 1.195e+00, 1.235e+00, 1.246e+00, 1.272e+00, 1.260e+00, 1.233e+00, 
      1.182e+00, 1.111e+00, 1.005e+00, 8.924e-01, 7.702e-01, 6.564e-01, 5.348e-01, 4.251e-01, 3.310e-01, 
      2.497e-01, 1.894e-01, 1.386e-01, 9.691e-02, 6.935e-02, 4.744e-02, 3.256e-02, 2.182e-02, 1.454e-02, 
      9.534e-03, 6.171e-03, 4.309e-03, 3.109e-03, 2.269e-03, 1.888e-03, 1.988e-03, 2.191e-03, 3.561e-03, 
      4.837e-03, 5.075e-03, 6.358e-03, 7.554e-03, 6.620e-03, 6.292e-03, 5.458e-03, 5.599e-03, 5.100e-03, 
      4.272e-03, 3.993e-03, 3.691e-03, 3.258e-03, 2.501e-03};
  }
  // 2017 weight
  else if(year=="2017"||year=="UL2017") {
    for(size_t i = 0; i < bad_pu17_datasets.size(); i++) 
      if(samplename == bad_pu17_datasets.at(i)) bad_pu17 = true;
    if(bad_pu17) {
      w_pu_down  = {2.165e-04, 6.838e-01, 4.274e-01, 1.983e-01, 2.317e-01, 3.736e-01, 4.795e-01,
        5.454e-01, 3.627e-01, 6.375e-01, 6.994e-01, 6.783e-01, 6.048e-01, 5.205e-01, 5.434e-01, 1.598e+00, 
        1.222e+00, 6.668e-01, 1.551e+00, 1.500e+00, 1.564e+00, 1.490e+00, 1.586e+00, 1.508e+00, 1.535e+00, 
        1.557e+00, 1.481e+00, 1.371e+00, 1.445e+00, 1.392e+00, 1.344e+00, 1.240e+00, 1.126e+00, 1.036e+00, 
        9.828e-01, 9.052e-01, 8.634e-01, 8.075e-01, 7.203e-01, 8.051e-01, 8.622e-01, 8.988e-01, 9.887e-01, 
        1.184e+00, 1.320e+00, 1.483e+00, 1.781e+00, 1.575e+00, 1.459e+00, 1.183e+00, 9.838e-01, 7.386e-01, 
        5.159e-01, 3.667e-01, 2.485e-01, 1.701e-01, 1.074e-01, 6.519e-02, 4.704e-02, 2.938e-02, 2.020e-02, 
        1.299e-02, 9.617e-03, 7.178e-03, 6.009e-03, 4.240e-03, 3.483e-03, 3.461e-03, 2.880e-03, 2.896e-03, 
        1.988e-03, 2.175e-03, 1.807e-03, 1.880e-03, 1.850e-03};
      
      w_pu_nom  = {2.070e-04, 6.181e-01, 3.277e-01, 2.051e-01, 2.010e-01, 3.351e-01, 4.597e-01, 
        4.869e-01, 2.512e-01, 4.290e-01, 4.791e-01, 4.882e-01, 4.627e-01, 3.959e-01, 4.078e-01, 1.185e+00,
        9.225e-01, 5.255e-01, 1.270e+00, 1.268e+00, 1.362e+00, 1.328e+00, 1.437e+00, 1.376e+00, 1.404e+00,
        1.434e+00, 1.384e+00, 1.306e+00, 1.408e+00, 1.386e+00, 1.367e+00, 1.289e+00, 1.198e+00, 1.125e+00,
        1.085e+00, 1.009e+00, 9.671e-01, 9.021e-01, 7.927e-01, 8.585e-01, 8.760e-01, 8.626e-01, 9.005e-01,
        1.043e+00, 1.157e+00, 1.338e+00, 1.704e+00, 1.639e+00, 1.685e+00, 1.535e+00, 1.446e+00, 1.232e+00,
        9.753e-01, 7.815e-01, 5.921e-01, 4.488e-01, 3.104e-01, 2.042e-01, 1.580e-01, 1.047e-01, 7.561e-02,
        5.057e-02, 3.854e-02, 2.930e-02, 2.473e-02, 1.742e-02, 1.419e-02, 1.391e-02, 1.141e-02, 1.133e-02,
        7.722e-03, 8.436e-03, 7.045e-03, 7.424e-03, 7.444e-03};
      
      w_pu_up  = {2.006e-04, 5.340e-01, 2.684e-01, 2.062e-01, 1.732e-01, 2.969e-01, 4.441e-01, 
        4.456e-01, 2.023e-01, 2.784e-01, 3.417e-01, 3.400e-01, 3.514e-01, 3.066e-01, 3.129e-01, 8.969e-01, 
        6.938e-01, 4.049e-01, 1.019e+00, 1.052e+00, 1.162e+00, 1.165e+00, 1.287e+00, 1.251e+00, 1.286e+00,
        1.317e+00, 1.277e+00, 1.220e+00, 1.337e+00, 1.343e+00, 1.352e+00, 1.299e+00, 1.230e+00, 1.180e+00,
        1.162e+00, 1.100e+00, 1.067e+00, 1.003e+00, 8.837e-01, 9.526e-01, 9.563e-01, 9.125e-01, 9.102e-01,
        1.000e+00, 1.058e+00, 1.186e+00, 1.501e+00, 1.477e+00, 1.595e+00, 1.563e+00, 1.612e+00, 1.524e+00,
        1.349e+00, 1.213e+00, 1.031e+00, 8.743e-01, 6.723e-01, 4.880e-01, 4.128e-01, 2.962e-01, 2.295e-01,
        1.632e-01, 1.310e-01, 1.040e-01, 9.090e-02, 6.571e-02, 5.441e-02, 5.374e-02, 4.405e-02, 4.342e-02,
        2.923e-02, 3.151e-02, 2.600e-02, 2.716e-02, 2.715e-02};
    }
    else{
      w_pu_down  = {1.561e-01, 0.000e+00, 3.027e+00, 1.404e+00, 2.696e+00, 1.890e+00, 2.587e+00, 
        1.766e+00, 9.632e-01, 2.078e+00, 2.628e+00, 2.069e+00, 1.549e+00, 1.537e+00, 1.452e+00, 1.459e+00, 
        1.498e+00, 1.423e+00, 1.344e+00, 1.377e+00, 1.321e+00, 1.364e+00, 1.354e+00, 1.360e+00, 1.436e+00,
        1.349e+00, 1.332e+00, 1.345e+00, 1.282e+00, 1.274e+00, 1.213e+00, 1.119e+00, 1.069e+00, 1.026e+00,
        8.585e-01, 8.533e-01, 7.821e-01, 7.792e-01, 7.155e-01, 6.973e-01, 7.114e-01, 8.063e-01, 9.559e-01,
        1.089e+00, 1.278e+00, 1.415e+00, 1.550e+00, 1.444e+00, 1.261e+00, 1.083e+00, 8.727e-01, 7.160e-01,
        5.104e-01, 3.571e-01, 2.403e-01, 1.503e-01, 1.048e-01, 6.315e-02, 4.187e-02, 2.714e-02, 1.787e-02,
        1.304e-02, 9.189e-03, 8.089e-03, 5.361e-03, 4.121e-03, 3.098e-03, 2.749e-03, 2.706e-03, 2.862e-03,
        1.490e-03, 2.780e-03, 2.171e-03, 1.902e-03, 1.872e-03};

      w_pu_nom  = {1.492e-01, 0.000e+00, 2.321e+00, 1.453e+00, 2.339e+00, 1.695e+00, 2.480e+00, 
        1.576e+00, 6.670e-01, 1.398e+00, 1.800e+00, 1.490e+00, 1.185e+00, 1.169e+00, 1.090e+00, 1.082e+00, 
        1.131e+00, 1.121e+00, 1.101e+00, 1.164e+00, 1.150e+00, 1.216e+00, 1.227e+00, 1.241e+00, 1.314e+00, 
        1.243e+00, 1.245e+00, 1.282e+00, 1.249e+00, 1.269e+00, 1.234e+00, 1.163e+00, 1.137e+00, 1.115e+00,
        9.477e-01, 9.515e-01, 8.761e-01, 8.705e-01, 7.875e-01, 7.435e-01, 7.228e-01, 7.738e-01, 8.706e-01,
        9.593e-01, 1.121e+00, 1.277e+00, 1.483e+00, 1.503e+00, 1.456e+00, 1.405e+00, 1.282e+00, 1.194e+00,
        9.649e-01, 7.610e-01, 5.726e-01, 3.967e-01, 3.029e-01, 1.978e-01, 1.406e-01, 9.670e-02, 6.690e-02,
        5.077e-02, 3.683e-02, 3.302e-02, 2.206e-02, 1.694e-02, 1.262e-02, 1.105e-02, 1.072e-02, 1.120e-02,
        5.787e-03, 1.078e-02, 8.464e-03, 7.511e-03, 7.531e-03};

      w_pu_up  = {1.446e-01, 0.000e+00, 1.900e+00, 1.460e+00, 2.015e+00, 1.502e+00, 2.396e+00, 
        1.443e+00, 5.374e-01, 9.075e-01, 1.284e+00, 1.037e+00, 9.000e-01, 9.055e-01, 8.363e-01, 8.189e-01, 
        8.505e-01, 8.637e-01, 8.827e-01, 9.656e-01, 9.817e-01, 1.066e+00, 1.100e+00, 1.129e+00, 1.203e+00, 
        1.141e+00, 1.149e+00, 1.197e+00, 1.186e+00, 1.229e+00, 1.219e+00, 1.172e+00, 1.169e+00, 1.169e+00,
        1.015e+00, 1.037e+00, 9.668e-01, 9.678e-01, 8.778e-01, 8.250e-01, 7.890e-01, 8.186e-01, 8.800e-01,
        9.202e-01, 1.025e+00, 1.132e+00, 1.306e+00, 1.354e+00, 1.378e+00, 1.431e+00, 1.430e+00, 1.477e+00,
        1.334e+00, 1.181e+00, 9.974e-01, 7.728e-01, 6.562e-01, 4.728e-01, 3.674e-01, 2.737e-01, 2.031e-01,
        1.638e-01, 1.252e-01, 1.172e-01, 8.109e-02, 6.387e-02, 4.840e-02, 4.269e-02, 4.138e-02, 4.290e-02,
        2.191e-02, 4.027e-02, 3.124e-02, 2.748e-02, 2.746e-02};
    }
  }
  else if(year=="2018"||year=="UL2018") {
    w_pu_down  = {0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 2.706e+00,
      7.950e+00, 0.000e+00, 2.651e+00, 2.260e+00, 1.887e+00, 2.041e+00, 1.809e+00, 1.553e+00, 2.070e+00,
      2.290e+00, 1.856e+00, 1.550e+00, 1.827e+00, 1.676e+00, 1.556e+00, 1.593e+00, 1.416e+00, 1.441e+00,
      1.319e+00, 1.322e+00, 1.238e+00, 1.311e+00, 1.210e+00, 1.143e+00, 1.094e+00, 1.112e+00, 1.207e+00,
      1.072e+00, 1.120e+00, 9.896e-01, 1.063e+00, 9.803e-01, 8.016e-01, 8.419e-01, 8.090e-01, 8.291e-01,
      5.897e-01, 5.504e-01, 4.440e-01, 4.280e-01, 3.460e-01, 2.786e-01, 2.454e-01, 1.862e-01, 1.675e-01,
      1.183e-01, 1.397e-01, 1.163e-01, 7.591e-02, 1.069e-01, 5.304e-02, 4.329e-02, 3.912e-02, 3.223e-02,
      3.366e-02, 3.096e-02, 4.019e-02, 3.222e-02, 1.633e-02, 2.555e-02, 6.319e-03, 4.830e-03, 1.169e-02,
      0.000e+00, 0.000e+00, 8.106e-04, 1.413e-03, 0.000e+00};
    
    w_pu_nom  = {0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 2.290e+00,
      6.663e+00, 0.000e+00, 2.175e+00, 1.818e+00, 1.496e+00, 1.608e+00, 1.423e+00, 1.223e+00, 1.634e+00, 
      1.819e+00, 1.490e+00, 1.262e+00, 1.516e+00, 1.423e+00, 1.352e+00, 1.414e+00, 1.279e+00, 1.316e+00,
      1.213e+00, 1.219e+00, 1.143e+00, 1.211e+00, 1.120e+00, 1.062e+00, 1.022e+00, 1.048e+00, 1.151e+00,
      1.037e+00, 1.103e+00, 9.978e-01, 1.102e+00, 1.050e+00, 8.908e-01, 9.752e-01, 9.806e-01, 1.055e+00,
      7.904e-01, 7.785e-01, 6.636e-01, 6.762e-01, 5.776e-01, 4.908e-01, 4.552e-01, 3.625e-01, 3.413e-01,
      2.513e-01, 3.084e-01, 2.660e-01, 1.795e-01, 2.611e-01, 1.339e-01, 1.132e-01, 1.061e-01, 9.106e-02,
      9.937e-02, 9.587e-02, 1.310e-01, 1.108e-01, 5.935e-02, 9.830e-02, 2.576e-02, 2.086e-02, 5.346e-02,
      0.000e+00, 0.000e+00, 4.399e-03, 8.116e-03, 0.000e+00}; 
    
    w_pu_up  = {0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 1.961e+00,
      5.639e+00, 0.000e+00, 1.814e+00, 1.489e+00, 1.207e+00, 1.284e+00, 1.132e+00, 9.722e-01, 1.301e+00,
      1.453e+00, 1.198e+00, 1.025e+00, 1.249e+00, 1.193e+00, 1.157e+00, 1.236e+00, 1.140e+00, 1.192e+00,
      1.111e+00, 1.124e+00, 1.057e+00, 1.122e+00, 1.038e+00, 9.850e-01, 9.507e-01, 9.798e-01, 1.083e+00,
      9.840e-01, 1.059e+00, 9.723e-01, 1.093e+00, 1.065e+00, 9.278e-01, 1.047e+00, 1.090e+00, 1.219e+00,
      9.516e-01, 9.801e-01, 8.757e-01, 9.371e-01, 8.417e-01, 7.524e-01, 7.339e-01, 6.141e-01, 6.064e-01,
      4.673e-01, 5.984e-01, 5.368e-01, 3.758e-01, 5.653e-01, 2.992e-01, 2.605e-01, 2.516e-01, 2.226e-01,
      2.508e-01, 2.504e-01, 3.552e-01, 3.129e-01, 1.751e-01, 3.039e-01, 8.363e-02, 7.125e-02, 1.923e-01,
      0.000e+00, 0.000e+00, 1.853e-02, 3.602e-02, 0.000e+00};
  }
  else{
    w_pu_down = {1.0};
    w_pu_nom  = {1.0};
    w_pu_up   = {1.0};
  }

  // protection against accessing nonexisting entry in weight vector 
  if (ntrupv_mean>=w_pu_down.size()) ntrupv_mean = w_pu_down.size()-1; 

  // return weight
  if(syst==-1) return w_pu_down.at(ntrupv_mean);
  else if(syst==0) return w_pu_nom.at(ntrupv_mean);
  else if(syst==1) return w_pu_up.at(ntrupv_mean);
  else return 1;
}
*/
