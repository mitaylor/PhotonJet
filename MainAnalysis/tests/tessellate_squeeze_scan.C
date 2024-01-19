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
#include "TLatex.h"
#include "TTree.h"

#include <string>
#include <tuple>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

void fill_data(memory<TH1F>* see_iso, memory<TH1F>* see_noniso,
               multival* mpthf, interval* ipt, interval* ihf, TTree* t, pjtree* p,
                bool heavyion, bool apply_er,
               float pt_min, float photon_eta_abs, float hovere_max, float hf_min, float hf_max,
               float iso_max, float noniso_min, float noniso_max, 
               history<TH1F>* rho_weighting, history<TH1F>* efficiency, bool mc) {
    printf("fill data\n");

    auto nentries = static_cast<int64_t>(t->GetEntries());
    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 100000 == 0) { printf("%li/%li\n", i, nentries); }

        t->GetEntry(i);

        if (!mc && p->hiHF <= hf_min) { continue; }
        if (!mc && p->hiHF > hf_max) { continue; }

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

        if ((isolation > iso_max && isolation < noniso_min)
            || isolation > noniso_max) { continue; }

        // determine indices of photon pT and centrality to fill
        auto pt_x = ipt->index_for(leading_pt);
        auto hf_x = ihf->index_for(p->hiHF);

        std::vector<int64_t> pthf_x;
        if (mc) {
            for (int64_t k = 0; k < ihf->size(); ++k) {
                pthf_x.push_back(mpthf->index_for(x{pt_x, k}));
            }
        } else {
            pthf_x.push_back(mpthf->index_for(x{pt_x, hf_x}));
        }

        // efficiency and rho weighting corrections
        auto weight = p->w;

        if (efficiency != nullptr && leading_pt < 70) {
            auto bin = (*efficiency)[1]->FindBin(leading_pt);
            auto cor = (*efficiency)[0]->GetBinContent(bin) / (*efficiency)[1]->GetBinContent(bin);
            if (cor < 1) { std::cout << "error" << std::endl; return; }
            weight *= cor;
        }

        std::vector<float> weights;
        if (mc) {
            auto avg_rho = get_avg_rho(p, -photon_eta_abs, photon_eta_abs);

            for (int64_t k = 0; k < ihf->size(); ++k) {
                auto bin = (*rho_weighting)[k]->FindBin(avg_rho);
                auto cor = (*rho_weighting)[k]->GetBinContent(bin);
                weights.push_back(weight * cor);
            }
        } else {
            weights.push_back(weight);
        }

        auto const& see = isolation > iso_max ? see_noniso : see_iso;

        zip([&](auto const& index, auto const& weight) {
            (*see)[index]->Fill((*p->phoSigmaIEtaIEta_2012)[leading], weight);
        }, pthf_x, weights);
    }

    printf("\n");
}

