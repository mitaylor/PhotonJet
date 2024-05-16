#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/maglev.h"
#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/overflow_angles.h"

#include "TF1.h"
#include "TFile.h"
#include "TLatex.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TTree.h"

#include <algorithm>
#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

double f_double_sided_crystal_ball(double* x, double* params) {
    double x0 = x[0];
    /* gaussian */
    double n = params[0];
    double mean = params[1];
    double sigma = params[2];
    /* transition */
    double a1 = params[3];
    double n1 = params[4];
    double a2 = params[5];
    double n2 = params[6];

    double u = (x0 - mean) / sigma;
    double A1 = TMath::Power(n1 / TMath::Abs(a1), n1) * TMath::Exp(-a1 * a1 / 2);
    double A2 = TMath::Power(n2 / TMath::Abs(a2), n2) * TMath::Exp(-a2 * a2 / 2);
    double B1 = n1 / TMath::Abs(a1) - TMath::Abs(a1);
    double B2 = n2 / TMath::Abs(a2) - TMath::Abs(a2);

    if (u < -a1) return n * A1 * TMath::Power(B1 - u, -n1);
    if (u < a2) return n * TMath::Exp(-u * u / 2);
    return n * A2 * TMath::Power(B2 + u, -n2);
}

// double f_cms_shape(double* x, double* params) {
//     double x0 = x[0];
//     double n = params[0];
//     double alpha = params[1];
//     double beta = params[2];
//     double gamma = params[3];

//     return n * TMath::Erfc((alpha - x0) * beta)
//         * TMath::Exp((91.1876 - x0) * gamma);
// }

double f_combined(double* x, double* params) {
    return f_double_sided_crystal_ball(x, params); //+ f_cms_shape(x, &params[7]);
}

static std::string index_to_string(int64_t i, int64_t j) {
    return std::to_string(i) + "_"s + std::to_string(j);
}

