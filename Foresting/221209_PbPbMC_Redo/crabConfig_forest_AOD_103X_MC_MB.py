# OutputBase = '/store/user/mitaylor/PhotonJet'
OutputBase = '/store/group/phys_heavyions/mitaylor/PhotonJet'
DatasetName = '/MinBias_Hydjet_Drum5F_2018_5p02TeV/HINPbPbAutumn18DR-NoPUmva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM'
Tag = '20230204PbPbMCMB'

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
config.Data.outputDatasetTag = Tag
config.Data.outLFNDirBase = OutputBase + DatasetName

config.section_("Site")
config.Site.storageSite = 'T2_US_MIT'