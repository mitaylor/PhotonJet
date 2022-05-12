### HiForest Configuration
# Collisions: pp
# Type: MC
# Input: AOD

import FWCore.ParameterSet.Config as cms
from Configuration.Eras.Era_Run2_2017_ppRef_cff import Run2_2017_ppRef
process = cms.Process('HiForest',Run2_2017_ppRef)
process.options = cms.untracked.PSet()

#####################################################################################
# HiForest labelling info
#####################################################################################

process.load("HeavyIonsAnalysis.JetAnalysis.HiForest_cff")
process.HiForest.inputLines = cms.vstring("HiForest 106X",)
import subprocess, os
version = subprocess.check_output(['git',
    '-C', os.path.expandvars('$CMSSW_BASE/src'), 'describe', '--tags'])
if version == '':
    version = 'no git info'
process.HiForest.HiForestVersion = cms.string(version)

#####################################################################################
# Input source
#####################################################################################

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
	'/store/himc/RunIISummer20UL17pp5TeVRECO/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_5p02TeV-pythia8/AODSIM/106X_mc2017_realistic_forppRef5TeV_v3-v1/2530000/17D05C7E-121B-6C45-B2BF-A7C7715C54CF.root'
    )
)

# Number of events we want to process, -1 = all events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(8))

#####################################################################################
# Load Global Tag, Geometry, etc.
#####################################################################################

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.Geometry.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_mc2017_realistic_forppRef5TeV_v3', '')
process.HiForest.GlobalTagLabel = process.GlobalTag.globaltag

process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("BTagTrackProbability3DRcd"),
             tag = cms.string("JPcalib_MC94X_2017pp_v2"),
             connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")

         )
      ])

#####################################################################################
# Define tree output
#####################################################################################

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("HiForestAOD.root"))

#####################################################################################
# Additional Reconstruction and Analysis: Main Body
#####################################################################################

#############################
# Jets
#############################
process.load("HeavyIonsAnalysis.JetAnalysis.fullJetSequence_pp_mc_cff")
process.ak3PFJetAnalyzer.doWTARecluster = cms.untracked.bool(True)

# Use this version for JEC
# process.load("HeavyIonsAnalysis.JetAnalysis.fullJetSequence_pp_jec_cff")

from HeavyIonsAnalysis.Configuration.CommonFunctions_cff import overrideJEC_MC_pp5020_2017
process = overrideJEC_MC_pp5020_2017(process)
#####################################################################################

############################
# Event Analysis
############################
# use data version to avoid PbPb MC
process.load('HeavyIonsAnalysis.EventAnalysis.hievtanalyzer_data_cfi')
process.hiEvtAnalyzer.Vertex = cms.InputTag("offlinePrimaryVertices")
process.hiEvtAnalyzer.doCentrality = cms.bool(False)
process.hiEvtAnalyzer.doEvtPlane = cms.bool(False)
process.hiEvtAnalyzer.doMC = cms.bool(True) # general MC info
process.hiEvtAnalyzer.doHiMC = cms.bool(False) # HI specific MC info

process.load('HeavyIonsAnalysis.EventAnalysis.hltanalysis_cff')
process.load('HeavyIonsAnalysis.EventAnalysis.hltobject_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.l1object_cfi')

process.load('HeavyIonsAnalysis.JetAnalysis.HiGenAnalyzer_cfi')
process.HiGenParticleAna.genParticleSrc = cms.untracked.InputTag("genParticles")
process.HiGenParticleAna.doHI = False

process.load('HeavyIonsAnalysis.EventAnalysis.runanalyzer_cff')

process.load("HeavyIonsAnalysis.JetAnalysis.pfcandAnalyzer_pp_cfi")
process.pfcandAnalyzer.skipCharged      = False
process.pfcandAnalyzer.pfPtMin          = 0
process.pfcandAnalyzer.pfCandidateLabel = cms.InputTag("particleFlow")
process.pfcandAnalyzer.doVS             = cms.untracked.bool(False)
process.pfcandAnalyzer.doUEraw_         = cms.untracked.bool(False)
process.pfcandAnalyzer.genLabel         = cms.InputTag("genParticles")

#####################################################################################

#########################
# Track Analyzer
#########################
process.load('HeavyIonsAnalysis.JetAnalysis.ExtraTrackReco_cff')
process.load('HeavyIonsAnalysis.JetAnalysis.TrkAnalyzers_cff')

# Use this instead for track corrections
# process.load('HeavyIonsAnalysis.JetAnalysis.TrkAnalyzers_Corr_cff')

#####################################################################################

