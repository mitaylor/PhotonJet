default: Execute ExecuteTheory

Execute: PickHistograms.cpp
	g++ PickHistograms.cpp `root-config --cflags --glibs` -I. -o Execute

ExecuteTheory: PickHistogramsTheory.cpp
	g++ PickHistogramsTheory.cpp `root-config --cflags --glibs` -I. -o ExecuteTheory