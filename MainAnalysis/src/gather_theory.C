#include "../include/lambdas.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom3.h"
#include "TMath.h"

#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename... T>
void scale_bin_width(T*... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1., "width"); }), 0)... };
}

template <typename... T>
void normalise_to_unity(T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1. / obj->Integral("width")); }), 0)... };
}

bool back_to_back(float photon_phi, float jet_phi, float threshold) {
    float dphi = std::abs(photon_phi - jet_phi);
    if (dphi > TMath::Pi()) dphi = std::abs(dphi - 2*TMath::Pi());

    return dphi > threshold * TMath::Pi();
}

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

int gather_theory(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto trees = conf->get<std::vector<std::string>>("trees");

    /* selections */
    auto sel = new configurer(selections);

    auto const photon_pt_min = sel->get<float>("photon_pt_min");
    auto const photon_pt_max = sel->get<float>("photon_pt_max");
    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");
    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");
    
    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto rdrg = sel->get<std::vector<float>>("drg_range");
    auto rptg = sel->get<std::vector<float>>("ptg_range");

    auto osg = sel->get<std::vector<int64_t>>("osg");

    auto osg_part1 = sel->get<std::vector<int64_t>>("osg_part1");
    auto osg_part2 = sel->get<std::vector<int64_t>>("osg_part2");

    /* make histograms */
    auto incl = new interval(""s, 1, 0.f, 9999.f);

    auto mg = new multival(rdrg, rptg);

    auto fn = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    auto fg = [&](int64_t, std::string const& name, std::string const& label) {
        return new TH1F(name.data(), (";Generator Bin;"s + label).data(), mg->size(), 0, mg->size()); };

    std::vector<history<TH1F>*> hist_nevt(trees.size(), nullptr);
    std::vector<history<TH1F>*> hist_dr_jpt(trees.size(), nullptr);
    std::vector<history<TH1F>*> hist_dr(trees.size(), nullptr);
    std::vector<history<TH1F>*> hist_jpt(trees.size(), nullptr);
    std::vector<history<TH1F>*> hist_dr_part1(trees.size(), nullptr);
    std::vector<history<TH1F>*> hist_jpt_part1(trees.size(), nullptr);
    std::vector<history<TH1F>*> hist_dr_part2(trees.size(), nullptr);
    std::vector<history<TH1F>*> hist_jpt_part2(trees.size(), nullptr);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    TFile* f = new TFile(input.data(), "read");

    for (size_t i = 0; i < trees.size(); ++i) {
        std::cout << trees[i] << std::endl;

        TTree* t = (TTree*)f->Get(trees[i].c_str());
        int64_t nentries = static_cast<int64_t>(t->GetEntries());
        std::cout << nentries << std::endl;

        hist_nevt[i] = new history<TH1F>(trees[i] + "_nevt", "", fn, 1);
        hist_dr_jpt[i] = new history<TH1F>(trees[i] + "_dr_jpt", "", fg, 1);
        hist_dr[i] = new history<TH1F>(trees[i] + "_dr", "", null<TH1F>, 1);
        hist_jpt[i] = new history<TH1F>(trees[i] + "_jpt", "", null<TH1F>, 1);
        hist_dr_part1[i] = new history<TH1F>(trees[i] + "_dr_part1", "", null<TH1F>, 1);
        hist_jpt_part1[i] = new history<TH1F>(trees[i] + "_jpt_part1", "", null<TH1F>, 1);
        hist_dr_part2[i] = new history<TH1F>(trees[i] + "_dr_part2", "", null<TH1F>, 1);
        hist_jpt_part2[i] = new history<TH1F>(trees[i] + "_jpt_part2", "", null<TH1F>, 1);

        std::vector<double> *photonPt;
        std::vector<double> *photonEta;
        std::vector<double> *photonPhi;
        std::vector<double> *jetPt;
        std::vector<double> *jetEta;
        std::vector<double> *jetPhi;
        std::vector<double> *jetDj;
        double weight;

        t->SetBranchAddress("photonPt", &photonPt);
        t->SetBranchAddress("photonEta", &photonEta);
        t->SetBranchAddress("photonPhi", &photonPhi);
        t->SetBranchAddress("jetPt", &jetPt);
        t->SetBranchAddress("jetEta", &jetEta);
        t->SetBranchAddress("jetPhi", &jetPhi);
        t->SetBranchAddress("jetDj", &jetDj);
        t->SetBranchAddress("weight", &weight);

        for (int64_t j = 0; j < nentries; ++j) {
            std::cout << t->GetEntry(j) << std::endl;
            if (j < 100) std::cout << (*photonPt)[0] << std::endl;
            if (j < 100) std::cout << (*photonEta)[0] << std::endl;
            if (j < 100) std::cout << (*photonPhi)[0] << std::endl;

            if ((float) (*photonPt)[0] < photon_pt_min) { continue; }
            if ((float) (*photonPt)[0] > photon_pt_max) { continue; }
            if (std::abs((float) (*photonEta)[0]) > photon_eta_abs) { continue; }

            (*hist_nevt[i])[0]->Fill(1., (float) weight);

            for (size_t k = 0; k < jetPt->size(); ++k) {
                if (std::abs((float) (*jetEta)[k]) > jet_eta_abs) { continue; }
                if (!back_to_back((float) (*photonPhi)[0], (float) (*jetPhi)[k], dphi_min_numerator/dphi_min_denominator)) { continue; }

                (*hist_dr_jpt[i])[0]->Fill(mg->index_for(v{(float) (*jetDj)[k], (float) (*jetPt)[k]}), (float) weight);
            }
        }

        hist_dr_jpt[i]->divide(*hist_nevt[i]);

        (*hist_dr[i])[0] = fold((*hist_dr_jpt[i])[0], nullptr, mg, 0, osg);
        (*hist_jpt[i])[0] = fold((*hist_dr_jpt[i])[0], nullptr, mg, 1, osg);
        (*hist_dr_part1[i])[0] = fold((*hist_dr_jpt[i])[0], nullptr, mg, 0, osg_part1);
        (*hist_jpt_part1[i])[0] = fold((*hist_dr_jpt[i])[0], nullptr, mg, 1, osg_part1);
        (*hist_dr_part2[i])[0] = fold((*hist_dr_jpt[i])[0], nullptr, mg, 0, osg_part2);
        (*hist_jpt_part2[i])[0] = fold((*hist_dr_jpt[i])[0], nullptr, mg, 1, osg_part2);

        hist_dr[i]->rename(trees[i] + "_dr"s);
        hist_jpt[i]->rename(trees[i] + "_jpt"s);
        hist_dr_part1[i]->rename(trees[i] + "_dr_part1"s);
        hist_jpt_part1[i]->rename(trees[i] + "_jpt_part1"s);
        hist_dr_part2[i]->rename(trees[i] + "_dr_part2"s);
        hist_jpt_part2[i]->rename(trees[i] + "_jpt_part2"s);
    }

    f->Close();

    /* save histograms */
    in(output, [&]() {
        for (size_t i = 0; i < trees.size(); ++i) {
            hist_nevt[i]->save();
            hist_dr_jpt[i]->save();
            hist_dr[i]->save();
            hist_jpt[i]->save();
            hist_dr_part1[i]->save();
            hist_jpt_part1[i]->save();
            hist_dr_part2[i]->save();
            hist_jpt_part2[i]->save();
        }
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return gather_theory(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