#####################
# photons
######################
process.load('HeavyIonsAnalysis.PhotonAnalysis.ggHiNtuplizer_cfi')
process.ggHiNtuplizerGED.doEffectiveAreas = cms.bool(True)
process.ggHiNtuplizerGED.doPhoERegression = cms.bool(True)

####################################################################################

#####################
# Electron ID
#####################

from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
# turn on VID producer, indicate data format to be processed
# DataFormat.AOD or DataFormat.MiniAOD
dataFormat = DataFormat.AOD
switchOnVIDElectronIdProducer(process, dataFormat)

# define which IDs we want to produce. https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2#Recipe_for_regular_users_for_7_4
my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Spring15_25ns_V1_cff']

# add them to the VID producer
for idmod in my_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)

#####################################################################################

#########################
# Main analysis list
#########################

process.ana_step = cms.Path(
    process.hltanalysis *
    process.hiEvtAnalyzer *
    process.hltobject +
    process.l1object +
    process.HiGenParticleAna*
    process.genJetSequence +
    process.jetSequence +
    # Should be added in the path for VID module
    # process.egmGsfElectronIDSequence +
    process.ggHiNtuplizer +
    process.ggHiNtuplizerGED +
    # process.pfcandAnalyzer +
    process.HiForest +
    # process.trackSequencesPP +
    process.runAnalyzer
)

#####################################################################################

#########################
# Event Selection
#########################

process.load('HeavyIonsAnalysis.JetAnalysis.EventSelection_cff')
process.pHBHENoiseFilterResultProducer = cms.Path(process.HBHENoiseFilterResultProducer)
process.HBHENoiseFilterResult = cms.Path(process.fHBHENoiseFilterResult)
process.HBHENoiseFilterResultRun1 = cms.Path(process.fHBHENoiseFilterResultRun1)
process.HBHENoiseFilterResultRun2Loose = cms.Path(process.fHBHENoiseFilterResultRun2Loose)
process.HBHENoiseFilterResultRun2Tight = cms.Path(process.fHBHENoiseFilterResultRun2Tight)
process.HBHEIsoNoiseFilterResult = cms.Path(process.fHBHEIsoNoiseFilterResult)

process.PAprimaryVertexFilter = cms.EDFilter("VertexSelector",
    src = cms.InputTag("offlinePrimaryVertices"),
    cut = cms.string("!isFake && abs(z) <= 25 && position.Rho <= 2 && tracksSize >= 2"),
    filter = cms.bool(True), # otherwise it won't filter the events
)

process.NoScraping = cms.EDFilter("FilterOutScraping",
    applyfilter = cms.untracked.bool(True),
    debugOn = cms.untracked.bool(False),
    numtrack = cms.untracked.uint32(10),
    thresh = cms.untracked.double(0.25)
)

process.pPAprimaryVertexFilter = cms.Path(process.PAprimaryVertexFilter)
process.pBeamScrapingFilter=cms.Path(process.NoScraping)

process.load("HeavyIonsAnalysis.VertexAnalysis.PAPileUpVertexFilter_cff")
process.pVertexFilterCutG = cms.Path(process.pileupVertexFilterCutG)
process.pVertexFilterCutGloose = cms.Path(process.pileupVertexFilterCutGloose)
process.pVertexFilterCutGtight = cms.Path(process.pileupVertexFilterCutGtight)
process.pVertexFilterCutGplus = cms.Path(process.pileupVertexFilterCutGplus)
process.pVertexFilterCutE = cms.Path(process.pileupVertexFilterCutE)
process.pVertexFilterCutEandG = cms.Path(process.pileupVertexFilterCutEandG)

process.pAna = cms.EndPath(process.skimanalysis)

# Customization
# KT : filter events on reco photons
# photon selection
process.selectedPhotons = cms.EDFilter("PhotonSelector",
    src = cms.InputTag("gedPhotons"),
    cut = cms.string('abs(eta) < 2.5')
)

# leading photon E_T filter
process.photonFilter = cms.EDFilter("EtMinPhotonCountFilter",
    src = cms.InputTag("selectedPhotons"),
    etMin = cms.double(25.0),
    minNumber = cms.uint32(1)
)

process.photonFilterSequence = cms.Sequence(process.selectedPhotons *
                                            process.photonFilter
)

# needed to apply the filter on skimanalysis tree
process.superFilterPath = cms.Path(process.photonFilterSequence)
process.skimanalysis.superFilters = cms.vstring("superFilterPath")

for path in process.paths:
  getattr(process,path)._seq = process.photonFilterSequence * getattr(process,path)._seq