void fill_signal(memory<TH1F>* see, memory<TH1F>* sfrac,
                 multival* mpthf, interval* ipt, interval* ihf, TTree* t, pjtree* p, 
                 bool heavyion, bool apply_er, float pt_min, float photon_eta_abs, 
                 float hovere_max, float hf_min, float hf_max, float iso_max,
                 float noniso_min, float noniso_max, float gen_iso_max, std::vector<float> offsets, 
                 std::vector<float> widths, history<TH1F>* rho_weighting) {
    printf("fill signal\n");

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

        /* require gen-matching */
        int64_t gen_index = (*p->pho_genMatchedIndex)[leading];
        if (gen_index == -1) { continue; }

        auto pid = (*p->mcPID)[gen_index];
        auto mpid = (*p->mcMomPID)[gen_index];
        if (pid != 22 || (std::abs(mpid) > 22 && mpid != -999)) { continue; }

        /* gen isolation requirement */
        float isolation = (*p->mcCalIsoDR04)[gen_index];
        if (isolation > gen_iso_max) { continue; }

        std::vector<float> weight(ihf->size(), p->w);

        if (rho_weighting != nullptr) {
            auto avg_rho = get_avg_rho(p, -photon_eta_abs, photon_eta_abs);

            for (int64_t hf_x = 0; hf_x < ihf->size(); ++hf_x) {
                auto bin = (*rho_weighting)[hf_x]->FindBin(avg_rho);
                auto corr = (*rho_weighting)[hf_x]->GetBinContent(bin);
                weight[hf_x] *= corr;
            }
        }

        auto pt_x = ipt->index_for(leading_pt);

        for (int64_t hf_x = 0; hf_x < ihf->size(); ++hf_x) {
            auto pthf_x = mpthf->index_for(x{pt_x, hf_x});
            auto sigma_eta_eta = (*p->phoSigmaIEtaIEta_2012)[leading];
            sigma_eta_eta = widths[pthf_x]*(sigma_eta_eta + offsets[pthf_x]);

            (*see)[pthf_x]->Fill(sigma_eta_eta, weight[hf_x]);

            /* isolation requirement */
            float recoiso = (*p->pho_ecalClusterIsoR3)[leading]
                + (*p->pho_hcalRechitIsoR3)[leading]
                + (*p->pho_trackIsoR3PtCut20)[leading];

            if ((recoiso > iso_max && recoiso < noniso_min)
                || recoiso > noniso_max) { continue; }

            (*sfrac)[pthf_x]->Fill(recoiso > iso_max ? 1.5 : 0.5, weight[hf_x]);
        }
    }

    printf("\n");
}

