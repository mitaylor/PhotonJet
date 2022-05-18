

default: TestRun

RunAll: Execute
	bash RunAll.sh

TestRun: Execute
	mkdir -p Output
	./Execute --Data ../../MainAnalysis/data/jet_20/accumulate_aa.root \
		--DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
		--Response ../../MainAnalysis/data/jet_20/vacillate_aa.root \
		--ResponseHistogram aa_c_0 \
		--ResponseTruth aa_g_0 \
		--ResponseReco aa_r_0 \
		--PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
		--BinningRecoBins 30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
		--PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
		--BinningGenBins 10,20,30,40,50,60,70,80,100,120,200 \
		--Output Output/AANominal0.root

Execute: PickHistograms.cpp
	g++ PickHistograms.cpp `root-config --cflags --glibs` -I. -o Execute