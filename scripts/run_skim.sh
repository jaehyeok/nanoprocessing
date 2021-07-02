#######  ./scripts/run_skim.sh [year]
mkdir -p log/
for x in {0,1,2,3,4};
do ./skim.exe /xrootd_user/yjeong/xrootd/nanoprocessing/$1/processed_deepTvsQCD rpvfitnbge0 $x > log/log_skim_$x.txt &
done

