#!/usr/bin/env bash

set=$1


types=(populate_aa populate_aa_cent_down populate_aa_cent_up populate_aa_closure populate_aa_es populate_aa_no_eff populate_aa_no_er populate_aa_qcd populate_aa_qcd_gen_iso populate_aa_wo_ele_rej populate_pp populate_pp_closure populate_pp_er populate_pp_es populate_pp_qcd populate_pp_qcd_gen_iso populate_pp_wo_ele_rej)

for type in ${type[@]}; do
    echo ${type}
    
    mkdir /data/submit/mitay/populate/arc/pho_60_rebin1/{$type}_raw
    mkdir /data/submit/mitay/populate/arc/pho_60_rebin2/{$type}_raw
    mkdir /data/submit/mitay/populate/arc/pho_60_rebin3/{$type}_raw
    mkdir /data/submit/mitay/populate/arc/pho_60_rebin4/{$type}_raw

    mkdir /data/submit/mitay/populate/arc/pho_60_rebin1/{$type}_bkg
    mkdir /data/submit/mitay/populate/arc/pho_60_rebin2/{$type}_bkg
    mkdir /data/submit/mitay/populate/arc/pho_60_rebin3/{$type}_bkg
    mkdir /data/submit/mitay/populate/arc/pho_60_rebin4/{$type}_bkg

    mv /data/submit/mitay/populate/arc/pho_60_rebin1/{$type}_raw_* /data/submit/mitay/populate/arc/pho_60_rebin1/{$type}_raw
    mv /data/submit/mitay/populate/arc/pho_60_rebin2/{$type}_raw_* /data/submit/mitay/populate/arc/pho_60_rebin2/{$type}_raw
    mv /data/submit/mitay/populate/arc/pho_60_rebin3/{$type}_raw_* /data/submit/mitay/populate/arc/pho_60_rebin3/{$type}_raw
    mv /data/submit/mitay/populate/arc/pho_60_rebin4/{$type}_raw_* /data/submit/mitay/populate/arc/pho_60_rebin4/{$type}_raw

    mv /data/submit/mitay/populate/arc/pho_60_rebin1/{$type}_bkg_* /data/submit/mitay/populate/arc/pho_60_rebin1/{$type}_bkg
    mv /data/submit/mitay/populate/arc/pho_60_rebin2/{$type}_bkg_* /data/submit/mitay/populate/arc/pho_60_rebin2/{$type}_bkg
    mv /data/submit/mitay/populate/arc/pho_60_rebin3/{$type}_bkg_* /data/submit/mitay/populate/arc/pho_60_rebin3/{$type}_bkg
    mv /data/submit/mitay/populate/arc/pho_60_rebin4/{$type}_bkg_* /data/submit/mitay/populate/arc/pho_60_rebin4/{$type}_bkg
done
