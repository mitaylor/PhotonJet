#!/bin/sh


Number=0
Input=file:/eos/user/m/mitaylor/local_copies/HIRun2018A/HIHardProbes/AOD/04Apr2019-v1/260002/54E23773-F56C-A64F-A553-2F626B39E2E9.root

cat SubmitAOD_template.condor | sed "s#__INPUT__#$Input#; s#__NUMBER__#$Number#" > SubmitAOD_$Number.condor
condor_submit SubmitAOD_$Number.condor


Number=1
Input=file:/eos/user/m/mitaylor/local_copies/HIRun2018A/HIHardProbes/AOD/04Apr2019-v1/260002/5DBF41FC-309A-A343-8D3A-33BE46871413.root

cat SubmitAOD_template.condor | sed "s#__INPUT__#$Input#; s#__NUMBER__#$Number#" > SubmitAOD_$Number.condor
condor_submit SubmitAOD_$Number.condor


Number=2
Input=file:/eos/user/m/mitaylor/local_copies/HIRun2018A/HIHardProbes/AOD/04Apr2019-v1/260002/61114CBB-5252-C84E-AC1C-6FFD62DE928E.root

cat SubmitAOD_template.condor | sed "s#__INPUT__#$Input#; s#__NUMBER__#$Number#" > SubmitAOD_$Number.condor
condor_submit SubmitAOD_$Number.condor


Number=3
Input=file:/eos/user/m/mitaylor/local_copies/HIRun2018A/HIHardProbes/AOD/04Apr2019-v1/260002/74A8A880-041A-0D4E-BD59-5E3C169B3A84.root

cat SubmitAOD_template.condor | sed "s#__INPUT__#$Input#; s#__NUMBER__#$Number#" > SubmitAOD_$Number.condor
condor_submit SubmitAOD_$Number.condor

Number=4
Input=file:/eos/user/m/mitaylor/local_copies/HIRun2018A/HIHardProbes/AOD/04Apr2019-v1/260002/9CB509B5-DE26-6143-948C-4888109AFD23.root

cat SubmitAOD_template.condor | sed "s#__INPUT__#$Input#; s#__NUMBER__#$Number#" > SubmitAOD_$Number.condor
condor_submit SubmitAOD_$Number.condor

Number=5
Input=file:/eos/user/m/mitaylor/local_copies/HIRun2018A/HIHardProbes/AOD/04Apr2019-v1/260002/FFDC785D-A48B-3143-8556-7AF159B986E7.root

cat SubmitAOD_template.condor | sed "s#__INPUT__#$Input#; s#__NUMBER__#$Number#" > SubmitAOD_$Number.condor
condor_submit SubmitAOD_$Number.condor