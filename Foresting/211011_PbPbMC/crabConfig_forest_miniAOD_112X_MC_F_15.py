OutputBase = '/store/user/mitaylor/PhotonJet'
DatasetName = '/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM'
Tag = '20230127PbPbMC30F15'

from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = Tag

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'runConfig_forest_miniAOD_112X_MC.py'
config.JobType.maxMemoryMB = 5000
config.JobType.maxJobRuntimeMin = 1500   

config.section_("Data")
config.Data.inputDataset = DatasetName
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.publication = False
config.Data.lumiMask = '/afs/cern.ch/user/m/mitaylor/physics/PhotonJet2018/Foresting/211011_PbPbMC/30F15_remains.json'
config.Data.outputDatasetTag = Tag
config.Data.outLFNDirBase = OutputBase + DatasetName

config.section_("Site")
config.Site.storageSite = 'T2_US_MIT'