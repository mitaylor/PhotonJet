#include "../include/lambdas.h"
#include "../include/specifics.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TH1.h"

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename... T>
void scale_bin_width(T*... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1., "width"); }), 0)... };
}

void set_range(history<TH1F>* arg1, history<TH1F>* arg2) {
    arg1->apply([&](TH1* h1, int64_t index) {
        auto min = std::min(h1->GetMinimum(), (*arg2)[index]->GetMinimum());
        min = std::min(min, 0.0);

        auto max = std::max(h1->GetMaximum(), (*arg2)[index]->GetMaximum());

        default_formatter(h1, min*1.3, max*1.3); 
        default_formatter((*arg2)[index], min*1.3, max*1.3); 
    });
}

int jubilate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto comparison = conf->get<std::string>("comparison");
    auto scan = conf->get<std::string>("scan");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto labels = conf->get<std::vector<std::string>>("labels");
    auto minimums = conf->get<std::vector<float>>("minimums");
    auto maximums = conf->get<std::vector<float>>("maximums");

    auto jpt = conf->get<bool>("jpt");
    auto eta = conf->get<bool>("eta");
    auto dphi = conf->get<bool>("dphi");

    /* selections */
    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto rjpt = sel->get<std::vector<float>>("jpt_range");
    auto reta = sel->get<std::vector<float>>("eta_range");
    auto rdphi = sel->get<std::vector<float>>("dphi_range");

    auto const photon_pt_min = sel->get<float>("photon_pt_min");
    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");

    auto const jet_pt_min = sel->get<float>("jet_pt_min");
    auto const jet_pt_max = sel->get<float>("jet_pt_max");
    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");
    
    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto ihf = new interval(dhf);

    /* load history objects */
    TFile* f = new TFile((base + input).data(), "read");
    TFile* t = new TFile((base + comparison).data(), "read");

    TH1::SetDefaultSumw2();

    /* define plotting accessories */
    auto hf_info = [&](int64_t index) {
        info_text(index, 0.75, "Cent. %i - %i%%", dcent, true); };

    auto hb = new pencil();
    hb->category("type", "raw", "mix", "sub", "reco");

    auto system_tag = system + "  #sqrt{s_{NN}} = 5.02 TeV"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;

    zip([&](auto const& label, auto& min, auto& max) {
        std::cout << label << std::endl;

        auto name = "raw_"s + scan + "_"s + label;
        auto hist = new history<TH1F>(f, name); 

        auto name_mix = "raw_mix_"s + scan + "_"s + label;
        auto hist_mix = new history<TH1F>(f, name_mix);

        auto name_sub = "raw_sub_"s + scan + "_"s + label;
        auto hist_sub = new history<TH1F>(f, name_sub);

        auto name_reco = "aa_reco_"s + scan + "_"s + label;
        auto hist_reco = new history<TH1F>(t, name_reco);

        scale_bin_width(hist, hist_mix, hist_sub, hist_reco);
        set_range(hist, hist_mix);
        set_range(hist_sub, hist_reco);

        auto shape = hist->shape(); // photon pt, scan (optional), centrality
        
        if (shape.size() == 2) {
            auto cuts_info = [&](int64_t index) {
                if (index > 0) {
                    auto photon_selections = "p_{T}^{#gamma} > "s + to_text(photon_pt_min) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs);
                    if (jpt || eta) photon_selections += ", #Delta#phi_{j#gamma} > "s + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
                    auto jet_selections = "anti-k_{T} R = 0.3, |#eta^{jet}| < "s + to_text(jet_eta_abs) + ", " + to_text(jet_pt_min) + " < p_{T}^{jet} < "s + to_text(jet_pt_max) + " GeV";

                    TLatex* l = new TLatex();
                    l->SetTextAlign(31);
                    l->SetTextFont(43);
                    l->SetTextSize(11);
                    l->DrawLatexNDC(0.86, 0.65, photon_selections.data());
                    l->DrawLatexNDC(0.86, 0.60, jet_selections.data());
                }
            };

            auto c1 = new paper(set + "_"s + tag + "_mebs_"s + label, hb);
            apply_style(c1, cms, system_tag);
            c1->accessory(std::bind(line_at, _1, 0.f, min, max));
            c1->accessory(hf_info);
            c1->accessory(cuts_info);
            c1->divide(ihf->size() , -1);

            auto c2 = new paper(set + "_"s + tag + "_closure_"s + label, hb);
            apply_style(c2, cms, system_tag);
            c2->accessory(std::bind(line_at, _1, 0.f, min, max));
            c2->accessory(hf_info);
            c2->accessory(cuts_info);
            c2->divide(ihf->size() , -1);

            for (int64_t i = 0; i < hist->size(); ++i) {
                c1->add((*hist)[i], "raw");
                c1->stack((*hist_mix)[i], "mix");
                c2->add((*hist_reco)[i], "reco");
                c2->stack((*hist_sub)[i], "sub");
            }

            hb->sketch();
            c1->draw("pdf");
            c2->draw("pdf");
            delete c1; delete c2;
        }

        if (shape.size() == 3) {
            std::vector<paper*> cs1(shape[1], nullptr);
            std::vector<paper*> cs2(shape[1], nullptr);

            auto range_info = [&](int64_t index, int64_t step) {
                if (index > 0) {
                    if (jpt) { info_text(step, 0.71, "%g < p_{T}^{jet} < %g GeV", rjpt, false); }
                    if (eta) { info_text(step, 0.71, "%g < #eta^{jet} < %g", reta, false); }
                    if (dphi) { info_text(step, 0.71, "%g#pi < #Delta#phi_{j#gamma} < %g#pi", rdphi, false); }
                }
            };

            auto cuts_info = [&](int64_t index) {
                if (index > 0) {
                    auto photon_selections = "p_{T}^{#gamma} > "s + to_text(photon_pt_min) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs);
                    if (jpt || eta) photon_selections += ", #Delta#phi_{j#gamma} > "s + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
                    auto jet_selections = "anti-k_{T} R = 0.3"s;
                    if (dphi || jpt) jet_selections += ", |#eta^{jet}| < "s + to_text(jet_eta_abs);
                    if (dphi || eta) jet_selections += ", "s + to_text(jet_pt_min) + " < p_{T}^{jet} < "s + to_text(jet_pt_max) + " GeV"s;

                    TLatex* l = new TLatex();
                    l->SetTextAlign(31);
                    l->SetTextFont(43);
                    l->SetTextSize(11);
                    l->DrawLatexNDC(0.86, 0.65, photon_selections.data());
                    l->DrawLatexNDC(0.86, 0.60, jet_selections.data());
                }
            };

            for (int64_t i = 0; i < shape[1]; ++i) {
                cs1[i] = new paper(set + "_"s + tag + "_mebs_"s + label + "_"s + to_text(i), hb);
                apply_style(cs1[i], cms, system_tag);
                cs1[i]->accessory(std::bind(line_at, _1, 0.f, min, max));
                cs1[i]->accessory(std::bind(range_info, _1, i+1));
                cs1[i]->accessory(hf_info);
                cs1[i]->accessory(cuts_info);
                cs1[i]->divide(ihf->size() , -1);

                cs2[i] = new paper(set + "_"s + tag + "_closure_"s + label + "_"s + to_text(i), hb);
                apply_style(cs2[i], cms, system_tag);
                cs2[i]->accessory(std::bind(line_at, _1, 0.f, min, max));
                cs2[i]->accessory(std::bind(range_info, _1, i+1));
                cs2[i]->accessory(hf_info);
                cs2[i]->accessory(cuts_info);
                cs2[i]->divide(ihf->size() , -1);

                for (int64_t j = 0; j < shape[2]; ++j) {
                    std::vector<int64_t> index = {0, i, j};
                    cs1[i]->add((*hist)[index], "raw");
                    cs1[i]->stack((*hist_mix)[index], "mix");
                    cs2[i]->add((*hist_reco)[index], "reco");
                    cs2[i]->stack((*hist_sub)[index], "sub");
                }
            }

            hb->sketch();

            for (int64_t i = 0; i < shape[1]; ++i) {
                cs1[i]->draw("pdf");
                cs2[i]->draw("pdf");

                delete cs1[i]; delete cs2[i];
            }
        }
    }, labels, maximums, minimums);

    /* save output */
    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return jubilate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
