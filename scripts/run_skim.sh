#######  ./scripts/run_skim.sh [year]
mkdir -p log/
for x in {0,1,2,3,4};
do ./skim.exe /data2/nanoprocessing/$1/processed_0317 rpvfitnbge0 $x > log/log_skim_$x.txt &
done

