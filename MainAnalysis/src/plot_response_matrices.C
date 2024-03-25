#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TDecompSVD.h"
#include "TGraphAsymmErrors.h"
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
    auto osg = sel->get<std::vector<int64_t>>("osg");

    auto rdrr = sel->get<std::vector<float>>("drr_range");
    auto rptr = sel->get<std::vector<float>>("ptr_range");

    auto rdrg = sel->get<std::vector<float>>("drg_range");
    auto rptg = sel->get<std::vector<float>>("ptg_range");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");

    auto bpho_pt = sel->get<std::vector<float>>("photon_pt_bounds");
    auto bjet_pt = sel->get<std::vector<float>>("jet_pt_bounds");

    auto ihf = new interval(dhf);

    auto idrr = new interval("#deltaj", rdrr);
    auto iptr = new interval("p_{T}^{j}"s, rptr);

    auto idrg = new interval("#deltaj", rdrg);
    auto iptg = new interval("p_{T}^{j}"s, rptg);

    auto mr = new multival(*idrr, *iptr);
    auto mg = new multival(*idrg, *iptg);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input and victims */
    TFile* fi = new TFile((base + input).data(), "read");
    auto matrices = new history<TH2F>(fi, tag + "_c");
    auto gen = new history<TH1F>(fi, tag + "_g");
    auto gen_proj = new history<TH1F>(fi, tag + "_proj_g");
    auto reco = new history<TH1F>(fi, tag + "_r");
    auto reco_proj = new history<TH1F>(fi, tag + "_proj_r");
    auto photon = new history<TH2F>(fi, tag + "_ppt");

    TFile* fv = new TFile((base + victim).data(), "read");
    auto victims = new history<TH1F>(fv, label);

    auto victims_fold0 = new history<TH1F>(label + "_victims_fold0", "", null<TH1F>, ihf->size());
    auto victims_fold1 = new history<TH1F>(label + "_victims_fold1", "", null<TH1F>, ihf->size());

    auto reco_fold0 = new history<TH1F>(label + "_reco_fold0", "", null<TH1F>, ihf->size());
    auto reco_fold1 = new history<TH1F>(label + "_reco_fold1", "", null<TH1F>, ihf->size());

    auto gen_fold0 = new history<TH1F>(label + "_gen_fold0", "", null<TH1F>, ihf->size());
    auto gen_fold1 = new history<TH1F>(label + "_gen_fold1", "", null<TH1F>, ihf->size());

    auto gen_proj_fold0 = new history<TH1F>(label + "_gen_proj_fold0", "", null<TH1F>, ihf->size());
    auto gen_proj_fold1 = new history<TH1F>(label + "_gen_proj_fold1", "", null<TH1F>, ihf->size());

    auto reco_proj_fold0 = new history<TH1F>(label + "_reco_proj_fold0", "", null<TH1F>, ihf->size());
    auto reco_proj_fold1 = new history<TH1F>(label + "_reco_proj_fold1", "", null<TH1F>, ihf->size());

    TGraphAsymmErrors* eff[4];
    TGraphAsymmErrors* eff_fold0[4];
    TGraphAsymmErrors* eff_fold1[4];

    TGraphAsymmErrors* pur[4];
    TGraphAsymmErrors* pur_fold0[4];
    TGraphAsymmErrors* pur_fold1[4];

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

    gStyle->SetPalette(kInvertedDarkBodyRadiator);

    std::vector<paper*> cs(17, nullptr);
    zip([&](paper*& c, std::string const& title) {
        c = new paper(set + "_unfolding_dj_" + tag + "_" + type + "_" + title, hb);
        apply_style(c, "", "");
        if (heavyion) c->accessory(hf_info);
        c->accessory(kinematics);
        c->accessory(blurb);
        c->divide(ihf->size()/2, -1);
    }, cs, (std::initializer_list<std::string> const) {
        "matrices"s, "victims"s, "victims_fold0"s, "victims_fold1"s, "gen"s, "reco"s, "photon"s, "reco_fold0", "reco_fold1", "gen_fold0", "gen_fold1", "eff", "eff_fold0", "eff_fold1", "pur", "pur_fold0", "pur_fold1"});

    cs[2]->format(std::bind(default_formatter, _1, -2, 27));
    cs[3]->format(std::bind(default_formatter, _1, -0.001, 0.02));
    cs[7]->format(std::bind(default_formatter, _1, -2, 27));
    cs[8]->format(std::bind(default_formatter, _1, -0.001, 0.02));
    cs[6]->format(std::bind(default_formatter, _1, 40, 220));

    cs[0]->set(paper::flags::logz);  

    for (int64_t i = 0; i < ihf->size(); ++i) {
        if ((*gen_proj)[i]->GetMaximum() > (*gen)[i]->GetMaximum()) {
            (*gen_proj)[i]->Scale((*gen)[i]->GetMaximum()/(*gen_proj)[i]->GetMaximum());
        }

        if ((*reco_proj)[i]->GetMaximum() > (*reco)[i]->GetMaximum()) {
            (*reco_proj)[i]->Scale((*reco)[i]->GetMaximum()/(*reco_proj)[i]->GetMaximum());
        }
        
        /* folds */
        (*victims_fold0)[i] = fold((*victims)[i], nullptr, mr, 0, osr);
        (*victims_fold1)[i] = fold((*victims)[i], nullptr, mr, 1, osr);

        (*reco_fold0)[i] = fold((*reco)[i], nullptr, mr, 0, osr);
        (*reco_fold1)[i] = fold((*reco)[i], nullptr, mr, 1, osr);

        (*gen_fold0)[i] = fold((*gen)[i], nullptr, mg, 0, osg);
        (*gen_fold1)[i] = fold((*gen)[i], nullptr, mg, 1, osg);

        (*gen_proj_fold0)[i] = fold((*gen_proj)[i], nullptr, mg, 0, osg);
        (*gen_proj_fold1)[i] = fold((*gen_proj)[i], nullptr, mg, 1, osg);

        (*reco_proj_fold0)[i] = fold((*reco_proj)[i], nullptr, mr, 0, osr);
        (*reco_proj_fold1)[i] = fold((*reco_proj)[i], nullptr, mr, 1, osr);
std::cout << __LINE__ << std::endl;
        eff[i] = new TGraphAsymmErrors((*gen_proj)[i], (*gen)[i], "cl=0.683 b(1,1) mode");std::cout << __LINE__ << std::endl;
        eff_fold0[i] = new TGraphAsymmErrors((*gen_proj_fold0)[i], (*gen_fold0)[i], "cl=0.683 b(1,1) mode");std::cout << __LINE__ << std::endl;
        eff_fold1[i] = new TGraphAsymmErrors((*gen_proj_fold1)[i], (*gen_fold1)[i], "cl=0.683 b(1,1) mode");std::cout << __LINE__ << std::endl;
std::cout << __LINE__ << std::endl; std::cout << (*reco_proj)[i]->GetNbinsX() <<  " " << (*reco)[i]->GetNbinsX() << std::endl;
        pur[i] = new TGraphAsymmErrors((*reco_proj)[i], (*reco)[i], "cl=0.683 b(1,1) mode");std::cout << __LINE__ << std::endl;
        pur_fold0[i] = new TGraphAsymmErrors((*reco_proj_fold0)[i], (*reco_fold0)[i], "cl=0.683 b(1,1) mode");std::cout << __LINE__ << std::endl;
        pur_fold1[i] = new TGraphAsymmErrors((*reco_proj_fold1)[i], (*reco_fold1)[i], "cl=0.683 b(1,1) mode");std::cout << __LINE__ << std::endl;
std::cout << __LINE__ << std::endl;
        eff[i]->SetName(("eff_"s + to_text(i)).data());
        eff_fold0[i]->SetName(("eff_fold0_"s + to_text(i)).data());
        eff_fold1[i]->SetName(("eff_fold1_"s + to_text(i)).data());

        pur[i]->SetName(("pur_"s + to_text(i)).data());
        pur_fold0[i]->SetName(("pur_fold0_"s + to_text(i)).data());
        pur_fold1[i]->SetName(("pur_fold1_"s + to_text(i)).data());

        auto frame_eff = frame((*gen_proj)[i]->GetXaxis(), (*gen_proj)[i]->GetYaxis());
        auto frame_eff_fold0 = frame((*gen_proj_fold0)[i]->GetXaxis(), (*gen_proj_fold0)[i]->GetYaxis());
        auto frame_eff_fold1 = frame((*gen_proj_fold1)[i]->GetXaxis(), (*gen_proj_fold1)[i]->GetYaxis());
        
        auto frame_pur = frame((*reco_proj)[i]->GetXaxis(), (*reco_proj)[i]->GetYaxis());
        auto frame_pur_fold0 = frame((*reco_proj_fold0)[i]->GetXaxis(), (*reco_proj_fold0)[i]->GetYaxis());
        auto frame_pur_fold1 = frame((*reco_proj_fold1)[i]->GetXaxis(), (*reco_proj_fold1)[i]->GetYaxis());
        
        frame_eff->SetName(("frame_eff_"s + to_text(i)).data());
        frame_eff_fold0->SetName(("frame_eff_fold0_"s + to_text(i)).data());
        frame_eff_fold1->SetName(("frame_eff_fold1_"s + to_text(i)).data());

        frame_pur->SetName(("frame_pur_"s + to_text(i)).data());
        frame_pur_fold0->SetName(("frame_pur_fold0_"s + to_text(i)).data());
        frame_pur_fold1->SetName(("frame_pur_fold1_"s + to_text(i)).data());

        frame_eff->SetMaximum(1.4);
        frame_eff->GetYaxis()->SetTitle("Bin Efficiency");
        frame_eff->GetXaxis()->SetTitle("Gen Bin Index");

        frame_pur->SetMaximum(1.4);
        frame_pur->GetYaxis()->SetTitle("Bin Purity");
        frame_pur->GetXaxis()->SetTitle("Reco Bin Index");

        frame_eff_fold0->SetMaximum(1.4);
        frame_eff_fold0->GetYaxis()->SetTitle("Bin Efficiency");
        frame_eff_fold0->GetXaxis()->SetTitle("#deltaj");

        frame_pur_fold0->SetMaximum(1.4);
        frame_pur_fold0->GetYaxis()->SetTitle("Bin Purity");
        frame_pur_fold0->GetXaxis()->SetTitle("#deltaj");

        frame_eff_fold1->SetMaximum(1.4);
        frame_eff_fold1->GetYaxis()->SetTitle("Bin Efficiency");
        frame_eff_fold1->GetXaxis()->SetTitle("Jet p_{T}");

        frame_pur_fold1->SetMaximum(1.4);
        frame_pur_fold1->GetYaxis()->SetTitle("Bin Purity");
        frame_pur_fold1->GetXaxis()->SetTitle("Jet p_{T}");

        /* figures */
        cs[0]->add((*matrices)[i]);
        cs[0]->adjust((*matrices)[i], "colz", "");

        (*matrices)[i]->GetZaxis()->SetTitle("");

        cs[1]->add((*victims)[i]);
        cs[2]->add((*victims_fold0)[i]);
        cs[3]->add((*victims_fold1)[i]);
        cs[4]->add((*gen)[i]);
        cs[5]->add((*reco)[i]);
        cs[7]->add((*reco_fold0)[i]);
        cs[8]->add((*reco_fold1)[i]);
        cs[9]->add((*gen_fold0)[i]);
        cs[10]->add((*gen_fold1)[i]);
        cs[11]->add(frame_eff);
        cs[11]->stack((eff)[i]);
        cs[12]->add(frame_eff_fold0);
        cs[12]->stack((eff_fold0)[i]);
        cs[13]->add(frame_eff_fold1);
        cs[13]->stack((eff_fold1)[i]);
        cs[14]->add(frame_pur);
        cs[14]->stack((pur)[i]);
        cs[15]->add(frame_pur_fold0);
        cs[15]->stack((pur_fold0)[i]);
        cs[16]->add(frame_pur_fold1);
        cs[16]->stack((pur_fold1)[i]);

        if (photon != nullptr) {
            cs[6]->add((*photon)[i]);
            cs[6]->adjust((*photon)[i], "colz", "");
        }
    };

    hb->sketch();

    for (size_t i = 0; i < cs.size(); ++i) {
        if (i != 6) {
            cs[i]->draw("pdf");
        }
    }
    
    if (photon != nullptr) {
        cs[6]->draw("pdf");
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return plot_unfolding_inputs(argv[1], argv[2]);

    printf("usage: %s [config] [selections]\n", argv[0]);
    return 1;
}