#!/bin/sh

g++ test_btag.cpp JetCorrectorParameters.cpp FactorizedJetCorrector.cpp SimpleJetCorrector.cpp JetCorrectionUncertainty.cpp SimpleJetCorrectionUncertainty.cpp BTagCalibrationStandalone.cpp `fastjet-config --cxxflags --libs --plugins` `root-config --cflags --glibs` -o test_btag.exe
