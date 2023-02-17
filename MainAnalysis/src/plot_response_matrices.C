#include "../include/lambdas.h"

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

template <std::size_t N>
TH1F* fold(TH1* flat, TH2* covariance, multival const* m, int64_t axis,
           std::array<int64_t, N> const& offsets) {
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

template <typename T>
TH2F* shade(T* flat, multival const* m, std::array<int64_t, 4> const& offset) {
    auto name = std::string(flat->GetName()) + "_shade";
    auto hshade = m->book<TH2F, 4>(0, name, "", offset);

    for (int64_t i = 0; i < m->size(); ++i) {
        auto indices = m->indices_for(i);
        indices[0] = indices[0] - offset[0];
        indices[1] = indices[1] - offset[2];

        if (indices[0] < 0 || indices[0] >= hshade->GetNbinsX()
                || indices[1] < 0 || indices[1] >= hshade->GetNbinsY()) {
            continue; }

        hshade->SetBinContent(indices[0] + 1, indices[1] + 1,
            flat->GetBinContent(i + 1));
        hshade->SetBinError(indices[0] + 1, indices[1] + 1,
            flat->GetBinError(i + 1));
    }

    hshade->Scale(1., "width");

    return hshade;
}

int plot_unfolding_inputs(char const* config) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");
    auto type = conf->get<std::string>("type");
    auto observable = conf->get<std::string>("observable");
    auto xlabel = conf->get<std::string>("xlabel");

    auto victim = conf->get<std::string>("victim");
    auto label = conf->get<std::string>("label");
    auto divisions = conf->get<std::vector<int64_t>>("divisions");
    auto heavyion = conf->get<bool>("heavyion");

    auto rdrr = conf->get<std::vector<float>>("drr_range");
    auto rptr = conf->get<std::vector<float>>("ptr_range");

    auto rfold0 = conf->get<std::vector<float>>("fold0_range");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto mpthf = new multival(dpt, dhf);
    auto ihf = new interval(dhf);

    auto idrr = new interval(xlabel, rdrr);
    auto iptr = new interval("p_{T}^{j}"s, rptr);

    auto mr = new multival(*idrr, *iptr);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input and victims */
    TFile* fi = new TFile(input.data(), "read");
    auto matrices = new history<TH2F>(fi, tag + "_c");

    TFile* fv = new TFile(victim.data(), "read");
    auto victims = new history<TH1F>(fv, label);

    auto shape = victims->shape();

    auto shaded = new history<TH2F>(label + "_shade", "", null<TH2F>, shape);
    auto side0 = new history<TH1F>(label + "_side0", "", null<TH1F>, shape);
    auto side1 = new history<TH1F>(label + "_side1", "", null<TH1F>, shape);

    std::array<int64_t, 4> osr = { 0, 0, 0, 0 };

    /* info text */
    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, mpthf, pt_info, hf_info); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, 0.75, "anti-k_{T} R = 0.3, p_{T}^{jet} > 20 GeV, |#eta^{jet}| < 1.6");
            l->DrawLatexNDC(0.865, 0.81, "p_{T}^{#gamma} > 40 GeV, |#eta^{#gamma}| < 1.44, #Delta#phi_{j#gamma} < 7#pi/8");
        }
    };

    auto system_tag = system + "  #sqrt{s_{NN}} = 5.02 TeV"s;
    system_tag += (heavyion) ? ", 1.69 nb^{-1}"s : ", 3.02 pb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}}"s;
    if (!is_paper) cms += " #it{#scale[1.2]{Preliminary}}"s;

    /* figures */
    auto hb = new pencil();

    std::vector<paper*> cs(5, nullptr);
    zip([&](paper*& c, std::string const& title) {
        c = new paper(tag + "_" + type + "_" + observable + "_" + title, hb);
        apply_style(c, cms, system_tag);
        c->accessory(pthf_info);
        c->accessory(kinematics);
        c->divide(divisions[0], divisions[1]);
    }, cs, (std::initializer_list<std::string> const) {
        "matrices"s, "victims"s, "fold0"s, "fold1"s, "shaded"s });

    cs[2]->format(std::bind(default_formatter, _1, rfold0[0], rfold0[1]));
    cs[3]->format(std::bind(default_formatter, _1, -0.001, 0.02));

    for (int64_t i = 0; i < ihf->size(); ++i) {
        /* input folds */
        (*shaded)[i] = shade((*victims)[i], mr, osr);
        (*side0)[i] = fold((*victims)[i], nullptr, mr, 0, osr);
        (*side1)[i] = fold((*victims)[i], nullptr, mr, 1, osr);

        /* normalise to unity */
        (*side0)[i]->Scale(1. / (*side0)[i]->Integral("width"));

        /* figures */
        cs[0]->add((*matrices)[i]);
        cs[0]->adjust((*matrices)[i], "colz", "");

        cs[1]->add((*victims)[i]);
        cs[2]->add((*side0)[i]);
        cs[3]->add((*side1)[i]);

        cs[4]->add((*shaded)[i]);
        cs[4]->adjust((*shaded)[i], "colz", "");
    };

    hb->sketch();

    for (auto c : cs)
        c->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 2)
        return plot_unfolding_inputs(argv[1]);

    return 0;
}