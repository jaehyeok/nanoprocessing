#######  ./scripts/run_skim.sh [year]
for x in {0,1,2,3,4,5,6,7,8,9};
do ./skim.exe /xrootd_user/yjeong/xrootd/nanoprocessing/$1/test_HEM rpvfitnbge0 $x > log_skim_$x.txt &
done

