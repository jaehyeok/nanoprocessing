#!/bin/bash

for mass in 1000 1100 1200 1300 1400 1500 1600 1700 1800 1900 2000 2100
do
	python gen_btagEff.py -p signal_m${mass}
done
