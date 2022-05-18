

default: TestRun

RunAll: Execute
	bash RunAll.sh

TestRun: Execute
	mkdir -p Output
	./Execute --Data unfolding/data/accumulate_aa.root \
		--DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
		--Response unfolding/response_matrices/vacillate_aa_dr.root \
		--ResponseHistogram aa_c_1 \
		--ResponseTruth aa_g_1 \
		--ResponseReco aa_r_1 \
		--PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
		--BinningRecoBins 30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
		--PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
		--BinningGenBins 10,25,30,40,50,60,70,80,100,120,200 \
		--Output Output/AATest.root

Execute: PickHistograms.cpp
	g++ PickHistograms.cpp `root-config --cflags --glibs` -I. -o Execute
