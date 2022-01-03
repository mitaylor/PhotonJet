OutputBase = '/store/user/mitaylor/PhotonJet'
DatasetName = '/HighEGJet/Run2017G-UL2017_MiniAODv2-v1/MINIAOD'
Tag = '20220102PPEG'

from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = Tag

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'runConfig_forest_miniAOD_106X_EG.py'
config.JobType.maxMemoryMB = 5000
config.JobType.maxJobRuntimeMin = 1500

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