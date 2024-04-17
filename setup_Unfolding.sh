source /cvmfs/cms.cern.ch/cmsset_default.sh
cmssw-el7
cd ~/cmssw/CMSSW_11_2_1_patch2/src/
eval `scramv1 runtime -sh`
cd ~/physics/PhotonJet2018/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/submit/mitay/physics/PhotonJet2018/Unfolding/Unfolding/
