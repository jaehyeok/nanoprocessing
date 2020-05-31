#for tag in {SMS-T1tbs_RPV_mGluino,QCD,ST_,DYJets,WJets,WW,WZ,ZZ,TTJets,TTZ,TTTT};
#for tag in {SMS-T1tbs_RPV_mGluino,QCD,ST_,DYJets};
for tag in {WJets,TTTT};
do echo ${tag}
./norm_weights.exe /xrootd_user/yjeong/xrootd/nanoprocessing/2018/merged_rpvfitnbge0/ ${tag} /xrootd_user/yjeong/xrootd/nanoprocessing/2018/merged_norm/ /xrootd_user/yjeong/xrootd/nanoprocessing/2018/processed/ > log_norm_${tag}.txt &
done
