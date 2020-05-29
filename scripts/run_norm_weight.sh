#for tag in {SMS-T1tbs_RPV_mGluino,DYJetsToLL_M-50,QCD_HT,ST_,TT,WJets,WW,WZ,ZZ};
for tag in {SMS-T1tbs_RPV_mGluino,DYJetsToLL_M-50,QCD_HT,ST_};
do echo ${tag}
./norm_weights.exe /xrootd_user/yjeong/xrootd/nanoprocessing/2017/merged_rpvfitnbge0/ ${tag} /xrootd_user/yjeong/xrootd/nanoprocessing/2017/merged_norm /xrootd_user/yjeong/xrootd/nanoprocessing/2017/processed > norm_log_${tag}.txt &
done
