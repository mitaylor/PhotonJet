default: RunTest

Prepare:
	rm -f Input
	ln -sf ../UnfoldingPreparation/Output/ Input
	mkdir -p Output Plots

DeepClean: Clean
	rm -rf RooUnfold

Clean:
	rm -f Output/*root
	rm -f Plots/*pdf

SetupPrerequisites: SetupRooUnfoldMake

SetupRooUnfoldMake:
	git clone ssh://git@gitlab.cern.ch:7999/RooUnfold/RooUnfold.git
	(cd RooUnfold/ && make -j4)

SetupRooUnfoldCMake:
	git clone ssh://git@gitlab.cern.ch:7999/RooUnfold/RooUnfold.git
	(cd RooUnfold/ && mkdir -p build && cd build/ && cmake .. && make -j4)

# If you use the make version
Execute: Unfold1D.cpp
	g++ Unfold1D.cpp -o Execute \
		-IRooUnfold/src -LRooUnfold/ -lRooUnfold \
		`root-config --cflags --glibs`

# If you use the CMake version
# Execute: Unfold1D.cpp
# 	g++ Unfold1D.cpp -o Execute \
# 		-IRooUnfold/build -LRooUnfold/build -lRooUnfold \
# 		`root-config --cflags --glibs`

RunTest: Execute
	./Execute --Input Input/PPNominal0.root --Output Output/PPNominal0.root \
		--Prior MC --DoBayes true --DoSVD false --DoInvert true --DoTUnfold false --DoFit true

RunAll: Execute
	./RunAll.sh