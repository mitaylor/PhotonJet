#include "../include/lambdas.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TDecompSVD.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TSpline.h"
#include "TVectorT.h"
#include "TStyle.h"

#include "TUnfoldBinning.h"
#include "TUnfoldDensity.h"

#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;


template <typename T>
T* null(int64_t, std::string const&, std::string const&) {
    return nullptr;
}

TH2F* variance(TH1* flat, multival const* m) {
    auto cov = new TH2F("cov", "", m->size(), 0, m->size(),
        m->size(), 0, m->size());

    for (int64_t i = 0; i < m->size(); ++i) {
        auto err = flat->GetBinError(i + 1);
        cov->SetBinContent(i + 1, i + 1, err * err);
    }

    return cov;
}

TH1F* fold(TH1* flat, TH2* covariance, multival const* m, int64_t axis,
           std::vector<int64_t>& offsets) {
    auto name = std::string(flat->GetName()) + "_fold" + std::to_string(axis);
    auto hfold = m->axis(axis).book<TH1F, 2>(0, name, "",
        { offsets[axis << 1], offsets[(axis << 1) + 1] });

    auto shape = m->shape();

    auto front = std::vector<int64_t>(m->dims(), 0);
    auto back = std::vector<int64_t>(m->dims(), 0);
    for (int64_t i = 0; i < m->dims(); ++i) {
        front[i] = offsets[i << 1];
        back[i] = shape[i] - offsets[(i << 1) + 1];
    }

    auto size = back[axis] - front[axis];
    auto list = new std::vector<int64_t>[size];

    for (int64_t i = 0; i < m->size(); ++i) {
        auto indices = m->indices_for(i);

        bool flag = false;
        zip([&](int64_t index, int64_t f, int64_t b) {
            flag = flag || index < f || index >= b;
        }, indices, front, back);
        if (flag) { continue; }

        auto index = indices[axis] - front[axis];
        hfold->SetBinContent(index + 1, hfold->GetBinContent(index + 1)
            + flat->GetBinContent(i + 1));

        list[index].push_back(i);
    }

    auto cov = covariance ? (TH2F*)covariance->Clone() : variance(flat, m);

    for (int64_t i = 0; i < size; ++i) {
        auto indices = list[i];
        int64_t count = indices.size();

        auto error = 0.;
        for (int64_t j = 0; j < count; ++j) {
            auto j_x = indices[j] + 1;
            for (int64_t k = 0; k < count; ++k) {
                auto k_x = indices[k] + 1;
                error = error + cov->GetBinContent(j_x, k_x);
            }
        }

        hfold->SetBinError(i + 1, std::sqrt(error));
    }

    delete [] list;
    delete cov;

    hfold->Scale(1., "width");

    return hfold;
}

int plot_unfolding_inputs(char const* config, char const* selections) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");
    auto type = conf->get<std::string>("type");

    auto victim = conf->get<std::string>("victim");
    auto label = conf->get<std::string>("label");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

     /* selections */
    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto heavyion = sel->get<bool>("heavyion");

    auto osr = sel->get<std::vector<int64_t>>("osr");

    auto rdrr = sel->get<std::vector<float>>("drr_range");
    auto rptr = sel->get<std::vector<float>>("ptr_range");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");

    auto bpho_pt = sel->get<std::vector<float>>("photon_pt_bounds");
    auto bjet_pt = sel->get<std::vector<float>>("jet_pt_bounds");

    auto ihf = new interval(dhf);

    auto idrr = new interval("#deltaj", rdrr);
    auto iptr = new interval("p_{T}^{j}"s, rptr);

    auto mr = new multival(*idrr, *iptr);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input and victims */
    TFile* fi = new TFile((base + input).data(), "read");
    auto matrices = new history<TH2F>(fi, tag + "_c");

    TFile* fv = new TFile((base + victim).data(), "read");
    auto victims = new history<TH1F>(fv, label);

    auto side0 = new history<TH1F>(label + "_side0", "", null<TH1F>, ihf->size());
    auto side1 = new history<TH1F>(label + "_side1", "", null<TH1F>, ihf->size());

    /* info text */
    auto hf_info = [&](int64_t index) {
        info_text(index, 0.85, "Cent. %i - %i%%", dcent, true); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            auto photon_selections = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs)  + 
                ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
            auto jet_selections = "anti-k_{T} R = 0.3, " + to_text(bjet_pt[0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[1]) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(11);
            l->DrawLatexNDC(0.865, 0.21, photon_selections.data());
            l->DrawLatexNDC(0.865, 0.15, jet_selections.data());
        }
    };

    auto blurb = [&](int64_t index) {
        if (index > 0) {
            std::string system_tag = "#sqrt{s_{NN}} = 5.02 TeV"s;
            system_tag += (heavyion) ? ", PbPb 1.69 nb^{-1}"s : ", pp 302 pb^{-1}"s;
            auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Simulation}}"s;

            // TLatex* l = new TLatex();
            // l->SetTextAlign(11);
            // l->SetTextFont(42);
            // l->SetTextSize(11);
            // l->DrawLatexNDC(0.135, 0.865, cms.data());
            // l->DrawLatexNDC(0.135, 0.765, system_tag.data());

            TLatex* l = new TLatex();
            l->SetTextAlign(11);
            l->SetTextFont(43);
            l->SetTextSize(11);
            l->DrawLatexNDC(0.135, 0.83, cms.data());
            l->DrawLatexNDC(0.135, 0.78, system_tag.data());
        }
    };

    /* figures */
    auto hb = new pencil();

    gStyle->SetPalette(kInvertedDarkBodyRadiator);

    std::vector<paper*> cs(4, nullptr);
    zip([&](paper*& c, std::string const& title) {
        c = new paper(set + "_unfolding_dj_" + tag + "_" + type + "_" + title, hb);
        apply_style(c, "", "");
        if (heavyion) c->accessory(hf_info);
        c->accessory(kinematics);
        c->accessory(blurb);
        c->divide(ihf->size()/2, -1);
    }, cs, (std::initializer_list<std::string> const) {
        "matrices"s, "victims"s, "fold0"s, "fold1"s });

    cs[2]->format(std::bind(default_formatter, _1, -2, 27));
    cs[3]->format(std::bind(default_formatter, _1, -0.001, 0.02));

    cs[0]->set(paper::flags::logz);  

    for (int64_t i = 0; i < ihf->size(); ++i) {
        /* input folds */
        (*side0)[i] = fold((*victims)[i], nullptr, mr, 0, osr);
        (*side1)[i] = fold((*victims)[i], nullptr, mr, 1, osr);

        /* normalise to unity */
        (*side0)[i]->Scale(1. / (*side0)[i]->Integral("width"));

        /* figures */
        cs[0]->add((*matrices)[i]);
        cs[0]->adjust((*matrices)[i], "colz", "");

        (*matrices)[i]->GetZaxis()->SetTitle("");

        cs[1]->add((*victims)[i]);
        cs[2]->add((*side0)[i]);
        cs[3]->add((*side1)[i]);
    };

    hb->sketch();

    for (auto c : cs)
        c->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return plot_unfolding_inputs(argv[1], argv[2]);

    printf("usage: %s [config] [selections]\n", argv[0]);
    return 1;
}