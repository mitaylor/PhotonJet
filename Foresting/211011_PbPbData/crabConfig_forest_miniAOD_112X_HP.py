# OutputBase = '/store/group/phys_heavyions/mitaylor'
OutputBase = '/store/user/pchou/'
DatasetName = '/HIHardProbes/HIRun2018A-PbPb18_MiniAODv1-v1/MINIAOD'
Tag = 'HIHardProbes_HIRun2018A-PbPb18_MiniAODv1-v1'

from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = Tag

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'forest_miniAOD_112X_DATA.py'
config.JobType.maxMemoryMB = 5000    # request high memory machines.
config.JobType.maxJobRuntimeMin = 1500    # request longer runtime, ~48 hours. 2750

config.section_("Data")
config.Data.inputDataset = DatasetName
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.publication = False
config.Data.outputDatasetTag = Tag
config.Data.outLFNDirBase = OutputBase + DatasetName

config.section_("Site")
# config.Site.storageSite = "T2_CH_CERN"
config.Site.storageSite = 'T2_US_MIT'
