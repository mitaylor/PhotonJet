OutputBase = '/store/user/mitaylor/PhotonJet'
DatasetName = '/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-FixL1CaloGT_103X_upgrade2018_realistic_HI_v13_ext1-v2/AODSIM'
Tag = '20230109PbPbMCExtra30F120'

from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = Tag

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'runConfig_forest_AOD_103X_MC.py'
config.JobType.maxMemoryMB = 5000
config.JobType.maxJobRuntimeMin = 2750   

config.section_("Data")
config.Data.inputDataset = DatasetName
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.publication = False
config.Data.lumiMask = '/afs/cern.ch/user/m/mitaylor/physics/PhotonJet2018/Foresting/221209_PbPbMC_Redo/extra30F120_remains.json'
config.Data.outputDatasetTag = Tag
config.Data.outLFNDirBase = OutputBase + DatasetName

config.section_("Site")
config.Site.storageSite = 'T2_US_MIT'