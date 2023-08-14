#!/bin/sh
#SBATCH -J PU_WEIGHT_UL2016
#SBATCH -N 1
#SBATCH -n 1
#SBATCH -o /home/bjhong/nanoprocessing/out_%x.o%j.txt
#SBATCH -e /home/bjhong/nanoprocessing/err_%x.e%j.txt
cd /home/bjhong/nanoprocessing
echo $PWD
srun ./get_puweights.sh UL2016
