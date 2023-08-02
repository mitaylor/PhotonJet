#include "../include/lambdas.h"
#include "../include/pjtree.h"

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
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

#include "TDirectory.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TChain.h"

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

void GetFiles(char const* input, std::vector<std::string>& files) {
    TSystemDirectory dir(input, input);
    TList *list = dir.GetListOfFiles();

    if (list) {
        TSystemFile *file;
        std::string fname;
        TIter next(list);
        while ((file = (TSystemFile*) next())) {
            fname = file->GetName();

            if (file->IsDirectory() && (fname.find(".") == std::string::npos)) {
                std::string newDir = std::string(input) + fname + "/";
                GetFiles(newDir.c_str(), files);
            }
            else if ((fname.find(".root") != std::string::npos)) {
                files.push_back(std::string(input) + fname);
                std::cout << files.back() << std::endl;
            }
        }
    }

    return;
}

void FillChain(TChain& chain, std::vector<std::string>& files) {
    for (auto file : files) {
        chain.Add(file.c_str());
    }
}

enum ip { incl, cent, peri, nip };
enum det { barrel, endcap, ndet };
enum wp { veto, loose, medium, tight, claustro, nwp };
enum var { hoe, see, deta, dphi, eop, nele,
        /* hoe, see, */ iso = 2, npho};

constexpr float ecuts[ip::nip][det::ndet][wp::nwp][var::nele] = {
    {    /* ip::incl */
        {    /* det::barrel */
            {    0.05, 0.01020, 0.00327, 0.06055, 0.52688 }, /* wp::veto */
            {    0.05, 0.00997, 0.00272, 0.03419, 0.03101 }, /* wp::loose */
            {    0.05, 0.00948, 0.00272, 0.03415, 0.02762 }, /* wp::medium */
            {    0.05, 0.00922, 0.00268, 0.02609, 0.02711 }, /* wp::tight */
            {    -1.,     -1.,     -1.,     -1.,     -1. }  /* wp::claustro */
        }, { /* det::endcap */
            {    0.05, 0.02957, 0.00490, 0.09622, 0.14600 }, /* wp::veto */
            {    0.05, 0.02895, 0.00488, 0.02983, 0.01873 }, /* wp::loose */
            {    0.05, 0.02685, 0.00379, 0.02812, 0.01614 }, /* wp::medium */
            {    0.05, 0.02679, 0.00377, 0.02762, 0.00472 }, /* wp::tight */
            {    -1.,     -1.,     -1.,     -1.,     -1. }  /* wp::claustro */
        }
    }, { /* ip::cent */
        {    /* det::barrel */
            { 0.2733, 0.0147, 0.0041, 0.0853, 0.0367 }, /* wp::veto */
            { 0.1616, 0.0135, 0.0038, 0.0376, 0.0177 }, /* wp::loose */
            { 0.1589, 0.0116, 0.0037, 0.0224, 0.0173 }, /* wp::medium */
            { 0.1459, 0.0104, 0.0029, 0.0206, 0.0105 }, /* wp::tight */
            {    -1.,    -1.,    -1.,    -1.,    -1. }  /* wp::claustro */
        }, { /* det::endcap */
            { 0.1898, 0.0480, 0.0097, 0.2348, 0.0300 }, /* wp::veto */
            { 0.1317, 0.0466, 0.0063, 0.1186, 0.0201 }, /* wp::loose */
            { 0.1092, 0.0418, 0.0062, 0.0373, 0.0133 }, /* wp::medium */
            { 0.0925, 0.0358, 0.0051, 0.0266, 0.0065 }, /* wp::tight */
            {    -1.,    -1.,    -1.,    -1.,    -1. }  /* wp::claustro */
        }
    }, { /* ip::peri */
        {    /* det::barrel */
            { 0.1814, 0.0113, 0.0037, 0.1280, 0.1065 }, /* wp::veto */
            { 0.1268, 0.0107, 0.0035, 0.0327, 0.0774 }, /* wp::loose */
            { 0.0311, 0.0101, 0.0033, 0.0210, 0.0701 }, /* wp::medium */
            { 0.0067, 0.0099, 0.0026, 0.0170, 0.0077 }, /* wp::tight */
            {    -1.,    -1.,    -1.,    -1.,    -1. }  /* wp::claustro */
        }, { /* det::endcap */
            { 0.1138, 0.0376, 0.0074, 0.2085, 0.0237 }, /* wp::veto */
            { 0.0977, 0.0339, 0.0067, 0.0838, 0.0193 }, /* wp::loose */
            { 0.0810, 0.0316, 0.0051, 0.0384, 0.0192 }, /* wp::medium */
            { 0.0655, 0.0288, 0.0044, 0.0266, 0.0123 }, /* wp::tight */
            {    -1.,    -1.,    -1.,    -1.,    -1. }  /* wp::claustro */
        }
    }
};

