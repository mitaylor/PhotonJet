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

int jubilate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

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

    TH1::SetDefaultSumw2();

    /* define plotting accessories */
    auto hf_info = [&](int64_t index) {
        info_text(index, 0.75, "Cent. %i - %i%%", dcent, true); };

    auto hb = new pencil();
    hb->category("type", "raw", "mix");

    auto system_tag = system + "  #sqrt{s_{NN}} = 5.02 TeV"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;

    zip([&](auto const& label, auto& min, auto& max) {
        std::cout << label << std::endl;

        auto name = "raw_"s + label;
        auto hist = new history<TH1F>(f, name); 

        auto name_mix = "raw_mix_"s + label;
        auto hist_mix = new history<TH1F>(f, name_mix);

        scale_bin_width(hist, hist_mix);

        auto shape = hist->shape(); // photon pt, scan (optional), centrality
        
        if (shape.size() == 2) {
            auto cuts_info = [&](int64_t index) {
                if (index > 0) {
                    std::string photon_selections = "p_{T}^{#gamma} > "s + to_text(photon_pt_min) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs);
                    if (jpt || eta) photon_selections += ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
                    std::string jet_selections = "anti-k_{T} R = 0.3, |#eta^{jet}| < "s + to_text(jet_eta_abs) + ", " + to_text(jet_pt_min) + " < p_{T}^{jet} < "s + to_text(jet_pt_max) + " GeV";

                    TLatex* l = new TLatex();
                    l->SetTextAlign(31);
                    l->SetTextFont(43);
                    l->SetTextSize(11);
                    l->DrawLatexNDC(0.86, 0.65, photon_selections.c_str());
                    l->DrawLatexNDC(0.86, 0.60, jet_selections.c_str());
                }
            };

            auto c = new paper(set + "_" + tag + "_" + label, hb);
            apply_style(c, cms, system_tag);
            c->accessory(std::bind(line_at, _1, 0.f, min, max));
            c->accessory(hf_info);
            c->divide(ihf->size() , -1);

            for (int64_t i = 0; i < hist->size(); ++i) {
                c->add((*hist)[i], "raw");
                c->stack((*hist_mix)[i], "mix");
            }

            hb->sketch();
            c->draw("pdf");
            delete c;
        }

        if (shape.size() == 3) {
            std::vector<paper*> cs(shape[1], nullptr);

            auto range_info = [&](int64_t index) {
                if (jpt) { info_text(index, 0.71, "%d < p_{T}^{jet} < %d GeV", rjpt, false); }
                if (eta) { info_text(index, 0.71, "%d < #eta^{jet} < %d", reta, false); }
                if (dphi) { info_text(index, 0.71, "%d < #Delta#phi_{j#gamma} < %d", rdphi, false); }
            };

            auto cuts_info = [&](int64_t index) {
                if (index > 0) {
                    auto photon_selections = "p_{T}^{#gamma} > "s + to_text(photon_pt_min) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs);
                    if (jpt || eta) photon_selections += ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
                    auto jet_selections = "anti-k_{T} R = 0.3";
                    if (dphi || jpt) jet_selections += ", |#eta^{jet}| < "s + to_text(jet_eta_abs);
                    if (dphi || eta) jet_selections += ", " + to_text(jet_pt_min) + " < p_{T}^{jet} < "s + to_text(jet_pt_max) + " GeV";

                    TLatex* l = new TLatex();
                    l->SetTextAlign(31);
                    l->SetTextFont(43);
                    l->SetTextSize(11);
                    l->DrawLatexNDC(0.86, 0.65, photon_selections.data());
                    l->DrawLatexNDC(0.86, 0.60, jet_selections.data());
                }
            };

            for (int64_t i = 0; i < shape[1]; ++i) {
                cs[i] = new paper(set + "_" + tag + "_" + label + "_" + to_text(i), hb);
                apply_style(cs[i], cms, system_tag);
                cs[i]->accessory(std::bind(line_at, _1, 0.f, min, max));
                cs[i]->accessory(range_info);
                cs[i]->accessory(hf_info);
                cs[i]->accessory(cuts_info);
                cs[i]->divide(ihf->size() , -1);

                for (int64_t j = 0; j < shape[2]; ++j) {
                    std::vector<int64_t> index = {0, i, j};
                    cs[i]->add((*hist)[index], "raw");
                    cs[i]->stack((*hist_mix)[index], "mix");
                }
            }

            hb->sketch();

            for (int64_t i = 0; i < shape[1]; ++i) {
                cs[i]->draw("pdf");
                delete cs[i];
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
