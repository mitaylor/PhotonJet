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

int plot_binning(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto group = conf->get<std::string>("group");
    auto label = conf->get<std::string>("label");
    auto tag = conf->get<std::string>("tag");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");

    auto base = sel->get<std::string>("base");
    auto heavyion = sel->get<bool>("heavyion");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto rdrr = sel->get<std::vector<float>>("drr_range");
    auto rptr = sel->get<std::vector<float>>("ptr_range");

    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");
    auto dpt = sel->get<std::vector<float>>("photon_pt_diff");

    auto bpho_pt = sel->get<std::vector<float>>("photon_pt_bounds");
    auto bjet_pt = sel->get<std::vector<float>>("jet_pt_bounds");

    /* define intervals */
    auto mpthf = new multival(dpt, dhf);
    auto mhf = new multival(dhf);

    /* open input files */
    TFile* fin = new TFile((base + input).data(), "read");

    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    /* get inputs */
    auto nevt = new history<TH1F>(fin, group + "_nevt"s);
    auto hist = new history<TH1F>(fin, group + "_"s + label); 
    auto hist_mix = new history<TH1F>(fin, group + "_mix_"s + label);
    auto hist_sub = new history<TH1F>(fin, group + "_sub_"s + label);

    /* rescale by number of signal photons */
    hist->multiply(*nevt);
    hist_mix->multiply(*nevt);
    hist_sub->multiply(*nevt);

    /* discard overflow photon pt bin */
    auto discard = [](history<TH1F>*& h, int64_t axis) {
        auto shape = h->shape();
        shape[axis] = shape[axis] - 1;
        h = h->shrink("s", shape, std::vector<int64_t>(h->dims(), 0));
    };

    discard(nevt, 0);
    discard(hist, 0);
    discard(hist_mix, 0);
    discard(hist_sub, 0);

    /* integrate histograms */
    auto nevt_d_hf = nevt->sum(0);
    auto hist_d_hf = hist->sum(0);
    auto hist_mix_d_hf = hist_mix->sum(0);
    auto hist_sub_d_hf = hist_sub->sum(0);

    /* rename histograms */
    nevt->rename("nevt");
    hist->rename("hist");
    hist_mix->rename("hist_mix");
    hist_sub->rename("hist_sub");

    nevt_d_hf->rename("nevt_d_hf");
    hist_d_hf->rename("hist_d_hf");
    hist_mix_d_hf->rename("hist_mix_d_hf");
    hist_sub_d_hf->rename("hist_sub_d_hf");

    /* make 2D histogram version */
    auto mdrjpt = new multival(rdrr, rptr);
    auto fdrjpt = std::bind(&multival::book<TH2F>, mdrjpt, _1, _2, _3);

    auto hist2d_d_hf = new memory<TH2F>("hist2d_d_hf"s, "", fdrjpt, mhf);
    auto hist2d_mix_d_hf = new memory<TH2F>("hist2d_mix_d_hf"s, "", fdrjpt, mhf);
    auto hist2d_sub_d_hf = new memory<TH2F>("hist2d_sub_d_hf"s, "", fdrjpt, mhf);

    for (int64_t i = 0; i < mhf->size(); ++i) {
        for (int64_t j = 0; j < mdrjpt->size(); ++j) {
            auto indices = m->indices_for(i);

            auto value_d_hf = (*hist_d_hf)[i]->GetBinContent(j + 1);
            auto value_mix_d_hf = (*hist_mix_d_hf)[i]->GetBinContent(j + 1);
            auto value_sub_d_hf = (*hist_sub_d_hf)[i]->GetBinContent(j + 1);

            (*hist2d_d_hf)[i]->SetBinContent(indices[0], indices[1], value_d_hf);
            (*hist2d_mix_d_hf)[i]->SetBinContent(indices[0], indices[1], value_mix_d_hf);
            (*hist2d_sub_d_hf)[i]->SetBinContent(indices[0], indices[1], value_sub_d_hf);
        }
    };

    /* save histograms */
    nevt->save();
    hist->save();
    hist_mix->save();
    hist_sub->save();

    nevt_d_hf->save();
    hist_d_hf->save();
    hist_mix_d_hf->save();
    hist_sub_d_hf->save();

    hist2d_d_hf->save();
    hist2d_mix_d_hf->save();
    hist2d_sub_d_hf->save();

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

    std::vector<paper*> cs(3, nullptr);

    zip([&](paper*& c, std::string const& title) {
        c = new paper(set + "_" + title + "_binning_" + tag, hb);
        apply_style(c, "", "");
        if (heavyion) c->accessory(hf_info);
        c->accessory(kinematics);
        c->accessory(blurb);
        c->divide(mhf->size()/2, -1);
        c->set(paper::flags::logz); 
    }, cs, (std::initializer_list<std::string> const) {"unsubtracted"s, "mix"s, "subtracted"s});
    
    for (int64_t i = 0; i < mhf->size(); ++i) {
        (*hist2d_d_hf)[i]->SetMinimum(1);
        cs[0]->add((*hist2d_d_hf)[i]);
        cs[0]->adjust((*hist2d_d_hf)[i], "colz", "");

        (*hist2d_mix_d_hf)[i]->SetMinimum(1);
        cs[1]->add((*hist2d_mix_d_hf)[i]);
        cs[1]->adjust((*hist2d_mix_d_hf)[i], "colz", "");

        (*hist2d_sub_d_hf)[i]->SetMinimum(1);
        cs[2]->add((*hist2d_sub_d_hf)[i]);
        cs[2]->adjust((*hist2d_sub_d_hf)[i], "colz", "");
    };

    hb->sketch();

    for (auto c : cs)
        c->draw("pdf");

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return plot_binning(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}