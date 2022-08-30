OutputBase = '/store/user/mitaylor/PhotonJet'
DatasetName = '/ZeroBias/Run2017G-17Nov2017-v1/AOD'
Tag = '20220825PPZB'

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
config.Data.unitsPerJob = 1
config.Data.publication = False
config.Data.outputDatasetTag = Tag
config.Data.outLFNDirBase = OutputBase + DatasetName
config.Data.lumiMask = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions17/5TeV/ReReco/Cert_306546-306826_5TeV_EOY2017ReReco_Collisions17_JSON.txt'

config.section_("Site")
config.Site.storageSite = 'T2_US_MIT'