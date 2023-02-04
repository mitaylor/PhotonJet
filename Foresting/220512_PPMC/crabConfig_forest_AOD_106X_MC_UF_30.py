# OutputBase = '/store/user/mitaylor/PhotonJet'
OutputBase = '/store/group/phys_heavyions/mitaylor/PhotonJet'
DatasetName = '/QCDPhoton_pThat-30_TuneCP5_5p02TeV-pythia8/RunIISummer20UL17pp5TeVRECO-106X_mc2017_realistic_forppRef5TeV_v3-v2/AODSIM'
Tag = '20230204PPMUF30'

from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = Tag

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'runConfig_forest_AOD_106X_MC.py'
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
# config.Site.storageSite = 'T2_US_MIT'
config.Site.storageSite = 'T2_CH_CERN'
config.Site.ignoreGlobalBlacklist = True
config.Site.whitelist = ['T2_US_Vanderbilt']