#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLatex.h"
#include "TTree.h"

#include <string>
#include <tuple>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;


void fill_data(memory<TH2F>* sfrac, multival* mpthf, TTree* t, pjtree* p, 
                 bool heavyion, bool apply_er, float pt_min, float photon_eta_abs, 
                 float hovere_max, float hf_min, float hf_max, history<TH1F>* efficiency) {
    printf("fill data\n");

    auto nentries = static_cast<int64_t>(t->GetEntries());
    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 100000 == 0) { printf("%li/%li\n", i, nentries); }

        t->GetEntry(i);

        if (p->hiHF <= hf_min) { continue; }
        if (p->hiHF > hf_max) { continue; }

        int64_t leading = -1;
        float leading_pt = 0;
        for (int64_t j = 0; j < p->nPho; ++j) {
            auto pho_et = (*p->phoEt)[j];

            if (pho_et <= 30) { continue; }
            if (std::abs((*p->phoSCEta)[j]) >= photon_eta_abs) { continue; }
            if ((*p->phoHoverE)[j] > hovere_max) { continue; }
            
            if (heavyion && apply_er) pho_et = (*p->phoEtErNew)[j];
            if (!heavyion && apply_er) pho_et = (*p->phoEtEr)[j];

            if (pho_et < pt_min) { continue; }

            if (pho_et > leading_pt) {
                leading = j;
                leading_pt = pho_et;
            }
        }

        /* require leading photon */
        if (leading < 0) { continue; }

        /* hem failure region exclusion */
        if (heavyion && in_pho_failure_region(p, leading)) { continue; }

        /* isolation requirement */
        float isolation = (*p->pho_ecalClusterIsoR3)[leading]
            + (*p->pho_hcalRechitIsoR3)[leading]
            + (*p->pho_trackIsoR3PtCut20)[leading];

        // efficiency correction
        auto weight = p->w;

        if (efficiency != nullptr && leading_pt < 70) {
            auto bin = (*efficiency)[1]->FindBin(leading_pt);
            auto cor = (*efficiency)[0]->GetBinContent(bin) / (*efficiency)[1]->GetBinContent(bin);
            if (cor < 1) { std::cout << "error" << std::endl; return; }
            weight *= cor;
        }

        int64_t index = mpthf->index_for(v{leading_pt, p->hiHF});
        (*sfrac)[index]->Fill(isolation, (*p->phoSigmaIEtaIEta_2012)[leading], weight);
    }

    printf("\n");
}