int64_t inosculate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::vector<std::string>>("input");
    auto tag = conf->get<std::string>("tag");
    auto use_er = conf->get<bool>("use_er");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    /* selections */
    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");

    auto heavyion = sel->get<bool>("heavyion");
    auto const hovere_max = sel->get<float>("hovere_max");

    auto hf_min = dhf.front();

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* prepare histograms */
    auto ihf = new interval(dhf);
    std::vector<int64_t> shape = { 1, ihf->size() };

    auto imass = new interval("mass (GeV/c^{2})"s, 30, 60., 120.);
    auto fmass = std::bind(&interval::book<TH1F>, imass, _1, _2, _3);

    auto minv = new history<TH1F>("mass"s, "counts"s, fmass, shape);

    /* load input */
    for (auto const& file : input) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto p = new pjtree(false, true, false, t, { 1, 1, 1, 1, 0, 0, 0, 0, 0 });
        int64_t nentries = t->GetEntries();

        for (int64_t i = 0; i < nentries; ++i) {
            if (i % 100000 == 0) { printf("entry: %li/%li\n", i, nentries); }
            
            t->GetEntry(i);
            // if ((*p->accepts)[0] == 0) { continue; }

            if (p->hiHF <= hf_min) { continue; }
            auto hf_x = ihf->index_for(p->hiHF);
            if (hf_x == ihf->size()) { continue; }

            std::vector<float> masses;

            for (int64_t j = 0; j < p->nPho; ++j) {
                if ((*p->phoEt)[j] < 20) //15
                    continue;
                if (std::abs((*p->phoEta)[j]) > 1.4442)
                    continue;
                if (heavyion && in_pho_failure_region(p, j))
                    continue;

                if ((*p->phoHoverE)[j] > hovere_max) { continue; }
                if ((*p->phoSigmaIEtaIEta_2012)[j] > 0.011) { continue; }

                auto pho_phi = convert_radian((*p->phoPhi)[j]);

                bool electron = false;
                for (int64_t l = 0; l < p->nEle; ++l) {
                    if (std::abs((*p->eleEta)[l]) > 1.4442) { continue; }

                    auto deta = (*p->phoEta)[j] - (*p->eleEta)[l];
                    if (deta > 0.1) { continue; }

                    auto ele_phi = convert_radian((*p->elePhi)[l]);
                    auto dphi = revert_radian(pho_phi - ele_phi);
                    auto dr2 = deta * deta + dphi * dphi;

                    if (dr2 < 0.01 && passes_electron_id<
                                det::barrel, wp::loose, pjtree
                            >(p, l, heavyion)) {
                        electron = true; break; }
                }

                if (!electron) { continue; }

                for (int64_t k = j + 1; k < p->nPho; ++k) {
                    if ((*p->phoEt)[k] < 20) //15
                        continue;
                    if (std::abs((*p->phoEta)[k]) > 1.4442)
                        continue;
                    if (heavyion && in_pho_failure_region(p, k))
                        continue;

                    if ((*p->phoHoverE)[k] > hovere_max) { continue; }
                    if ((*p->phoSigmaIEtaIEta_2012)[k] > 0.011) { continue; }

                    auto pho_phi = convert_radian((*p->phoPhi)[k]);

                    bool electron = false;
                    for (int64_t l = 0; l < p->nEle; ++l) {
                        if (std::abs((*p->eleEta)[l]) > 1.4442) { continue; }

                        auto deta = (*p->phoEta)[k] - (*p->eleEta)[l];
                        if (deta > 0.1) { continue; }

                        auto ele_phi = convert_radian((*p->elePhi)[l]);
                        auto dphi = revert_radian(pho_phi - ele_phi);
                        auto dr2 = deta * deta + dphi * dphi;

                        if (dr2 < 0.01 && passes_electron_id<
                                    det::barrel, wp::loose, pjtree
                                >(p, l, heavyion)) {
                            electron = true; break; }
                    }

                    if (!electron) { continue; }

                    float phoEt_j = (*p->phoEt)[j];
                    if ((*p->phoEt)[j] > 30 && heavyion && use_er) phoEt_j = (*p->phoEtErNew)[j];
                    if ((*p->phoEt)[j] > 30 && !heavyion && use_er) phoEt_j = (*p->phoEtEr)[j];

                    float phoEt_k = (*p->phoEt)[k];
                    if ((*p->phoEt)[k] > 30 && heavyion && use_er) phoEt_k = (*p->phoEtErNew)[k];
                    if ((*p->phoEt)[k] > 30 && !heavyion && use_er) phoEt_k = (*p->phoEtEr)[k];

                    /* double electron invariant mass */
                    auto mass = std::sqrt(ml_invariant_mass<coords::collider>(
                        phoEt_j,
                        (*p->phoEta)[j],
                        (*p->phoPhi)[j],
                        0.f,
                        phoEt_k,
                        (*p->phoEta)[k],
                        (*p->phoPhi)[k],
                        0.f));

                    masses.push_back(mass);
                }

                
            }

            if (masses.empty()) { continue; }

            if (masses.size() > 1) {
                std::sort(masses.begin(), masses.end(), [](float a, float b) {
                    return std::abs(a - 91.1876) < std::abs(b - 91.1876); });
            }

            (*minv)[x{0, hf_x}]->Fill(masses[0], 1);
        }
    }

    TF1** fits[1] = { new TF1*[ihf->size()] };
    // TF1** fbkg[1] = { new TF1*[ihf->size()] };

    for (int64_t i = 0; i < 1; ++i) {
        for (int64_t j = 0; j < ihf->size(); ++j) {
            auto istr = index_to_string(i, j);

            fits[i][j] = new TF1(("f_"s + istr).data(),
                f_combined, 60, 120, 11);

            auto parameters = conf->get<std::vector<double>>("pars_"s + istr);
            fits[i][j]->SetParameters(parameters.data());
            fits[i][j]->SetParLimits(4, 0, 10);

            (*minv)[x{i, j}]->Fit(("f_"s + istr).data(), "L", "", 61, 119);
            (*minv)[x{i, j}]->Fit(("f_"s + istr).data(), "LM", "", 61, 119);

            conf->set<float>("mean_"s + istr, fits[i][j]->GetParameter(1));
            conf->set<float>("sigma_"s + istr, fits[i][j]->GetParameter(2));

            // fbkg[i][j] = new TF1(("f_bkg_"s + istr).data(),
            //    f_cms_shape, 60, 120, 4);

            double pars[11] = { 0 };
            fits[i][j]->GetParameters(pars);
            // fbkg[i][j]->SetParameters(&pars[7]);
            // fbkg[i][j]->SetLineStyle(7);
        }
    }

    std::vector<std::string> types = { "bb" };

    /* lambda to display mean, sigma from fit, background function */
    auto fit_info = [&](int64_t index) {
        auto indices = minv->indices_for(index - 1);
        auto i = indices[0];
        auto j = indices[1];

        TLatex* info = new TLatex();
        info->SetTextFont(43);
        info->SetTextSize(11);

        char buffer[128];

        auto mean = conf->get<float>("mean_"s + index_to_string(i, j));
        sprintf(buffer, "mean: %.2f", mean);
        info->DrawLatexNDC(0.675, 0.78, buffer);
        auto sigma = conf->get<float>("sigma_"s + index_to_string(i, j));
        sprintf(buffer, "sigma: %.2f", sigma);
        info->DrawLatexNDC(0.675, 0.75, buffer);

        // fbkg[i][j]->Draw("same");
    };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, 0.85, "p_{T,1}^{#gamma} > 20 GeV, p_{T,2}^{#gamma} > 20 GeV, |#eta^{#gamma}| < 1.44");
        }
    };

    auto system_tag = "  #sqrt{s_{NN}} = 5.02 TeV"s;
    system_tag += (heavyion) ? ", 1.69 nb^{-1}"s : ", 302 pb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;

    /* prepare plots */
    auto hb = new pencil();
    hb->category("type", "bb");

    hb->alias("bb", "EB #otimes EB");

    auto c1 = new paper(tag + "_ele_mass", hb);
    apply_style(c1, cms, system_tag);
    c1->legend(std::bind(coordinates, 0.135, 0.4, 0.75, 0.04));
    if (heavyion) c1->accessory(std::bind(hf_info, _1, 0.75));
    c1->accessory(fit_info);
    c1->accessory(kinematics);
    c1->divide(ihf->size(), 1);

    for (int64_t i = 0; i < 1; ++i) {
        for (int64_t j = 0; j < ihf->size(); ++j)
            c1->add((*minv)[x{i, j}], types[i]);
    }

    hb->sketch();
    c1->draw("pdf");

    /* save output */
    in(output, [&]() { minv->save(tag); });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return inosculate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
