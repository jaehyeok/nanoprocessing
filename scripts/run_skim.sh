#######  ./scripts/run_skim.sh [year]
for x in {0,1,2,3,4,5,6,7,8,9};
#for x in {0,1};
do ./skim.exe /xrootd_user/yjeong/xrootd/nanoprocessing/$1/processed_1211 rpvfitnbge0 $x > log_skim_$x.txt &
done

