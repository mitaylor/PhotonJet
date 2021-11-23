# OutputBase = '/store/group/phys_heavyions/mitaylor'
OutputBase = '/store/user/mitaylor/'
DatasetName = '/QCDPhoton_pThat-170_Filter30GeV_TuneCP5_5p02TeV_pythia8/RunIIpp5Spring18DR-94X_mc2017_realistic_forppRef5TeV_v1-v1/AODSIM'
Tag = 'QCDPhoton_pThat-170_Filter30GeV_TuneCP5_5p02TeV_pythia8_AOD_944_v3'

from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = Tag

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'runConfig_forest_AOD_94X_MC.py'
config.JobType.maxMemoryMB = 2500    # request high memory machines.
config.JobType.maxJobRuntimeMin = 2750    # request longer runtime, ~48 hours.

config.section_("Data")
config.Data.inputDataset = DatasetName
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.publication = False
config.Data.outputDatasetTag = Tag
config.Data.outLFNDirBase = OutputBase + DatasetName + '/'

config.section_("Site")
# config.Site.storageSite = "T2_CH_CERN"
config.Site.storageSite = 'T2_US_MIT'