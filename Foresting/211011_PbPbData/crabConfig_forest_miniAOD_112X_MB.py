OutputBase = '/store/user/mitaylor/PhotonJet'
DatasetName = '/HIMinimumBias4/HIRun2018A-PbPb18_MiniAODv1-v1/MINIAOD'
Tag = '20220215PbPbMB'

from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = Tag

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'runConfig_forest_miniAOD_112X_DATA_MB.py'
config.JobType.maxMemoryMB = 5000    # request high memory machines. 2500.
config.JobType.maxJobRuntimeMin = 1500

config.section_("Data")
config.Data.inputDataset = DatasetName
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 5 # may have to adjust
config.Data.publication = False
config.Data.outputDatasetTag = Tag
config.Data.outLFNDirBase = OutputBase + DatasetName
config.Data.lumiMask = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions18/HI/PromptReco/Cert_326381-327564_HI_PromptReco_Collisions18_JSON.txt'

config.section_("Site")
config.Site.storageSite = 'T2_US_MIT'
