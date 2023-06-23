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
#include "TF1.h"

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename... T>
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

template <typename... T>
void normalise_to_unity(T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1. / obj->Integral()); }), 0)... };
}

template <typename... T>
void scale_bin_width(T*... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1., "width"); }), 0)... };
}

int compare_photon_pt_spectrum(char const* config, char const* selections, const char* output) {
    auto conf = new configurer(config);

    auto input_data = conf->get<std::string>("input_data");
    auto input_mc = conf->get<std::string>("input_mc");

    auto input_accumulate_data = conf->get<std::string>("input_accumulate_data");
    auto input_accumulate_mc = conf->get<std::string>("input_accumulate_mc");

    auto input_populate_data = conf->get<std::string>("input_populate_data");
    auto input_populate_mc = conf->get<std::string>("input_populate_mc");

    auto input_purity_data = conf->get<std::string>("input_purity_data");
    auto input_purity_mc = conf->get<std::string>("input_purity_mc");

    auto purity_label = conf->get<std::string>("purity_label");

    auto tag = conf->get<std::string>("tag");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    /* selections */
    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    // auto heavyion = sel->get<bool>("heavyion");

    auto dpt = sel->get<std::vector<float>>("photon_pt_diff");

    /* load history objects */
    TFile* f_data = new TFile((base + input_data).data(), "read");
    TFile* f_mc = new TFile((base + input_mc).data(), "read");

    TFile* f_accumulate_data = new TFile((base + input_accumulate_data).data(), "read");
    TFile* f_accumulate_mc = new TFile((base + input_accumulate_mc).data(), "read");

    TFile* f_populate_data = new TFile((base + input_populate_data).data(), "read");
    TFile* f_populate_mc = new TFile((base + input_populate_mc).data(), "read");

    TFile* f_purity_data = new TFile((base + input_purity_data).data(), "read");
    TFile* f_purity_mc = new TFile((base + input_purity_mc).data(), "read");

    auto h_data_construct_populate = new history<TH1F>(f_data, "raw_spectrum_photon");
    h_data_construct_populate->rename("h_data_construct_populate");
    auto h_mc_construct_populate = new history<TH1F>(f_mc, "raw_spectrum_photon");
    h_mc_construct_populate->rename("h_mc_construct_populate");

    auto h_data_construct_populate_jet = new history<TH1F>(f_data, "raw_spectrum_photon_jet");
    h_data_construct_populate_jet->rename("h_data_construct_populate_jet");
    auto h_mc_construct_populate_jet = new history<TH1F>(f_mc, "raw_spectrum_photon_jet");
    h_mc_construct_populate_jet->rename("h_mc_construct_populate_jet");

    auto h_data_construct_populate_jet_sub = new history<TH1F>(f_data, "raw_sub_spectrum_photon_jet");
    h_data_construct_populate_jet_sub->rename("h_data_construct_populate_jet_sub");
    auto h_mc_construct_populate_jet_sub = new history<TH1F>(f_mc, "raw_sub_spectrum_photon_jet");
    h_mc_construct_populate_jet_sub->rename("h_mc_construct_populate_jet_sub");

    auto h_mc_construct_vacillate = new history<TH1F>(f_mc, "raw_mc_spectrum_photon");
    h_mc_construct_vacillate->rename("h_mc_construct_vacillate");
    auto h_mc_construct_vacillate_jet = new history<TH1F>(f_mc, "raw_mc_spectrum_photon_jet");
    h_mc_construct_vacillate_jet->rename("h_mc_construct_vacillate_jet");

    auto h_data_accumulate_nevt = new history<TH1F>(f_accumulate_data, tag + "_nominal_s_pure_raw_nevt");
    h_data_accumulate_nevt->rename("h_data_accumulate_nevt");
    auto h_mc_accumulate_nevt = new history<TH1F>(f_accumulate_mc, tag + "_qcd_nominal_s_pure_raw_nevt");
    h_mc_accumulate_nevt->rename("h_mc_accumulate_nevt");

    auto h_data_populate_nevt = new history<TH1F>(f_populate_data, "raw_nevt");
    h_data_populate_nevt->rename("h_data_populate_nevt");
    auto h_mc_populate_nevt = new history<TH1F>(f_populate_mc, "raw_nevt");
    h_mc_populate_nevt->rename("h_mc_populate_nevt");

    auto h_data_purity = new history<TH1F>(f_purity_data, purity_label + "_pthf");
    auto h_mc_purity = new history<TH1F>(f_purity_mc, tag + "_qcd_pthf");

    /* create intervals and multivals */
    auto mpthf_long = new multival(dpt, dhf);

    dpt.pop_back();

    auto mpthf_trunc = new multival(dpt, dhf);
    auto ihf = new interval(dhf);
    auto ipt = new interval(dpt);

    auto fpt = std::bind(&interval::book<TH1F>, ipt, _1, _2, _3);

    auto h_data_construct_accumulate = new history<TH1F>("h_data_construct_accumulate"s, "", fpt, ihf->size());
    auto h_mc_construct_accumulate = new history<TH1F>("h_mc_construct_accumulate"s, "", fpt, ihf->size());

    auto h_data_construct_accumulate_jet = new history<TH1F>("h_data_construct_accumulate_jet"s, "", fpt, ihf->size());
    auto h_mc_construct_accumulate_jet = new history<TH1F>("h_mc_construct_accumulate_jet"s, "", fpt, ihf->size());

    auto h_data_construct_accumulate_jet_sub = new history<TH1F>("h_data_construct_accumulate_jet_sub"s, "", fpt, ihf->size());
    auto h_mc_construct_accumulate_jet_sub = new history<TH1F>("h_mc_construct_accumulate_jet_sub"s, "", fpt, ihf->size());

    auto h_data_accumulate = new history<TH1F>("h_data_accumulate"s, "", fpt, ihf->size());
    auto h_mc_accumulate = new history<TH1F>("h_mc_accumulate"s, "", fpt, ihf->size());

    auto h_data_populate = new history<TH1F>("h_data_populate"s, "", fpt, ihf->size());
    auto h_mc_populate = new history<TH1F>("h_mc_populate"s, "", fpt, ihf->size());

    /* set histogram contents */
    for (int64_t i = 0; i < ihf->size(); ++i) {
        for (int j = 0; j < ipt->size(); ++j) {
            auto index_long = mpthf_long->index_for(x{j, i});
            auto index_trunc = mpthf_trunc->index_for(x{j, i});

            (*h_data_construct_accumulate)[i]->SetBinContent(j+1, 
                    (*h_data_construct_populate)[i]->GetBinContent(j+1) * (*h_data_purity)[index_long]->GetBinContent(1));
            (*h_data_construct_accumulate)[i]->SetBinError(j+1, 
                    (*h_data_construct_populate)[i]->GetBinError(j+1) * (*h_data_purity)[index_long]->GetBinContent(1));
            (*h_mc_construct_accumulate)[i]->SetBinContent(j+1, 
                    (*h_mc_construct_populate)[i]->GetBinContent(j+1) * (*h_mc_purity)[index_long]->GetBinContent(1));
            (*h_mc_construct_accumulate)[i]->SetBinError(j+1, 
                    (*h_mc_construct_populate)[i]->GetBinError(j+1) * (*h_mc_purity)[index_long]->GetBinContent(1));

            (*h_data_construct_accumulate_jet)[i]->SetBinContent(j+1, 
                    (*h_data_construct_populate_jet)[i]->GetBinContent(j+1) * (*h_data_purity)[index_long]->GetBinContent(1));
            (*h_data_construct_accumulate_jet)[i]->SetBinError(j+1, 
                    (*h_data_construct_populate_jet)[i]->GetBinError(j+1) * (*h_data_purity)[index_long]->GetBinContent(1));
            (*h_mc_construct_accumulate_jet)[i]->SetBinContent(j+1, 
                    (*h_mc_construct_populate_jet)[i]->GetBinContent(j+1) * (*h_mc_purity)[index_long]->GetBinContent(1));
            (*h_mc_construct_accumulate_jet)[i]->SetBinError(j+1, 
                    (*h_mc_construct_populate_jet)[i]->GetBinError(j+1) * (*h_mc_purity)[index_long]->GetBinContent(1));

            (*h_data_construct_accumulate_jet_sub)[i]->SetBinContent(j+1, 
                    (*h_data_construct_populate_jet_sub)[i]->GetBinContent(j+1) * (*h_data_purity)[index_long]->GetBinContent(1));
            (*h_data_construct_accumulate_jet_sub)[i]->SetBinError(j+1, 
                    (*h_data_construct_populate_jet_sub)[i]->GetBinError(j+1) * (*h_data_purity)[index_long]->GetBinContent(1));
            (*h_mc_construct_accumulate_jet_sub)[i]->SetBinContent(j+1, 
                    (*h_mc_construct_populate_jet_sub)[i]->GetBinContent(j+1) * (*h_mc_purity)[index_long]->GetBinContent(1));
            (*h_mc_construct_accumulate_jet_sub)[i]->SetBinError(j+1, 
                    (*h_mc_construct_populate_jet_sub)[i]->GetBinError(j+1) * (*h_mc_purity)[index_long]->GetBinContent(1));

            (*h_data_accumulate)[i]->SetBinContent(j+1, (*h_data_accumulate_nevt)[index_trunc]->GetBinContent(1));
            (*h_data_accumulate)[i]->SetBinError(j+1, (*h_data_accumulate_nevt)[index_trunc]->GetBinError(1));
            (*h_mc_accumulate)[i]->SetBinContent(j+1, (*h_mc_accumulate_nevt)[index_trunc]->GetBinContent(1));
            (*h_mc_accumulate)[i]->SetBinError(j+1, (*h_mc_accumulate_nevt)[index_trunc]->GetBinError(1));

            (*h_data_populate)[i]->SetBinContent(j+1, (*h_data_populate_nevt)[index_long]->GetBinContent(1));
            (*h_data_populate)[i]->SetBinError(j+1, (*h_data_populate_nevt)[index_long]->GetBinError(1));
            (*h_mc_populate)[i]->SetBinContent(j+1, (*h_mc_populate_nevt)[index_long]->GetBinContent(1));
            (*h_mc_populate)[i]->SetBinError(j+1, (*h_mc_populate_nevt)[index_long]->GetBinError(1));
        }
    }

    scale_bin_width(h_data_construct_populate, h_mc_construct_populate);
    scale_bin_width(h_data_construct_populate_jet, h_mc_construct_populate_jet);
    scale_bin_width(h_data_construct_populate_jet_sub, h_mc_construct_populate_jet_sub);
    scale_bin_width(h_data_construct_accumulate, h_mc_construct_accumulate);
    scale_bin_width(h_data_construct_accumulate_jet, h_mc_construct_accumulate_jet);
    scale_bin_width(h_data_construct_accumulate_jet_sub, h_mc_construct_accumulate_jet_sub);
    scale_bin_width(h_mc_construct_vacillate, h_mc_construct_vacillate_jet);
    scale_bin_width(h_data_accumulate, h_mc_accumulate);
    scale_bin_width(h_data_populate, h_mc_populate);

    normalise_to_unity(h_data_construct_populate, h_mc_construct_populate);
    normalise_to_unity(h_data_construct_populate_jet, h_mc_construct_populate_jet);
    normalise_to_unity(h_data_construct_populate_jet_sub, h_mc_construct_populate_jet_sub);
    normalise_to_unity(h_data_construct_accumulate, h_mc_construct_accumulate);
    normalise_to_unity(h_data_construct_accumulate_jet, h_mc_construct_accumulate_jet);
    normalise_to_unity(h_data_construct_accumulate_jet_sub, h_mc_construct_accumulate_jet_sub);
    normalise_to_unity(h_mc_construct_vacillate, h_mc_construct_vacillate_jet);
    normalise_to_unity(h_data_accumulate, h_mc_accumulate);
    normalise_to_unity(h_data_populate, h_mc_populate);

    /* create ratio */
    auto h_ratio = new history<TH1F>("h_data_populate"s, "", fpt, ihf->size());

    for (int64_t i = 0; i < ihf->size(); ++i) {
        (*h_ratio)[i]->Divide((*h_data_construct_accumulate)[i], (*h_mc_construct_vacillate)[i]);
    }

    /* set up figures */
    auto system_tag = "  #sqrt{s_{NN}} = 5.02 TeV, 1.69 nb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;

    auto hf_info = [&](int64_t index) {
        info_text(index, 0.70, "Cent. %i - %i%%", dcent, true); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(12);
            l->DrawLatexNDC(0.865, 0.70, "40 < p_{T}^{#gamma} < 200, |#eta^{#gamma}| < 1.44");
        }
    };

    auto hb = new pencil();
    hb->category("type", "data", "analysis_mc", "prior_mc", "ratio");

    auto p1 = new paper(set + "_" + purity_label + "_populate_photon_spectra", hb);
    p1->divide(ihf->size(), -1);
    p1->set(paper::flags::logy);
    p1->set(paper::flags::logx);
    p1->accessory(hf_info);
    p1->accessory(kinematics);
    apply_style(p1, cms, system_tag);
    
    h_data_construct_populate->apply([&](TH1* h) { p1->add(h, "data"); });
    h_mc_construct_populate->apply([&](TH1* h, int64_t index) { p1->stack(index + 1, h, "analysis_mc"); });

    auto p2 = new paper(set + "_" + purity_label + "_accumulate_photon_spectra", hb);
    p2->divide(ihf->size(), -1);
    p2->set(paper::flags::logy);
    p2->set(paper::flags::logx);
    p2->accessory(hf_info);
    p2->accessory(kinematics);
    apply_style(p2, cms, system_tag);
    
    h_data_construct_accumulate->apply([&](TH1* h) { p2->add(h, "data"); });
    // h_mc_construct_accumulate->apply([&](TH1* h, int64_t index) { p2->stack(index + 1, h, "analysis_mc"); });
    h_mc_construct_vacillate->apply([&](TH1* h, int64_t index) { p2->stack(index + 1, h, "prior_mc"); });
    h_ratio->apply([&](TH1* h) { p2->add(h, "ratio"); });

    auto p3 = new paper(set + "_" + purity_label + "_populate_photon_jet_spectra", hb);
    p3->divide(ihf->size(), -1);
    p3->set(paper::flags::logy);
    p3->set(paper::flags::logx);
    p3->accessory(hf_info);
    p3->accessory(kinematics);
    apply_style(p3, cms, system_tag);
    
    h_data_construct_populate_jet->apply([&](TH1* h) { p3->add(h, "data"); });
    h_mc_construct_populate_jet->apply([&](TH1* h, int64_t index) { p3->stack(index + 1, h, "analysis_mc"); });

    auto p4 = new paper(set + "_" + purity_label + "_accumulate_photon_jet_spectra", hb);
    p4->divide(ihf->size(), -1);
    p4->set(paper::flags::logy);
    p4->set(paper::flags::logx);
    p4->accessory(hf_info);
    p4->accessory(kinematics);
    apply_style(p4, cms, system_tag);
    
    h_data_construct_accumulate_jet->apply([&](TH1* h) { p4->add(h, "data"); });
    h_mc_construct_accumulate_jet->apply([&](TH1* h, int64_t index) { p4->stack(index + 1, h, "analysis_mc"); });
    h_mc_construct_vacillate_jet->apply([&](TH1* h, int64_t index) { p4->stack(index + 1, h, "prior_mc"); });

    auto p5 = new paper(set + "_" + purity_label + "_populate_photon_jet_sub_spectra", hb);
    p5->divide(ihf->size(), -1);
    p5->set(paper::flags::logy);
    p5->set(paper::flags::logx);
    p5->accessory(hf_info);
    p5->accessory(kinematics);
    apply_style(p5, cms, system_tag);
    
    h_data_construct_populate_jet_sub->apply([&](TH1* h) { p5->add(h, "data"); });
    h_mc_construct_populate_jet_sub->apply([&](TH1* h, int64_t index) { p5->stack(index + 1, h, "analysis_mc"); });

    auto p6 = new paper(set + "_" + purity_label + "_accumulate_photon_jet_sub_spectra", hb);
    p6->divide(ihf->size(), -1);
    p6->set(paper::flags::logy);
    p6->set(paper::flags::logx);
    p6->accessory(hf_info);
    p6->accessory(kinematics);
    apply_style(p6, cms, system_tag);
    
    h_data_construct_accumulate_jet_sub->apply([&](TH1* h) { p6->add(h, "data"); });
    h_mc_construct_accumulate_jet_sub->apply([&](TH1* h, int64_t index) { p6->stack(index + 1, h, "analysis_mc"); });
    h_mc_construct_vacillate_jet->apply([&](TH1* h, int64_t index) { p6->stack(index + 1, h, "prior_mc"); });

    hb->sketch();

    p1->draw("pdf");
    p2->draw("pdf");
    p3->draw("pdf");
    p4->draw("pdf");
    p5->draw("pdf");
    p6->draw("pdf");

    in(output, [&]() {
        h_data_construct_populate->save();
        h_mc_construct_populate->save();
        h_data_construct_populate_jet->save();
        h_mc_construct_populate_jet->save();
        h_data_construct_populate_jet_sub->save();
        h_mc_construct_populate_jet_sub->save();

        h_mc_construct_vacillate->save();
        h_mc_construct_vacillate_jet->save();

        h_data_populate->save();
        h_mc_populate->save();

        h_data_construct_accumulate->save();
        h_mc_construct_accumulate->save();
        h_data_construct_accumulate_jet->save();
        h_mc_construct_accumulate_jet->save();
        h_data_construct_accumulate_jet_sub->save();
        h_mc_construct_accumulate_jet_sub->save();

        h_data_accumulate->save();
        h_mc_accumulate->save();
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return compare_photon_pt_spectrum(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