auto fit_templates(TH1F* hdata, TH1F* hsig, TH1F* hbkg,
                   std::vector<float> const& range) {
    auto stub = "_"s + hdata->GetName();

    TH1F* tdata = (TH1F*)hdata->Clone(("t"s + stub).data());
    TH1F* tsig = (TH1F*)hsig->Clone(("t_s"s + stub).data());
    TH1F* tbkg = (TH1F*)hbkg->Clone(("t_b"s + stub).data());

    tsig->Scale(1. / tsig->Integral());
    tbkg->Scale(1. / tbkg->Integral());

    auto evaluate = [&](double* x, double* p) {
        float nsig = tsig->GetBinContent(tsig->FindBin(x[0]));
        float nbkg = tbkg->GetBinContent(tbkg->FindBin(x[0]));

        return p[0] * (nsig * p[1] + nbkg * (1 - p[1]));
    };

    TF1* f = new TF1(("f"s + stub).data(), evaluate, range[0], range[1], 2);
    f->SetParameters(tdata->Integral(), 0.8);
    f->SetParLimits(1, 0., 1.);

    tdata->Fit(("f"s + stub).data(), "L0Q", "", range[0], range[1]);
    tdata->Fit(("f"s + stub).data(), "L0Q", "", range[0], range[1]);
    tdata->Fit(("f"s + stub).data(), "L0QM", "", range[0], range[1]);

    auto p0 = f->GetParameter(0);
    auto p1 = f->GetParameter(1);

    auto p0_err = f->GetParError(0);
    auto p1_err = f->GetParError(1);

    auto chisq = f->GetChisquare();
    auto ndof = f->GetNDF();

    return std::make_tuple(p0, p1, p0_err, p1_err, chisq, ndof);
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
    auto mc = conf->get<bool>("mc");

    auto noniso_min = conf->get<float>("noniso_min");
    auto noniso_max = conf->get<float>("noniso_max");

    auto rsee = conf->get<std::vector<float>>("rsee");
    auto rfit = conf->get<std::vector<float>>("rfit");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto widths = conf->get<std::vector<float>>("widths");

    /* selections */
    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto heavyion = sel->get<bool>("heavyion");

    auto gen_iso_max = sel->get<float>("gen_iso_max");
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
    auto ihf = new interval(dhf);
    auto mpthf = new multival(dpt, dhf);

    auto incl = new interval(""s, 1, 0., 1.);
    auto itwo = new interval(""s, 2, 0., 2.);
    auto isee = new interval("#sigma_{#eta#eta}"s, rsee[0], rsee[1], rsee[2]);

    auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    auto ftwo = std::bind(&interval::book<TH1F>, itwo, _1, _2, _3);
    auto fsee = std::bind(&interval::book<TH1F>, isee, _1, _2, _3);

    auto see_data = new memory<TH1F>("see_data"s, "counts", fsee, mpthf);
    auto see_sig_initial = new memory<TH1F>("see_sig_initial"s, "counts", fsee, mpthf);
    auto see_sig = new memory<TH1F>("see_sig"s, "counts", fsee, mpthf);
    auto see_bkg = new memory<TH1F>("see_bkg"s, "counts", fsee, mpthf);

    auto purity = new memory<TH1F>("pthf"s, "purity"s, fincl, mpthf);
    auto sfrac = new memory<TH1F>("sfrac"s, "counts"s, ftwo, mpthf);

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

    /* load centrality weighting for MC */
    TFile* fr;
    history<TH1F>* rho_weighting = nullptr;

    if (!rho_file.empty()) {
        fr = new TFile((base + rho_file).data(), "read");
        rho_weighting = new history<TH1F>(fr, rho_label);
    }

    /* load inputs */
    for (auto const& file : data) {
        std::cout << file << std::endl;

        TFile* fd = new TFile(file.data(), "read");
        TTree* td = (TTree*)fd->Get("pj");
        auto pd = new pjtree(false, false, heavyion, td, { 1, 0, 1, 0, 0, 0, heavyion, 0, 0});

        fill_data(see_data, see_bkg, mpthf, ipt, ihf, td, pd, heavyion, apply_er,
                pt_min, photon_eta_abs, hovere_max, hf_min, hf_max, iso_max, 
                noniso_min, noniso_max, rho_weighting, efficiency, mc && heavyion);
    }

    /* fill signal with MC */
    std::vector<float> offsets(mpthf->size(), 0);

    for (auto const& file : signal) {
        std::cout << file << std::endl;
        
        TFile* fs = new TFile(file.data(), "read");
        TTree* ts = (TTree*)fs->Get("pj");
        auto ps = new pjtree(true, false, heavyion, ts, { 1, 1, 1, 0, 0, 0, heavyion, 0, 0});

        fill_signal(see_sig_initial, sfrac, mpthf, ipt, ihf, ts, ps, 
                    heavyion, apply_er, pt_min, photon_eta_abs, 
                    hovere_max, hf_min, hf_max, iso_max, 
                    noniso_min, noniso_max, gen_iso_max, offsets, 
                    widths, rho_weighting);
    }

    /* alter signal template to match data std and mean between 0 and 0.01 */
    for (int64_t i = 0; i < mpthf->size(); ++i) {
        auto res = fit_templates((*see_data)[i], (*see_sig_initial)[i], (*see_bkg)[i], rfit);

        auto stub = "p_"s + (*see_data)[i]->GetName();
        auto pfit = (TH1F*)(*see_sig_initial)[i]->Clone((stub + "f").data());
        auto pbkg = (TH1F*)(*see_bkg)[i]->Clone((stub + "b").data());

        auto entries = std::get<0>(res);
        auto fraction = std::get<1>(res);

        pfit->Scale(entries * fraction / pfit->Integral());
        pbkg->Scale(entries * (1. - fraction) / pbkg->Integral());

        pfit->Add(pbkg);

        (*see_data)[i]->GetXaxis()->SetRangeUser(0, 0.01);
        pfit->GetXaxis()->SetRangeUser(0, 0.01);

        offsets[i] = (*see_data)[i]->GetMean() / widths[i] - pfit->GetMean();

        std::cout << "offset: " << (*see_data)[i]->GetMean() - pfit->GetMean() 
                  << ", std data: " << (*see_data)[i]->GetRMS() << "std mc: " << pfit->GetRMS() << std::endl;

        (*see_data)[i]->GetXaxis()->SetRange(0, 0);
        pfit->GetXaxis()->SetRange(0, 0);
    }
    
    for (auto const& file : signal) {
        std::cout << file << std::endl;
        
        TFile* fs = new TFile(file.data(), "read");
        TTree* ts = (TTree*)fs->Get("pj");
        auto ps = new pjtree(true, false, heavyion, ts, { 1, 1, 1, 0, 0, 0, heavyion, 0, 0});

        fill_signal(see_sig, sfrac, mpthf, ipt, ihf, ts, ps, 
                    heavyion, apply_er, pt_min, photon_eta_abs, 
                    hovere_max, hf_min, hf_max, iso_max, 
                    noniso_min, noniso_max, gen_iso_max, offsets, 
                    widths, rho_weighting);
    }

    auto hb = new pencil();
    hb->category("type", "data", "sig", "bkg");

    hb->alias("sig", "PYTHIA8");
    hb->alias("bkg", "noniso. data");

    hb->style("sig", [](TH1* h) {
        h->SetLineColor(kPink);
        h->SetFillColor(kOrange + 7);
        h->SetFillStyle(3004);
    });

    hb->style("bkg", [](TH1* h) {
        h->SetLineColor(kGreen + 4);
        h->SetFillColor(kGreen + 1);
        h->SetFillStyle(3001);
    });

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

    auto c1 = new paper(set + "_purity_" + tag + "_squeeze", hb);
    apply_style(c1, cms, system_tag);
    c1->accessory(pthf_info);
    c1->accessory(purity_info);
    c1->accessory(kinematics);
    c1->divide(ipt->size() - 1, -1);

    auto c2 = new paper(set + "_purity_log_" + tag + "_squeeze", hb);
    apply_style(c2, cms, system_tag);
    c2->accessory(pthf_info);
    c2->accessory(purity_info);
    c2->accessory(kinematics);
    c2->divide(ipt->size() - 1, -1);
    c2->set(paper::flags::logy);

    printf("fit templates\n");

    for (int64_t i = 0; i < mpthf->size(); ++i) {
        auto indices = mpthf->indices_for(i);

        if (indices[0] < ipt->size() - 1) {
            auto res = fit_templates((*see_data)[i], (*see_sig)[i], (*see_bkg)[i], rfit);

            auto stub = "p_"s + (*see_data)[i]->GetName();
            auto pfit = (TH1F*)(*see_sig)[i]->Clone((stub + "f").data());
            auto pbkg = (TH1F*)(*see_bkg)[i]->Clone((stub + "b").data());

            auto entries = std::get<0>(res);
            auto fraction = std::get<1>(res);
            auto chisq = std::get<4>(res);
            auto ndof = std::get<5>(res);

            pfit->Scale(entries * fraction / pfit->Integral());
            pbkg->Scale(entries * (1. - fraction) / pbkg->Integral());

            auto psig = (TH1F*)pfit->Clone((stub + "s").data());

            pfit->Add(pbkg);

            c1->add((*see_data)[i], "data");
            c1->stack(psig, "sig");
            c1->stack(pbkg, "bkg");

            c1->adjust(psig, "hist f", "lf");
            c1->adjust(pbkg, "hist f", "lf");

            c2->add((*see_data)[i], "data");
            c2->stack(psig, "sig");
            c2->stack(pbkg, "bkg");

            c2->adjust(psig, "hist f", "lf");
            c2->adjust(pbkg, "hist f", "lf");

            auto ntot = pfit->Integral(1, pfit->FindBin(see_max));
            auto nbkg = pbkg->Integral(1, pbkg->FindBin(see_max));

            (*purity)[i]->SetBinContent(1, 1. - nbkg / ntot);
            printf("purity: %.3f\n", (*purity)[i]->GetBinContent(1));
            printf("chisq: %.3f\n", chisq);
            printf("ndof: %d\n", ndof);
            std::cout << (*see_data)[i]->Integral(1, (*see_data)[i]->FindBin(see_max)) << ntot << std::endl;
        }
        else {
            (*purity)[i]->SetBinContent(1, 1);
            printf("purity: %.3f\n", (*purity)[i]->GetBinContent(1));
        }
    }

    hb->sketch();
    c1->draw("pdf");
    c2->draw("pdf");

    /* save purities */
    in(output, [&]() {
        purity->save(tag);
        sfrac->save(tag);

        see_data->save(tag);
        see_sig->save(tag);
        see_bkg->save(tag);
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return tessellate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}