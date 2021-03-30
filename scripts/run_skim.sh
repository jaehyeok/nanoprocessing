#######  ./scripts/run_skim.sh [year]
mkdir -p log/
#for x in {0,1,2,3,4,5,6,7,8,9};
for x in {0,1,2,3,4};
do ./skim.exe /xrootd_user/yjeong/xrootd/nanoprocessing/$1/processed_1202 rpvfitnbge0 $x > log/log_skim_$x.txt &
done

