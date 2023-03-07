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
#include "TTree.h"
#include "TH1.h"
#include "TF1.h"

#include <memory>
#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int plot_qcd(char const* config) {
    auto conf = new configurer(config);

    auto mini = conf->get<std::string>("mini");
    auto aod = conf->get<std::string>("aod");

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    TFile* min = new TFile(mini.data(), "read");
    TFile* ain = new TFile(aod.data(), "read");

    auto m_h_pthat = new history<TH1F>(min, "aa_filtered_h_pthat"s);
    auto m_h_jetpt = new history<TH1F>(min, "aa_filtered_h_jetpt"s);
    auto m_h_njets = new history<TH1F>(min, "aa_filtered_h_njets"s);

    auto m_h_refpt = new history<TH1F>(min, "aa_filtered_h_refpt"s);
    auto m_h_nrefs = new history<TH1F>(min, "aa_filtered_h_nrefs"s);
    auto m_h_phopt = new history<TH1F>(min, "aa_filtered_h_phopt"s);

    auto a_h_pthat = new history<TH1F>(ain, "aa_aod_filtered_h_pthat"s);
    auto a_h_jetpt = new history<TH1F>(ain, "aa_aod_filtered_h_jetpt"s);
    auto a_h_njets = new history<TH1F>(ain, "aa_aod_filtered_h_njets"s);

    auto a_h_refpt = new history<TH1F>(ain, "aa_aod_filtered_h_refpt"s);
    auto a_h_nrefs = new history<TH1F>(ain, "aa_aod_filtered_h_nrefs"s);
    auto a_h_phopt = new history<TH1F>(ain, "aa_aod_filtered_h_phopt"s);

    auto system_tag = "PbPb  #sqrt{s_{NN}} = 5.02 TeV"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Simulation}}"s;

    std::function<void(int64_t, float)> pho_kinematics = [&](int64_t x, float pos) {
        if (x > 0) {
            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, pos, "Inclusive centrality");
            l->DrawLatexNDC(0.865, pos-0.05, "Selected photons");
            l->DrawLatexNDC(0.865, pos-0.1, "p_{T}^{#gamma} > 40 GeV, |#eta^{#gamma}| < 1.44");
        }
    };

    std::function<void(int64_t, float)> jet_kinematics = [&](int64_t x, float pos) {
        if (x > 0) {
            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, pos, "Inclusive centrality");
            l->DrawLatexNDC(0.865, pos-0.05, "Jets associated with selected photons");
            l->DrawLatexNDC(0.865, pos-0.1, "p_{T}^{#gamma} > 40 GeV, |#eta^{#gamma}| < 1.44");
            l->DrawLatexNDC(0.865, pos-0.15, "Reconstructed p_{T}^{j} > 5 GeV");
        }
    };

    std::function<void(int64_t, float)> gen_kinematics = [&](int64_t x, float pos) {
        if (x > 0) {
            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, pos, "Inclusive centrality");
            l->DrawLatexNDC(0.865, pos-0.05, "Generator-level jets associated with selected photons");
            l->DrawLatexNDC(0.865, pos-0.1, "p_{T}^{#gamma} > 40 GeV, |#eta^{#gamma}| < 1.44");
            l->DrawLatexNDC(0.865, pos-0.15, "Reconstructed p_{T}^{j} > 5 GeV");
        }
    };

    std::function<void(int64_t, float)> evt_kinematics = [&](int64_t x, float pos) {
        if (x > 0) {
            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, pos, "Inclusive centrality");
            l->DrawLatexNDC(0.865, pos-0.05, "Events with selected photons");
            l->DrawLatexNDC(0.865, pos-0.1, "p_{T}^{#gamma} > 40 GeV, |#eta^{#gamma}| < 1.44");
        }
    };

    auto hb = new pencil();
    hb->category("type", "nominal_miniaod", "nominal_aod");

    hb->alias("nominal_miniaod", "Nominal MiniAOD");
    hb->alias("nominal_aod", "Nominal AOD");

    auto c1 = new paper("aa_mc_miniaod_aod_selected_pthat_comparison", hb);
    apply_style(c1, cms, system_tag);
    c1->accessory(std::bind(evt_kinematics, _1, 0.75));
    c1->add((*m_h_pthat)[0], "nominal_miniaod");
    c1->stack((*a_h_pthat)[0], "nominal_aod");

    auto c2 = new paper("aa_mc_miniaod_aod_selected_phopt_comparison", hb);
    apply_style(c2, cms, system_tag);
    c2->accessory(std::bind(pho_kinematics, _1, 0.75));
    c2->add((*m_h_phopt)[0], "nominal_miniaod");
    c2->stack((*a_h_phopt)[0], "nominal_aod");

    auto c3 = new paper("aa_mc_miniaod_aod_selected_njets_comparison", hb);
    apply_style(c3, cms, system_tag);
    c3->accessory(std::bind(jet_kinematics, _1, 0.75));
    c3->add((*m_h_njets)[0], "nominal_miniaod");
    c3->stack((*a_h_njets)[0], "nominal_aod");

    auto c4 = new paper("aa_mc_miniaod_aod_selected_jetpt_comparison", hb);
    apply_style(c4, cms, system_tag);
    c4->accessory(std::bind(jet_kinematics, _1, 0.75));
    c4->add((*m_h_jetpt)[0], "nominal_miniaod");
    c4->stack((*a_h_jetpt)[0], "nominal_aod");

    auto c5 = new paper("aa_mc_miniaod_aod_selected_nrefs_comparison", hb);
    apply_style(c5, cms, system_tag);
    c5->accessory(std::bind(gen_kinematics, _1, 0.75));
    c5->add((*m_h_nrefs)[0], "nominal_miniaod");
    c5->stack((*a_h_nrefs)[0], "nominal_aod");

    auto c6 = new paper("aa_mc_miniaod_aod_selected_refpt_comparison", hb);
    apply_style(c6, cms, system_tag);
    c6->accessory(std::bind(gen_kinematics, _1, 0.75));
    c6->add((*m_h_refpt)[0], "nominal_miniaod");
    c6->stack((*a_h_refpt)[0], "nominal_aod");

    hb->sketch();
    c1->draw("pdf");
    c2->draw("pdf");
    c3->draw("pdf");
    c4->draw("pdf");
    c5->draw("pdf");
    c6->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 2)
        return plot_qcd(argv[1]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}