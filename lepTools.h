using namespace std; 

//https://github.com/richstu/nano2pico/blob/master/src/el_producer.cpp#L85-L105
bool idElectron_noIso(int bitmap, int level){
	// decision for each cut represented by 3 bits (0:fail, 1:veto, 2:loose, 3:medium, 4:tight)
	// Electron_vidNestedWPBitmap 
	//0 - MinPtCut
	//1 - GsfEleSCEtaMultiRangeCut
	//2 - GsfEleDEtaInSeedCut
	//3 - GsfEleDPhiInCut
	//4 - GsfEleFull5x5SigmaIEtaIEtaCut
	//5 - GsfEleHadronicOverEMEnergyScaledCut
	//6 - GsfEleEInverseMinusPInverseCut
	//7 - GsfEleRelPFIsoScaledCut
	//8 - GsfEleConversionVetoCut
	//9 - GsfEleMissingHitsCut
	bool pass = true;
	// cout<<std::bitset<8*sizeof(bitmap)>(bitmap)<<endl;
	for (int i(0); i<10; i++){
		if (i==7) continue;
		if ( ((bitmap >> i*3) & 0x7) < level) pass = false;
	}
	return pass;
}

