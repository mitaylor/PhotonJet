#!/usr/bin/env bash

Samples=(PPNominal PPWithoutEleRej PPTightPurity PPLoosePurity PPQCD\
				PPQCDGenIso PPJEUUp PPJEUDown PPJERUp)

for Sample in ${Samples[@]}; do
    echo "${Sample}0"
    ./Execute --Input Input/${Sample}0.root --Output Output/${Sample}0.root \
        --Prior MC --DoBayes true --DoSVD false --DoInvert true --DoTUnfold false --DoFit true
done

Samples=(AANominal AAWithoutEleRej AATightPurity AALoosePurity AAQCD\
				AAQCDGenIso AAJEUUp AAJEUDown AAJERUp AACentUp AACentDown)

for Sample in ${Samples[@]}; do
    echo "${Sample}0"
    ./Execute --Input Input/${Sample}0.root --Output Output/${Sample}0.root \
        --Prior MC --DoBayes true --DoSVD false --DoInvert true --DoTUnfold false --DoFit true
    echo "${Sample}1"
    ./Execute --Input Input/${Sample}1.root --Output Output/${Sample}1.root \
        --Prior MC --DoBayes true --DoSVD false --DoInvert true --DoTUnfold false --DoFit true
    echo "${Sample}2"
    ./Execute --Input Input/${Sample}2.root --Output Output/${Sample}2.root \
        --Prior MC --DoBayes true --DoSVD false --DoInvert true --DoTUnfold false --DoFit true
    echo "${Sample}3"
    ./Execute --Input Input/${Sample}3.root --Output Output/${Sample}3.root \
        --Prior MC --DoBayes true --DoSVD false --DoInvert true --DoTUnfold false --DoFit true
done