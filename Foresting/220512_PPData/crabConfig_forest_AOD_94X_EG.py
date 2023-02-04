# OutputBase = '/store/user/mitaylor/PhotonJet'
OutputBase = '/store/group/phys_heavyions/mitaylor/PhotonJet'
DatasetName = '/HighEGJet/Run2017G-17Nov2017-v2/AOD'
Tag = '20230204PPEG'

from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = Tag

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'runConfig_forest_AOD_94X_EG.py'
config.JobType.maxMemoryMB = 5000
config.JobType.maxJobRuntimeMin = 2750

config.section_("Data")
config.Data.inputDataset = DatasetName
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 5
config.Data.publication = False
config.Data.outputDatasetTag = Tag
config.Data.outLFNDirBase = OutputBase + DatasetName

config.section_("Site")
# config.Site.storageSite = 'T2_US_MIT'
config.Site.storageSite = 'T2_CH_CERN'
config.Site.ignoreGlobalBlacklist = True