int tessellate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto data = conf->get<std::vector<std::string>>("data");
    auto signal = conf->get<std::vector<std::string>>("signal");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto eff_file = conf->get<std::string>("eff_file");
    auto eff_label = conf->get<std::string>("eff_label");

    auto rho_file = conf->get<std::string>("rho_file");
    auto rho_label = conf->get<std::string>("rho_label");

    auto apply_er = conf->get<bool>("apply_er");

    auto noniso_min = conf->get<float>("noniso_min");
    auto noniso_max = conf->get<float>("noniso_max");

    auto rsee = conf->get<std::vector<float>>("rsee");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    /* selections */
    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto heavyion = sel->get<bool>("heavyion");

    auto see_max = sel->get<float>("see_max");
    auto pt_min = sel->get<float>("photon_pt_min");
    auto photon_eta_abs = sel->get<float>("photon_eta_abs");
    auto hovere_max = sel->get<float>("hovere_max");
    auto iso_max = sel->get<float>("iso_max");

    auto dpt = sel->get<std::vector<float>>("photon_pt_diff");

    auto dpt_short = dpt;
    dpt_short.pop_back();

    /* exclude most peripheral events */
    auto hf_min = dhf.front();
    auto hf_max = dhf.back();

    /* prepare histograms */
    auto ipt = new interval(dpt);
    auto mpthf = new multival(dpt, dhf);

    std::vector<float> bsee = {rsee[1], see_max, rsee[2]};
    std::vector<float> bsumiso = {-100, iso_max, noniso_min, noniso_max};

    auto incl = new interval(""s, 1, 0., 1.);
    auto isumiso = new interval("SumIso (GeV)"s, bsumiso);
    auto isee = new interval("#sigma_{#eta#eta}"s, bsee);

    auto mfrac = new multival(*isumiso, *isee);

    auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    auto ffrac = std::bind(&multival::book<TH2F>, mfrac, _1, _2, _3);

    auto purity = new memory<TH1F>("pthf"s, "purity"s, fincl, mpthf);
    auto sfrac = new memory<TH2F>("sfrac"s, "counts"s, ffrac, mpthf);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load efficiency correction */
    TFile* fe;
    history<TH1F>* efficiency = nullptr;

    if (!eff_file.empty()) {
        fe = new TFile((base + eff_file).data(), "read");
        efficiency = new history<TH1F>(fe, eff_label);
    }

    /* load inputs */
    for (auto const& file : data) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*) f->Get("pj");
        auto p = new pjtree(false, false, heavyion, t, { 1, 0, 1, 0, 0, 0, heavyion, 0, 0});

        fill_data(sfrac, mpthf, t, p, heavyion, apply_er, pt_min, photon_eta_abs, 
            hovere_max, hf_min, hf_max, efficiency);
    }

    auto hb = new pencil();

    auto convert_index = [&](int64_t index) {
        auto final = index + (index - 1) / (ipt->size() - 1);
        // std::cout << index << " " << final << std::endl;
        return final;
    };

    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(convert_index(index), 0.75, 0.04, mpthf, pt_info, hf_info); };

    auto purity_info = [&](int64_t index) {
        char buffer[128] = { '\0' };
        sprintf(buffer, "purity: %.3f",
            (*purity)[convert_index(index) - 1]->GetBinContent(1));

        TLatex* text = new TLatex();
        text->SetTextFont(43);
        text->SetTextSize(12);
        text->DrawLatexNDC(0.54, 0.56, buffer);
    };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            auto photon_selections = "|#eta^{#gamma}| < "s + to_text(photon_eta_abs);

            TLatex* l = new TLatex();
            l->SetTextAlign(11);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.135, 0.79, photon_selections.data());
        }
    };

    auto system_tag = system + "  #sqrt{s_{NN}} = 5.02 TeV"s;
    system_tag += (heavyion) ? ", 1.69 nb^{-1}"s : ", 302 pb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}}"s;

    auto c1 = new paper(set + "_" + tag + "_abcd_purity", hb);
    apply_style(c1, cms, system_tag);
    c1->accessory(pthf_info);
    c1->accessory(purity_info);
    c1->accessory(kinematics);
    c1->divide(ipt->size() - 1, -1);

    printf("fit templates\n");

    for (int64_t i = 0; i < mpthf->size(); ++i) {
        auto indices = mpthf->indices_for(i);

        if (indices[0] < ipt->size() - 1) {
            c1->add((*sfrac)[i]);
            c1->adjust((*sfrac)[i], "colz text", "");

            auto A = (*sfrac)[i]->GetBinContent(1, 1);
            auto B = (*sfrac)[i]->GetBinContent(3, 1);
            auto C = (*sfrac)[i]->GetBinContent(1, 2);
            auto D = (*sfrac)[i]->GetBinContent(3, 2);

            (*purity)[i]->SetBinContent(1, 1. - B*C / (A*D));
            printf("purity: %.3f\n", (*purity)[i]->GetBinContent(1));
        }
        else {
            (*purity)[i]->SetBinContent(1, 1);
            printf("purity: %.3f\n", (*purity)[i]->GetBinContent(1));
        }
    }

    hb->sketch();
    c1->draw("pdf");

    /* save purities */
    in(output, [&]() {
        purity->save(tag);
        sfrac->save(tag);
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return tessellate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
