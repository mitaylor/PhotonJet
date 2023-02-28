#include "../include/lambdas.h"

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
#include "TH2.h"

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename... T>
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

template <typename... T>
void normalise_to_unity(T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1. / obj->Integral("width")); }), 0)... };
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

int data_mc_comparison(char const* config, const char* output) {
    auto conf = new configurer(config);

    auto input_nominal = conf->get<std::string>("input_nominal");
    auto input_old = conf->get<std::string>("input_old");
    auto input_nominal_unfolding = conf->get<std::string>("input_nominal_unfolding");
    auto input_old_unfolding = conf->get<std::string>("input_old_unfolding");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto rdrr = conf->get<std::vector<float>>("drr_range");
    auto rdrg = conf->get<std::vector<float>>("drg_range");
    auto rptr = conf->get<std::vector<float>>("ptr_range");
    auto rptg = conf->get<std::vector<float>>("ptg_range");

    /* create intervals and multivals */
    auto ihf = new interval(dhf);

    auto idrr = new interval("#deltaj"s, rdrr);
    auto iptr = new interval("p_{T}^{j}"s, rptr);

    auto idrg = new interval("#deltaj"s, rdrg);
    auto iptg = new interval("p_{T}^{j}"s, rptg);

    auto mr = new multival(*idrr, *iptr);
    auto mg = new multival(*idrg, *iptg);

    /* offsets */
    std::array<int64_t, 4> osr = { 0, 0, 1, 3 };
    std::array<int64_t, 4> osg = { 0, 0, 1, 1 };

    /* load history objects */
    TFile* f_nominal = new TFile(input_nominal.data(), "read");
    TFile* f_old = new TFile(input_old.data(), "read");
    TFile* f_nominalu = new TFile(input_nominal_unfolding.data(), "read");
    TFile* f_oldu = new TFile(input_old_unfolding.data(), "read");

    /* r */
    auto h_r_nominal = new history<TH1F>(f_nominal, "aa_r");
    h_r_nominal->rename("aa_r_nominal");
    auto h_r_old = new history<TH1F>(f_old, "aa_r");
    h_r_old->rename("aa_r_old");
    auto h_r_nominalu = new history<TH1F>(f_nominalu, "aa_r");
    h_r_nominalu->rename("aa_r_nominalu");
    auto h_r_oldu = new history<TH1F>(f_oldu, "aa_r");
    h_r_oldu->rename("aa_r_oldu");
    
    /* g */
    auto h_g_nominal = new history<TH1F>(f_nominal, "aa_g");
    h_g_nominal->rename("aa_g_nominal");
    auto h_g_old = new history<TH1F>(f_old, "aa_g");
    h_g_old->rename("aa_g_old");
    auto h_g_nominalu = new history<TH1F>(f_nominalu, "aa_g");
    h_g_nominalu->rename("aa_g_nominalu");
    auto h_g_oldu = new history<TH1F>(f_oldu, "aa_g");
    h_g_oldu->rename("aa_g_oldu");

    /* folded */
    auto h_r_nominal_fold0 = new history<TH1F>("h_r_nominal_fold0", ";#deltaj;1/N^{#gammaj}dN/d#deltaj", null<TH1F>, ihf->size());
    auto h_r_old_fold0 = new history<TH1F>("h_r_old_fold0", ";#deltaj;1/N^{#gammaj}dN/d#deltaj", null<TH1F>, ihf->size());
    auto h_r_nominalu_fold0 = new history<TH1F>("h_r_nominalu_fold0", ";#deltaj;1/N^{#gammaj}dN/d#deltaj", null<TH1F>, ihf->size());
    auto h_r_oldu_fold0 = new history<TH1F>("h_r_oldu_fold0", ";#deltaj;1/N^{#gammaj}dN/d#deltaj", null<TH1F>, ihf->size());

    auto h_r_nominal_fold1 = new history<TH1F>("h_r_nominal_fold1", ";jet pT (GeV)", null<TH1F>, ihf->size());
    auto h_r_old_fold1 = new history<TH1F>("h_r_old_fold1", ";jet pT (GeV)", null<TH1F>, ihf->size());
    auto h_r_nominalu_fold1 = new history<TH1F>("h_r_nominalu_fold1", ";jet pT (GeV)", null<TH1F>, ihf->size());
    auto h_r_oldu_fold1 = new history<TH1F>("h_r_oldu_fold1", ";jet pT (GeV)", null<TH1F>, ihf->size());

    auto h_g_nominal_fold0 = new history<TH1F>("h_g_nominal_fold0", ";#deltaj;1/N^{#gammaj}dN/d#deltaj", null<TH1F>, ihf->size());
    auto h_g_old_fold0 = new history<TH1F>("h_g_old_fold0", ";#deltaj;1/N^{#gammaj}dN/d#deltaj", null<TH1F>, ihf->size());
    auto h_g_nominalu_fold0 = new history<TH1F>("h_g_nominalu_fold0", ";#deltaj;1/N^{#gammaj}dN/d#deltaj", null<TH1F>, ihf->size());
    auto h_g_oldu_fold0 = new history<TH1F>("h_g_oldu_fold0", ";#deltaj;1/N^{#gammaj}dN/d#deltaj", null<TH1F>, ihf->size());

    auto h_g_nominal_fold1 = new history<TH1F>("h_g_nominal_fold1", ";jet pT (GeV)", null<TH1F>, ihf->size());
    auto h_g_old_fold1 = new history<TH1F>("h_g_old_fold1", ";jet pT (GeV)", null<TH1F>, ihf->size());
    auto h_g_nominalu_fold1 = new history<TH1F>("h_g_nominalu_fold1", ";jet pT (GeV)", null<TH1F>, ihf->size());
    auto h_g_oldu_fold1 = new history<TH1F>("h_g_oldu_fold1", ";jet pT (GeV)", null<TH1F>, ihf->size());

    // auto h_r_ratio_fold0 = new history<TH1F>("h_r_ratio_fold0", "", null<TH1F>, ihf->size());
    // auto h_r_ratio_fold1 = new history<TH1F>("h_r_ratio_fold1", "", null<TH1F>, ihf->size());
    // auto h_g_ratio_fold0 = new history<TH1F>("h_g_ratio_fold0", "", null<TH1F>, ihf->size());
    // auto h_g_ratio_fold1 = new history<TH1F>("h_g_ratio_fold1", "", null<TH1F>, ihf->size());

    for (int64_t i = 0; i < ihf->size(); ++i) {
        (*h_r_nominal_fold0)[i] = fold((*h_r_nominal)[i], nullptr, mr, 0, osr);
        (*h_r_old_fold0)[i] = fold((*h_r_old)[i], nullptr, mr, 0, osr);
        (*h_r_nominalu_fold0)[i] = fold((*h_r_nominalu)[i], nullptr, mr, 0, osr);
        (*h_r_oldu_fold0)[i] = fold((*h_r_oldu)[i], nullptr, mr, 0, osr);

        (*h_r_nominal_fold1)[i] = fold((*h_r_nominal)[i], nullptr, mr, 1, osr);
        (*h_r_old_fold1)[i] = fold((*h_r_old)[i], nullptr, mr, 1, osr);
        (*h_r_nominalu_fold1)[i] = fold((*h_r_nominalu)[i], nullptr, mr, 1, osr);
        (*h_r_oldu_fold1)[i] = fold((*h_r_oldu)[i], nullptr, mr, 1, osr);

        (*h_g_nominal_fold0)[i] = fold((*h_g_nominal)[i], nullptr, mg, 0, osg);
        (*h_g_old_fold0)[i] = fold((*h_g_old)[i], nullptr, mg, 0, osg);
        (*h_g_nominalu_fold0)[i] = fold((*h_g_nominalu)[i], nullptr, mg, 0, osg);
        (*h_g_oldu_fold0)[i] = fold((*h_g_oldu)[i], nullptr, mg, 0, osg);

        (*h_g_nominal_fold1)[i] = fold((*h_g_nominal)[i], nullptr, mg, 1, osg);
        (*h_g_old_fold1)[i] = fold((*h_g_old)[i], nullptr, mg, 1, osg);
        (*h_g_nominalu_fold1)[i] = fold((*h_g_nominalu)[i], nullptr, mg, 1, osg);
        (*h_g_oldu_fold1)[i] = fold((*h_g_oldu)[i], nullptr, mg, 1, osg);
    }

    h_r_nominal_fold0->rename("h_r_nominal_fold0");
    h_r_old_fold0->rename("h_r_old_fold0");
    h_r_nominalu_fold0->rename("h_r_nominalu_fold0");
    h_r_oldu_fold0->rename("h_r_oldu_fold0");

    h_r_nominal_fold1->rename("h_r_nominal_fold1");
    h_r_old_fold1->rename("h_r_old_fold1");
    h_r_nominalu_fold1->rename("h_r_nominalu_fold1");
    h_r_oldu_fold1->rename("h_r_oldu_fold1");

    h_g_nominal_fold0->rename("h_g_nominal_fold0");
    h_g_old_fold0->rename("h_g_old_fold0");
    h_g_nominalu_fold0->rename("h_g_nominalu_fold0");
    h_g_oldu_fold0->rename("h_g_oldu_fold0");

    h_g_nominal_fold1->rename("h_g_nominal_fold1");
    h_g_old_fold1->rename("h_g_old_fold1");
    h_g_nominalu_fold1->rename("h_g_nominalu_fold1");
    h_g_oldu_fold1->rename("h_g_oldu_fold1");

    normalise_to_unity(h_r_nominal_fold0, h_r_old_fold0, h_r_nominalu_fold0, h_r_oldu_fold0);
    normalise_to_unity(h_r_nominal_fold1, h_r_old_fold1, h_r_nominalu_fold1, h_r_oldu_fold1);
    normalise_to_unity(h_g_nominal_fold0, h_g_old_fold0, h_g_nominalu_fold0, h_g_oldu_fold0);
    normalise_to_unity(h_g_nominal_fold1, h_g_old_fold1, h_g_nominalu_fold1, h_g_oldu_fold1);

    std::cout << __LINE__ << std::endl;

    for (size_t j = 0; j < dhf.size()-1; ++j) {
        // std::cout << (*h_r_ratio_fold0)[j]->GetNbinsX() << " " << (*h_r_old_fold0)[j]->GetNbinsX() << " " << (*h_r_nominal_fold0)[j]->GetNbinsX() << std::endl;
        // std::cout << (*h_r_ratio_fold1)[j]->GetNbinsX() << " " << (*h_r_old_fold1)[j]->GetNbinsX() << " " << (*h_r_nominal_fold1)[j]->GetNbinsX() << std::endl;
        // std::cout << (*h_g_ratio_fold0)[j]->GetNbinsX() << " " << (*h_g_old_fold0)[j]->GetNbinsX() << " " << (*h_g_nominal_fold0)[j]->GetNbinsX() << std::endl;
        // std::cout << (*h_g_ratio_fold1)[j]->GetNbinsX() << " " << (*h_g_old_fold1)[j]->GetNbinsX() << " " << (*h_g_nominal_fold1)[j]->GetNbinsX() << std::endl;

        std::cout << (*h_r_old_fold0)[j]->GetNbinsX() << " " << (*h_r_nominal_fold0)[j]->GetNbinsX() << std::endl;
        std::cout << (*h_r_old_fold1)[j]->GetNbinsX() << " " << (*h_r_nominal_fold1)[j]->GetNbinsX() << std::endl;
        std::cout << (*h_g_old_fold0)[j]->GetNbinsX() << " " << (*h_g_nominal_fold0)[j]->GetNbinsX() << std::endl;
        std::cout << (*h_g_old_fold1)[j]->GetNbinsX() << " " << (*h_g_nominal_fold1)[j]->GetNbinsX() << std::endl;
        // (*h_r_ratio_fold0)[j]->Divide((*h_r_old_fold0)[j], (*h_r_nominal_fold0)[j]);std::cout << __LINE__ << std::endl;
        // (*h_r_ratio_fold1)[j]->Divide((*h_r_old_fold1)[j], (*h_r_nominal_fold1)[j]);std::cout << __LINE__ << std::endl;
        // (*h_g_ratio_fold0)[j]->Divide((*h_g_old_fold0)[j], (*h_g_nominal_fold0)[j]);std::cout << __LINE__ << std::endl;
        // (*h_g_ratio_fold1)[j]->Divide((*h_g_old_fold1)[j], (*h_g_nominal_fold1)[j]);std::cout << __LINE__ << std::endl;
    }

    /* set up figures */
    auto system_tag = "  #sqrt{s_{NN}} = 5.02 TeV, 1.69 nb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;

    auto hf_info = [&](int64_t index) {
        info_text(index, 0.75, "Cent. %i - %i%%", dcent, true); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            TLatex* l = new TLatex();
            l->SetTextAlign(11);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.135, 0.71, "40 < p_{T}^{#gamma} < 200, |#eta^{#gamma}| < 1.44");
            l->DrawLatexNDC(0.135, 0.67, "anti-k_{T} R = 0.3, 20 < p_{T}^{jet} < 200, |#eta^{jet}| < 1.6");
        }
    };

    auto hb = new pencil();
    hb->category("type", "nominal", "old", "nominal_previous", "old_previous");

    hb->alias("nominal", "New Extra MC");
    hb->alias("old", "Old Extra MC");
    hb->alias("nominal_previous", "Nominal w/ old weights");
    hb->alias("old_previous", "Old response matrix");

    /* (1) reco comparisions */
    auto p1 = new paper("vacillate_aa_r_flat_comparison_latest", hb);
    p1->divide(ihf->size(), -1);
    p1->accessory(hf_info);
    p1->accessory(kinematics);
    apply_style(p1, cms, system_tag);
    
    h_r_old->apply([&](TH1* h) { p1->add(h, "nominal"); });
    h_r_nominal->apply([&](TH1* h, int64_t index) { p1->stack(index + 1, h, "old"); });

    auto p2 = new paper("vacillate_aa_r_flat_comparison_nominal", hb);
    p2->divide(ihf->size(), -1);
    p2->accessory(hf_info);
    p2->accessory(kinematics);
    apply_style(p2, cms, system_tag);
    
    h_r_nominal->apply([&](TH1* h) { p2->add(h, "nominal"); });
    h_r_nominalu->apply([&](TH1* h, int64_t index) { p2->stack(index + 1, h, "nominal_previous"); });

    auto p3 = new paper("vacillate_aa_r_flat_comparison_old", hb);
    p3->divide(ihf->size(), -1);
    p3->accessory(hf_info);
    p3->accessory(kinematics);
    apply_style(p3, cms, system_tag);
    
    h_r_old->apply([&](TH1* h) { p3->add(h, "old"); });
    h_r_oldu->apply([&](TH1* h, int64_t index) { p3->stack(index + 1, h, "old_previous"); });

    /* (2) gen comparisions */
    auto p4 = new paper("vacillate_aa_g_flat_comparison_latest", hb);
    p4->divide(ihf->size(), -1);
    p4->accessory(hf_info);
    p4->accessory(kinematics);
    apply_style(p4, cms, system_tag);
    
    h_g_old->apply([&](TH1* h) { p4->add(h, "nominal"); });
    h_g_nominal->apply([&](TH1* h, int64_t index) { p4->stack(index + 1, h, "old"); });

    auto p5 = new paper("vacillate_aa_g_flat_comparison_nominal", hb);
    p5->divide(ihf->size(), -1);
    p5->accessory(hf_info);
    p5->accessory(kinematics);
    apply_style(p5, cms, system_tag);
    
    h_g_nominal->apply([&](TH1* h) { p5->add(h, "nominal"); });
    h_g_nominalu->apply([&](TH1* h, int64_t index) { p5->stack(index + 1, h, "nominal_previous"); });

    auto p6 = new paper("vacillate_aa_g_flat_comparison_old", hb);
    p6->divide(ihf->size(), -1);
    p6->accessory(hf_info);
    p6->accessory(kinematics);
    apply_style(p6, cms, system_tag);
    
    h_g_old->apply([&](TH1* h) { p6->add(h, "old"); });
    h_g_oldu->apply([&](TH1* h, int64_t index) { p6->stack(index + 1, h, "old_previous"); });
    
    /* (3) reco dr comparisions */
    auto p7 = new paper("vacillate_aa_r_dr_comparison_latest", hb);
    p7->divide(ihf->size(), -1);
    p7->accessory(hf_info);
    p7->accessory(kinematics);
    apply_style(p7, cms, system_tag);
    
    h_r_old_fold0->apply([&](TH1* h) { p7->add(h, "nominal"); });
    h_r_nominal_fold0->apply([&](TH1* h, int64_t index) { p7->stack(index + 1, h, "old"); });

    auto p8 = new paper("vacillate_aa_r_dr_comparison_nominal", hb);
    p8->divide(ihf->size(), -1);
    p8->accessory(hf_info);
    p8->accessory(kinematics);
    apply_style(p8, cms, system_tag);
    
    h_r_nominal_fold0->apply([&](TH1* h) { p8->add(h, "nominal"); });
    h_r_nominalu_fold0->apply([&](TH1* h, int64_t index) { p8->stack(index + 1, h, "nominal_previous"); });

    auto p9 = new paper("vacillate_aa_r_dr_comparison_old", hb);
    p9->divide(ihf->size(), -1);
    p9->accessory(hf_info);
    p9->accessory(kinematics);
    apply_style(p9, cms, system_tag);
    
    h_r_old_fold0->apply([&](TH1* h) { p9->add(h, "old"); });
    h_r_oldu_fold0->apply([&](TH1* h, int64_t index) { p9->stack(index + 1, h, "old_previous"); });

    /* (4) gen dr comparisions */
    auto p10 = new paper("vacillate_aa_g_dr_comparison_latest", hb);
    p10->divide(ihf->size(), -1);
    p10->accessory(hf_info);
    p10->accessory(kinematics);
    apply_style(p10, cms, system_tag);
    
    h_g_old_fold0->apply([&](TH1* h) { p10->add(h, "nominal"); });
    h_g_nominal_fold0->apply([&](TH1* h, int64_t index) { p10->stack(index + 1, h, "old"); });

    auto p11 = new paper("vacillate_aa_g_dr_comparison_nominal", hb);
    p11->divide(ihf->size(), -1);
    p11->accessory(hf_info);
    p11->accessory(kinematics);
    apply_style(p11, cms, system_tag);
    
    h_g_nominal_fold0->apply([&](TH1* h) { p11->add(h, "nominal"); });
    h_g_nominalu_fold0->apply([&](TH1* h, int64_t index) { p11->stack(index + 1, h, "nominal_previous"); });

    auto p12 = new paper("vacillate_aa_g_dr_comparison_old", hb);
    p12->divide(ihf->size(), -1);
    p12->accessory(hf_info);
    p12->accessory(kinematics);
    apply_style(p12, cms, system_tag);
    
    h_g_old_fold0->apply([&](TH1* h) { p12->add(h, "old"); });
    h_g_oldu_fold0->apply([&](TH1* h, int64_t index) { p12->stack(index + 1, h, "old_previous"); });

    /* (5) reco jtpt comparisions */
    auto p13 = new paper("vacillate_aa_r_jtpt_comparison_latest", hb);
    p13->divide(ihf->size(), -1);
    p13->accessory(hf_info);
    p13->accessory(kinematics);
    apply_style(p13, cms, system_tag);
    
    h_r_old_fold1->apply([&](TH1* h) { p13->add(h, "nominal"); });
    h_r_nominal_fold1->apply([&](TH1* h, int64_t index) { p13->stack(index + 1, h, "old"); });

    auto p14 = new paper("vacillate_aa_r_jtpt_comparison_nominal", hb);
    p14->divide(ihf->size(), -1);
    p14->accessory(hf_info);
    p14->accessory(kinematics);
    apply_style(p14, cms, system_tag);
    
    h_r_nominal_fold1->apply([&](TH1* h) { p14->add(h, "nominal"); });
    h_r_nominalu_fold1->apply([&](TH1* h, int64_t index) { p14->stack(index + 1, h, "nominal_previous"); });

    auto p15 = new paper("vacillate_aa_r_jtpt_comparison_old", hb);
    p15->divide(ihf->size(), -1);
    p15->accessory(hf_info);
    p15->accessory(kinematics);
    apply_style(p15, cms, system_tag);
    
    h_r_old_fold1->apply([&](TH1* h) { p15->add(h, "old"); });
    h_r_oldu_fold1->apply([&](TH1* h, int64_t index) { p15->stack(index + 1, h, "old_previous"); });

    /* (6) gen jtpt comparisions */
    auto p16 = new paper("vacillate_aa_g_jtpt_comparison_latest", hb);
    p16->divide(ihf->size(), -1);
    p16->accessory(hf_info);
    p16->accessory(kinematics);
    apply_style(p16, cms, system_tag);
    
    h_g_old_fold1->apply([&](TH1* h) { p16->add(h, "nominal"); });
    h_g_nominal_fold1->apply([&](TH1* h, int64_t index) { p16->stack(index + 1, h, "old"); });

    auto p17 = new paper("vacillate_aa_g_jtpt_comparison_nominal", hb);
    p17->divide(ihf->size(), -1);
    p17->accessory(hf_info);
    p17->accessory(kinematics);
    apply_style(p17, cms, system_tag);
    
    h_g_nominal_fold1->apply([&](TH1* h) { p17->add(h, "nominal"); });
    h_g_nominalu_fold1->apply([&](TH1* h, int64_t index) { p17->stack(index + 1, h, "nominal_previous"); });

    auto p18 = new paper("vacillate_aa_g_jtpt_comparison_old", hb);
    p18->divide(ihf->size(), -1);
    p18->accessory(hf_info);
    p18->accessory(kinematics);
    apply_style(p18, cms, system_tag);
    
    h_g_old_fold1->apply([&](TH1* h) { p18->add(h, "old"); });
    h_g_oldu_fold1->apply([&](TH1* h, int64_t index) { p18->stack(index + 1, h, "old_previous"); });

    // /* (7) ratio plots */
    // auto p19 = new paper("vacillate_aa_r_dr_ratio", hb);
    // p19->divide(ihf->size(), -1);
    // p19->accessory(hf_info);
    // p19->accessory(kinematics);
    // apply_style(p19, cms, system_tag);
    // title(std::bind(rename_axis, _1, "Old Extra MC / New Extra MC"), h_r_ratio_fold0);
    // h_r_ratio_fold0->apply([&](TH1* h) { p19->add(h, "old"); });

    // auto p20 = new paper("vacillate_aa_r_jtpt_ratio", hb);
    // p20->divide(ihf->size(), -1);
    // p20->accessory(hf_info);
    // p20->accessory(kinematics);
    // apply_style(p20, cms, system_tag);
    // title(std::bind(rename_axis, _1, "Old Extra MC / New Extra MC"), h_r_ratio_fold1);
    // h_r_ratio_fold1->apply([&](TH1* h) { p20->add(h, "old"); });

    // auto p21 = new paper("vacillate_aa_g_dr_ratio", hb);
    // p21->divide(ihf->size(), -1);
    // p21->accessory(hf_info);
    // p21->accessory(kinematics);
    // apply_style(p21, cms, system_tag);
    // title(std::bind(rename_axis, _1, "Old Extra MC / New Extra MC"), h_g_ratio_fold0);
    // h_g_ratio_fold0->apply([&](TH1* h) { p21->add(h, "old"); });

    // auto p22 = new paper("vacillate_aa_g_jtpt_ratio", hb);
    // p22->divide(ihf->size(), -1);
    // p22->accessory(hf_info);
    // p22->accessory(kinematics);
    // apply_style(p22, cms, system_tag);
    // title(std::bind(rename_axis, _1, "Old Extra MC / New Extra MC"), h_g_ratio_fold1);
    // h_g_ratio_fold1->apply([&](TH1* h) { p22->add(h, "old"); });

    hb->sketch();

    p1->draw("pdf");
    p2->draw("pdf");
    p3->draw("pdf");
    p4->draw("pdf");
    p5->draw("pdf");
    p6->draw("pdf");
    p7->draw("pdf");
    p8->draw("pdf");
    p9->draw("pdf");
    p10->draw("pdf");
    p11->draw("pdf");
    p12->draw("pdf");
    p13->draw("pdf");
    p14->draw("pdf");
    p15->draw("pdf");
    p16->draw("pdf");
    p17->draw("pdf");
    p18->draw("pdf");
    // p19->draw("pdf");
    // p20->draw("pdf");
    // p21->draw("pdf");
    // p22->draw("pdf");

    in(output, [&]() {
        h_r_nominal->save();
        h_r_old->save();
        h_r_nominalu->save();
        h_r_oldu->save();
        h_g_nominal->save();
        h_g_old->save();
        h_g_nominalu->save();
        h_g_oldu->save();
        h_r_nominal_fold0->save();
        h_r_old_fold0->save();
        h_r_nominalu_fold0->save();
        h_r_oldu_fold0->save();
        h_r_nominal_fold1->save();
        h_r_old_fold1->save();
        h_r_nominalu_fold1->save();
        h_r_oldu_fold1->save();
        h_g_nominal_fold0->save();
        h_g_old_fold0->save();
        h_g_nominalu_fold0->save();
        h_g_oldu_fold0->save();
        h_g_nominal_fold1->save();
        h_g_old_fold1->save();
        h_g_nominalu_fold1->save();
        h_g_oldu_fold1->save();
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return data_mc_comparison(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
