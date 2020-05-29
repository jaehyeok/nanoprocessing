#for tag in {SMS-T1tbs_RPV_mGluino,QCD_HT,ST_,DYJets,WJets,WW,WZ,ZZ,TTJets,TTZ};
for tag in {SMS-T1tbs_RPV_mGluino,QCD_HT,ST_};
#for tag in {DYJetsToLL_M-50_HT-1200to2500,DYJetsToLL_M-50_HT-2500toInf,DYJetsToLL_M-50_HT-400to600,DYJetsToLL_M-50_HT-600to800,DYJetsToLL_M-50_HT-800to1200};
#for tag in {WJets,WW,WZ,ZZ};
#for tag in {TTW,TTZ};
do echo ${tag}
./norm_weights.exe /xrootd_user/yjeong/xrootd/nanoprocessing/2018/merged_rpvfitnbge0/ ${tag} /xrootd_user/yjeong/xrootd/nanoprocessing/2018/merged_norm/ /xrootd_user/yjeong/xrootd/nanoprocessing/2018/processed/ > norm_log_${tag}.txt &
done
