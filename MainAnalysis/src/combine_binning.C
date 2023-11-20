#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"
#include "../git/history/include/memory.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom3.h"

#include <memory>
#include <string>
#include <vector>
#include <ctime>

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename... T>
void scale(double factor, T*... args) {
    (void)(int [sizeof...(T)]) { (args->scale(factor), 0)... };
}

int combine_binning(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto base = conf->get<std::string>("base");
    auto inputs = conf->get<std::vector<std::string>>("inputs");
    auto group = conf->get<std::string>("group");
    auto label = conf->get<std::string>("label");
    auto tag = conf->get<std::string>("tag");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");

    auto heavyion = sel->get<bool>("heavyion");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");

    auto bpho_pt = sel->get<std::vector<float>>("photon_pt_bounds");
    auto bjet_pt = sel->get<std::vector<float>>("jet_pt_bounds");

    auto ihf = new interval(dhf);

    /* open input files */
    std::vector<TFile*> files(inputs.size(), nullptr);
    zip([&](auto& file, auto const& input) {
        auto file_name = base + "/" + set + "/" + input;
        file = new TFile(file_name.data(), "read");
    }, files, inputs);

    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    /* combine nevt information */
    auto nevt = new history<TH1F>(files[0], group + "_nevt"s);

    for (size_t i = 1; i < files.size(); ++i) {
        auto nevt_add = new history<TH1F>(files[i], group + "_nevt"s);
        *nevt += *nevt_add;
    }

    nevt->save();

    std::cout << label << std::endl;

    auto name = group + "_"s + label;
    auto hist = new history<TH2F>(files[0], name); 

    auto name_mix = group + "_mix_"s + label;
    auto hist_mix = new history<TH2F>(files[0], name_mix);

    for (size_t i = 1; i < files.size(); ++i) {
        auto hist_add = new history<TH2F>(files[i], name);
        auto hist_mix_add = new history<TH2F>(files[i], name_mix);

        *hist += *hist_add;
        *hist_mix += *hist_mix_add;

        delete hist_add;
    }

    hist->apply([](TH1* h) { 
        for (int i = 0; i < h->GetNbinsX(); ++i) {
            if (h->GetBinContent(i + 1) == 0) {
                h->SetBinError(i + 1, 1);
            }
        }
    });


    auto hist_sub = new history<TH2F>(*hist, "sub");
    hist_sub->rename(group + "_sub_" + label);
    *hist_sub -= *hist_mix;

    hist->save();
    hist_mix->save();
    hist_sub->save();

    /* info text */
    auto hf_info = [&](int64_t index) {
        info_text(index, 0.73, "Cent. %i - %i%%", dcent, true); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            auto photon_selections = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs)  + 
                ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
            auto jet_selections = "anti-k_{T} R = 0.3, " + to_text(bjet_pt[0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[1]) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(11);
            l->DrawLatexNDC(0.86, 0.20, photon_selections.data());
            l->DrawLatexNDC(0.86, 0.14, jet_selections.data());
        }
    };

    auto blurb = [&](int64_t index) {
        if (index > 0) {
            auto system_tag = (heavyion) ? "PbPb 1.69 nb^{-1}, "s : "pp 302 pb^{-1}, "s;
            system_tag += "#sqrt{s_{NN}} = 5.02 TeV"s;
            auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;

            TLatex* l = new TLatex();
            l->SetTextAlign(11);
            l->SetTextFont(43);
            l->SetTextSize(11);
            l->DrawLatexNDC(0.14, 0.83, cms.data());
            l->DrawLatexNDC(0.14, 0.77, system_tag.data());
        }
    };

    /* figures */
    auto hb = new pencil();

    auto c = new paper(set + "_binning_" + tag, hb);
    apply_style(c, "", "");
    if (heavyion) c->accessory(hf_info);
    c->accessory(kinematics);
    c->accessory(blurb);
    c->divide(ihf->size()/2, -1);
    
    for (int64_t i = 0; i < ihf->size(); ++i) {
        c->add((*hist)[i*2]);
        c->adjust((*hist)[i*2], "colz0", "");
    };

    hb->sketch();
    c->draw("pdf");

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return combine_binning(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}