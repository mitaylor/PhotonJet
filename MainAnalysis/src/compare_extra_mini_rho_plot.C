#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TH1F.h"
#include "TLatex.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int narrate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto file = conf->get<std::string>("file");

    TH1::SetDefaultSumw2();

    TFile* fin = new TFile(file.data(), "read");

    std::cout<<file<<std::endl;

    auto rho_ana_nominal_miniaod = new history<TH1F>(fin, "rho_ana_nominal_miniaod"s);
    auto rho_ana_old_extra_aod = new history<TH1F>(fin, "rho_ana_old_extra_aod"s);
    auto rho_ana_new_extra_aod = new history<TH1F>(fin, "rho_ana_new_extra_aod"s);

    auto rho_pho_nominal_miniaod = new history<TH1F>(fin, "rho_pho_nominal_miniaod"s);
    auto rho_pho_old_extra_aod = new history<TH1F>(fin, "rho_pho_old_extra_aod"s);
    auto rho_pho_new_extra_aod = new history<TH1F>(fin, "rho_pho_new_extra_aod"s);

    auto rho_ana_ratio_old_extra_aod = new history<TH1F>(fin, "rho_ana_ratio_old_extra_aod"s);
    auto rho_ana_ratio_new_extra_aod = new history<TH1F>(fin, "rho_ana_ratio_new_extra_aod"s);

    auto rho_pho_ratio_old_extra_aod = new history<TH1F>(fin, "rho_pho_ratio_old_extra_aod"s);
    auto rho_pho_ratio_new_extra_aod = new history<TH1F>(fin, "rho_pho_ratio_new_extra_aod"s);

    // (*rho_ana_nominal_miniaod)[0]->Scale(1. / (*rho_ana_nominal_miniaod)[0]->Integral());
    // (*rho_pho_nominal_miniaod)[0]->Scale(1. / (*rho_pho_nominal_miniaod)[0]->Integral());
    // (*rho_ana_old_extra_aod)[0]->Scale(1. / (*rho_ana_old_extra_aod)[0]->Integral());
    // (*rho_pho_old_extra_aod)[0]->Scale(1. / (*rho_pho_old_extra_aod)[0]->Integral());
    // (*rho_ana_new_extra_aod)[0]->Scale(1. / (*rho_ana_new_extra_aod)[0]->Integral());
    // (*rho_pho_new_extra_aod)[0]->Scale(1. / (*rho_pho_new_extra_aod)[0]->Integral());

    // (*rho_ana_ratio_old_extra_aod)[0]->Divide((*rho_ana_old_extra_aod)[0], (*rho_ana_nominal_miniaod)[0]);
    // (*rho_pho_ratio_old_extra_aod)[0]->Divide((*rho_pho_old_extra_aod)[0], (*rho_pho_nominal_miniaod)[0]);
    // (*rho_ana_ratio_new_extra_aod)[0]->Divide((*rho_ana_new_extra_aod)[0], (*rho_ana_nominal_miniaod)[0]);
    // (*rho_pho_ratio_new_extra_aod)[0]->Divide((*rho_pho_new_extra_aod)[0], (*rho_pho_nominal_miniaod)[0]);

    (*rho_ana_nominal_miniaod)[0]->Rebin();
    (*rho_pho_nominal_miniaod)[0]->Rebin();
    (*rho_ana_old_extra_aod)[0]->Rebin();
    (*rho_pho_old_extra_aod)[0]->Rebin();
    (*rho_ana_new_extra_aod)[0]->Rebin();
    (*rho_pho_new_extra_aod)[0]->Rebin();

    (*rho_ana_ratio_old_extra_aod)[0]->Rebin();
    (*rho_pho_ratio_old_extra_aod)[0]->Rebin();
    (*rho_ana_ratio_new_extra_aod)[0]->Rebin();
    (*rho_pho_ratio_new_extra_aod)[0]->Rebin();

    (*rho_ana_nominal_miniaod)[0]->SetMarkerSize(0.5);
    (*rho_pho_nominal_miniaod)[0]->SetMarkerSize(0.5);
    (*rho_ana_old_extra_aod)[0]->SetMarkerSize(0.5);
    (*rho_pho_old_extra_aod)[0]->SetMarkerSize(0.5);
    (*rho_ana_new_extra_aod)[0]->SetMarkerSize(0.5);
    (*rho_pho_new_extra_aod)[0]->SetMarkerSize(0.5);

    (*rho_ana_ratio_old_extra_aod)[0]->SetMarkerSize(0.5);
    (*rho_pho_ratio_old_extra_aod)[0]->SetMarkerSize(0.5);
    (*rho_ana_ratio_new_extra_aod)[0]->SetMarkerSize(0.5);
    (*rho_pho_ratio_new_extra_aod)[0]->SetMarkerSize(0.5);

    /* draw rho distributions */
    auto system_tag = "PbPb  #sqrt{s_{NN}} = 5.02 TeV"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Simulation}}"s;
    
    std::function<void(int64_t, float)> pho_kinematics = [&](int64_t x, float pos) {
        if (x > 0) {
            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, pos, "p_{T}^{#gamma} > 40 GeV, |#eta^{#gamma}| < 1.44");
        }
    };

    auto hb = new pencil();
    hb->category("type", "nominal_miniaod", "new_extra_aod", "old_extra_aod");

    hb->alias("nominal_miniaod", "Nominal MC MiniAOD");
    hb->alias("new_extra_aod", "New Extra MC AOD");
    hb->alias("old_extra_aod", "Old Extra MC AOD");
    
    auto c1 = new paper("aa_qcd_hiPuRhoAnalyzer_normal", hb);
    apply_style(c1, cms, system_tag);
    c1->accessory(std::bind(pho_kinematics, _1, 0.85));
    c1->add((*rho_ana_nominal_miniaod)[0], "nominal_miniaod");
    c1->stack((*rho_ana_old_extra_aod)[0], "old_extra_aod");
    c1->stack((*rho_ana_new_extra_aod)[0], "new_extra_aod");
    
    auto c2 = new paper("aa_qcd_hiPuRhoAnalyzer_log", hb);
    apply_style(c2, cms, system_tag);
    c2->accessory(std::bind(pho_kinematics, _1, 0.85));
    c2->set(paper::flags::logy);
    c2->add((*rho_ana_nominal_miniaod)[0], "nominal_miniaod");
    c2->stack((*rho_ana_old_extra_aod)[0], "old_extra_aod");
    c2->stack((*rho_ana_new_extra_aod)[0], "new_extra_aod");

    auto c3 = new paper("aa_qcd_hiPuRhoAnalyzer_ratio", hb);
    apply_style(c3, cms, system_tag);
    c3->accessory(std::bind(pho_kinematics, _1, 0.85));
    c3->add((*rho_ana_ratio_old_extra_aod)[0], "old_extra_aod");
    c3->stack((*rho_ana_ratio_new_extra_aod)[0], "new_extra_aod");

    auto c4 = new paper("aa_qcd_ggHiNtuplizer_normal", hb);
    apply_style(c4, cms, system_tag);
    c4->accessory(std::bind(pho_kinematics, _1, 0.85));
    c4->add((*rho_pho_nominal_miniaod)[0], "nominal_miniaod");
    c4->stack((*rho_pho_old_extra_aod)[0], "old_extra_aod");
    c4->stack((*rho_pho_new_extra_aod)[0], "new_extra_aod");

    auto c5 = new paper("aa_qcd_ggHiNtuplizer_log", hb);
    c5->accessory(std::bind(pho_kinematics, _1, 0.85));
    apply_style(c5, cms, system_tag);
    c5->set(paper::flags::logy);
    c5->add((*rho_pho_nominal_miniaod)[0], "nominal_miniaod");
    c5->stack((*rho_pho_old_extra_aod)[0], "old_extra_aod");
    c5->stack((*rho_pho_new_extra_aod)[0], "new_extra_aod");

    auto c6 = new paper("aa_qcd_ggHiNtuplizer_ratio", hb);
    apply_style(c6, cms, system_tag);
    c6->accessory(std::bind(pho_kinematics, _1, 0.85));
    c6->add((*rho_pho_ratio_old_extra_aod)[0], "old_extra_aod");
    c6->stack((*rho_pho_ratio_new_extra_aod)[0], "new_extra_aod");

    hb->sketch();
    c1->draw("pdf");
    c2->draw("pdf");
    c3->draw("pdf");
    c4->draw("pdf");
    c5->draw("pdf");
    c6->draw("pdf");

    /* save output */
    in(output, [&]() {
        rho_ana_nominal_miniaod->save();
        rho_pho_nominal_miniaod->save();
        rho_ana_old_extra_aod->save();
        rho_pho_old_extra_aod->save();
        rho_ana_new_extra_aod->save();
        rho_pho_new_extra_aod->save();
        rho_ana_ratio_old_extra_aod->save();
        rho_pho_ratio_old_extra_aod->save();
        rho_ana_ratio_new_extra_aod->save();
        rho_pho_ratio_new_extra_aod->save();
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return narrate(argv[1], argv[2]);

    return 0;
}
