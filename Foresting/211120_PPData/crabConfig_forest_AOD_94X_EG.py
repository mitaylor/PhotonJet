# OutputBase = '/store/group/phys_heavyions/mitaylor'
OutputBase = '/store/user/mitaylor/'
DatasetName = '/HighEGJet/Run2017G-09Aug2019_UL2017-v1/AOD'
Tag = 'HighEGJet_Run2017G-09Aug2019_UL2017-v1_v1'

from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = Tag

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'runConfig_forest_AOD_94X_EG.py'
config.JobType.maxMemoryMB = 2500    # request high memory machines.
config.JobType.maxJobRuntimeMin = 2750    # request longer runtime, ~48 hours.

config.section_("Data")
config.Data.inputDataset = DatasetName
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 5
config.Data.publication = False
config.Data.outputDatasetTag = Tag
config.Data.outLFNDirBase = OutputBase + DatasetName + '/'

config.section_("Site")
# config.Site.storageSite = "T2_CH_CERN"
config.Site.storageSite = 'T2_US_MIT'