bool passes_basic_electron_selections(float eleMissHits, float eleIP3D) {
    return eleMissHits <= 1 && eleIP3D < 0.03;
}

template <det T, wp U, typename V>
bool passes_electron_id(float eleMissHits, float eleIP3D, float eleHoverEBc, int hiBin, bool heavyion) {
    if (!passes_basic_electron_selections(eleMissHits, eleIP3D)) { return false; }

    auto iptype = heavyion ? (hiBin < 60 ? ip::cent : ip::peri) : ip::incl;
    return eleHoverEBc < ecuts[iptype][T][U][var::hoe]
        && eleSigmaIEtaIEta_2012 < ecuts[iptype][T][U][var::see]
        && std::abs(eledEtaSeedAtVtx) < ecuts[iptype][T][U][var::deta]
        && std::abs(eledPhiAtVtx) < ecuts[iptype][T][U][var::dphi]
        && std::abs(eleEoverPInv) < ecuts[iptype][T][U][var::eop];
}

int64_t inosculate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
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

    /* load inputs */
    std::vector<std::string> files;
    GetFiles(input.c_str(), files);

    TChain egmChain("ggHiNtuplizerGED/EventTree");
    FillChain(egmChain, files);
    TTreeReader egmReader(&egmChain);

    TTreeReaderValue<int>                nPho(egmReader, "nPho");
    TTreeReaderValue<std::vector<float>> phoEt(egmReader, "phoEt");
    TTreeReaderValue<std::vector<float>> phoSCEta(egmReader, "phoSCEta");
    TTreeReaderValue<std::vector<float>> phoHoverE(egmReader, "phoHoverE");
    TTreeReaderValue<std::vector<float>> phoSigmaIEtaIEta_2012(egmReader, "phoSigmaIEtaIEta_2012");
    TTreeReaderValue<std::vector<float>> phoEta(egmReader, "phoEta");
    TTreeReaderValue<std::vector<float>> phoPhi(egmReader, "phoPhi");

    TTreeReaderValue<int>                nEle(egmReader, "nEle");
    TTreeReaderValue<std::vector<float>> eleSCEta(egmReader, "eleSCEta");
    TTreeReaderValue<std::vector<float>> eleEta(egmReader, "eleEta");
    TTreeReaderValue<std::vector<float>> elePhi(egmReader, "elePhi");
    TTreeReaderValue<std::vector<float>> eleHoverEBc(egmReader, "eleHoverEBc");
    TTreeReaderValue<std::vector<float>> eleSigmaIEtaIEta_2012(egmReader, "eleSigmaIEtaIEta_2012");
    TTreeReaderValue<std::vector<float>> eledEtaSeedAtVtx(egmReader, "eledEtaSeedAtVtx");
    TTreeReaderValue<std::vector<float>> eledPhiAtVtx(egmReader, "eledPhiAtVtx");
    TTreeReaderValue<std::vector<float>> eleEoverPInv(egmReader, "eleEoverPInv");
    TTreeReaderValue<std::vector<float>> eleMissHits(egmReader, "eleMissHits");
    TTreeReaderValue<std::vector<float>> eleIP3D(egmReader, "eleIP3D");

    TChain evtChain("hiEvtAnalyzer/HiTree");
    FillChain(evtChain, files);
    TTreeReader evtReader(&evtChain);
    TTreeReaderValue<float> hiHF(evtReader, "hiHF");
    TTreeReaderValue<int>   hiBin(evtReader, "hiBin");

    /* read in information from TTrees */
    Long64_t nentries = egmReader.GetEntries(true);

    for (Long64_t i = 0; i < nentries; i++) {
        egmReader.Next(); evtReader.Next();

        if (i % 100000 == 0) { printf("entry: %li/%li\n", i, nentries); }

        if (*hiHF <= hf_min) { continue; }

        auto hf_x = ihf->index_for(*hiHF);
        std::vector<float> masses;

        for (int64_t j = 0; j < *nPho; ++j) {
            if ((*phoEt)[j] < 20) //15
                continue;
            if (std::abs((*phoSCEta)[j]) > 1.4442)
                continue;
            if (heavyion && in_pho_failure_region(p, j))
                continue;

            if ((*phoHoverE)[j] > hovere_max) { continue; }
            if ((*phoSigmaIEtaIEta_2012)[j] > 0.011) { continue; }

            auto pho_phi = convert_radian((*phoPhi)[j]);

            bool electron = false;
            for (int64_t l = 0; l < *nEle; ++l) {
                if (std::abs((*eleSCEta)[l]) > 1.4442) { continue; }

                auto deta = (*phoEta)[j] - (*eleEta)[l];
                if (deta > 0.1) { continue; }

                auto ele_phi = convert_radian((*elePhi)[l]);
                auto dphi = revert_radian(pho_phi - ele_phi);
                auto dr2 = deta * deta + dphi * dphi;

                if (dr2 < 0.01 && passes_electron_id<
                            det::barrel, wp::loose, pjtree
                        >((*eleMissHits)[l], (*eleIP3D)[l], 
                        (*eleHoverEBc)[l], (*eleSigmaIEtaIEta_2012)[l], 
                        (*eledEtaSeedAtVtx)[l], (*eledPhiAtVtx)[l],
                        (*eleEoverPInv)[l], *hiBin, heavyion)) {
}
                    electron = true; break;
                }
            }

            if (!electron) { continue; }

            for (int64_t k = j + 1; k < *nPho; ++k) {
                if ((*phoEt)[k] < 20) //15s
                    continue;
                if (std::abs((*phoSCEta)[k]) > 1.4442)
                    continue;
                if (heavyion && in_pho_failure_region(p, k))
                    continue;

                if ((*phoHoverE)[k] > hovere_max) { continue; }
                if ((*phoSigmaIEtaIEta_2012)[k] > 0.011) { continue; }

                auto pho_phi = convert_radian((*phoPhi)[k]);

                bool electron = false;
                for (int64_t l = 0; l < *nEle; ++l) {
                    if (std::abs((*eleSCEta)[l]) > 1.4442) { continue; }

                    auto deta = (*phoEta)[k] - (*eleEta)[l];
                    if (deta > 0.1) { continue; }

                    auto ele_phi = convert_radian((*elePhi)[l]);
                    auto dphi = revert_radian(pho_phi - ele_phi);
                    auto dr2 = deta * deta + dphi * dphi;

                    if (dr2 < 0.01 && passes_electron_id<
                            det::barrel, wp::loose, pjtree
                            >((*eleMissHits)[l], (*eleIP3D)[l], 
                            (*eleHoverEBc)[l], (*eleSigmaIEtaIEta_2012)[l], 
                            (*eledEtaSeedAtVtx)[l], (*eledPhiAtVtx)[l],
                            (*eleEoverPInv)[l], *hiBin, heavyion)) {

                        electron = true; break;
                    }
                }

                if (!electron) { continue; }

                float phoEt_j = (*phoEt)[j];
                if ((*phoEt)[j] > 30 && heavyion && use_er) phoEt_j = (*phoEtErNew)[j];
                if ((*phoEt)[j] > 30 && !heavyion && use_er) phoEt_j = (*phoEtEr)[j];

                float phoEt_k = (*phoEt)[k];
                if ((*phoEt)[k] > 30 && heavyion && use_er) phoEt_k = (*phoEtErNew)[k];
                if ((*phoEt)[k] > 30 && !heavyion && use_er) phoEt_k = (*phoEtEr)[k];

                /* double electron invariant mass */
                auto mass = std::sqrt(ml_invariant_mass<coords::collider>(
                    phoEt_j,
                    (*phoEta)[j],
                    (*phoPhi)[j],
                    0.f,
                    phoEt_k,
                    (*phoEta)[k],
                    (*phoPhi)[k],
                    0.f));

                masses.push_back(mass);
            }
        }

        if (masses.empty()) { continue; }

        std::sort(masses.begin(), masses.end(), [](float a, float b) {
            return std::abs(a - 91.1876) < std::abs(b - 91.1876); });

        (*minv)[x{0, hf_x}]->Fill(masses[0], 1);
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
