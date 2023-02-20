#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"

#include <memory>
#include <string>
#include <vector>
#include <ctime>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int hallucinate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto inputs = conf->get<std::vector<std::string>>("inputs");
    auto tag = conf->get<std::string>("tag");

    auto rjpt = conf->get<std::vector<float>>("jpt_range");
    auto rdphi = conf->get<std::vector<float>>("dphi_range");
    auto rdr = conf->get<std::vector<float>>("dr_range");

    auto rrdr = conf->get<std::vector<float>>("rdr_range");
    auto rrdphi = conf->get<std::vector<float>>("rdphi_range");
    auto rrpt = conf->get<std::vector<float>>("rpt_range");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* define ranges */
    convert_in_place_pi(rdphi);

    auto mpthf = new multival(dpt, dhf);

    auto incl = new interval(""s, 1, 0.f, 9999.f);
    auto idphi = new interval("#Delta#phi^{#gammaj}"s, rdphi);
    auto idr = new interval("#deltaj"s, rdr);
    auto ijpt = new interval("p_{T}^{j}"s, rjpt);

    auto mdr = new multival(rrdr, rrpt);
    auto mdphi = new multival(rrdphi, rrpt);

    auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    auto fdphi = std::bind(&interval::book<TH1F>, idphi, _1, _2, _3);
    auto fdr = std::bind(&interval::book<TH1F>, idr, _1, _2, _3);
    auto fjpt = std::bind(&interval::book<TH1F>, ijpt, _1, _2, _3);

    auto frdr = [&](int64_t, std::string const& name, std::string const&) {
        return new TH1F(name.data(), ";index;", mdr->size(), 0, mdr->size()); };
    auto frdphi = [&](int64_t, std::string const& name, std::string const&) {
        return new TH1F(name.data(), ";index;", mdphi->size(), 0, mdphi->size()); };

    /* create histograms */
    std::vector<std::string> labels = { "nevt", 
        "pjet_es_f_dphi", "mix_pjet_es_f_dphi", "sub_pjet_es_f_dphi", 
        "pjet_wta_f_dphi", "mix_pjet_wta_f_dphi", "sub_pjet_wta_f_dphi",
        "pjet_f_dr", "mix_pjet_f_dr", "sub_pjet_f_dr",
        "pjet_f_jpt", "mix_pjet_f_jpt", "sub_pjet_f_jpt",
        "pjet_es_u_dphi", "mix_pjet_es_u_dphi", "sub_pjet_es_u_dphi",
        "pjet_wta_u_dphi", "mix_pjet_wta_u_dphi", "sub_pjet_wta_u_dphi",
        "pjet_u_dr", "mix_pjet_u_dr", "sub_pjet_u_dr" };

    std::vector<history<TH1F>*> new_histograms(labels.size(), nullptr);

    new_histograms[0] = new memory<TH1F>(labels[0], "", fincl, mpthf); // nevt

    new_histograms[1] = new memory<TH1F>(labels[1], "1/N^{#gamma} dN/d#Delta#phi^{#gammaj}", fdphi, mpthf); // pjet_es_f_dphi
    new_histograms[2] = new memory<TH1F>(labels[2], "1/N^{#gamma} dN/d#Delta#phi^{#gammaj}", fdphi, mpthf); // mix_pjet_es_f_dphi
    new_histograms[3] = new memory<TH1F>(labels[3], "1/N^{#gamma} dN/d#Delta#phi^{#gammaj}", fdphi, mpthf); // sub_pjet_es_f_dphi

    new_histograms[4] = new memory<TH1F>(labels[4], "1/N^{#gamma} dN/d#Delta#phi^{#gammaj}", fdphi, mpthf); // pjet_wta_f_dphi
    new_histograms[5] = new memory<TH1F>(labels[5], "1/N^{#gamma} dN/d#Delta#phi^{#gammaj}", fdphi, mpthf); // mix_pjet_wta_f_dphi
    new_histograms[6] = new memory<TH1F>(labels[6], "1/N^{#gamma} dN/d#Delta#phi^{#gammaj}", fdphi, mpthf); // sub_pjet_wta_f_dphi

    new_histograms[7] = new memory<TH1F>(labels[7], "1/N^{#gamma} dN/d#deltaj", fdr, mpthf); // pjet_f_dr
    new_histograms[8] = new memory<TH1F>(labels[8], "1/N^{#gamma} dN/d#deltaj", fdr, mpthf); // mix_pjet_f_dr
    new_histograms[9] = new memory<TH1F>(labels[9], "1/N^{#gamma} dN/d#deltaj", fdr, mpthf); // sub_pjet_f_dr

    new_histograms[10] = new memory<TH1F>(labels[10], "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf); // pjet_f_jpt
    new_histograms[11] = new memory<TH1F>(labels[11], "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf); // mix_pjet_f_jpt
    new_histograms[12] = new memory<TH1F>(labels[12], "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf); // sub_pjet_f_jpt

    new_histograms[13] = new memory<TH1F>(labels[13], "", frdphi, mpthf); // pjet_es_u_dphi
    new_histograms[14] = new memory<TH1F>(labels[14], "", frdphi, mpthf); // mix_pjet_es_u_dphi
    new_histograms[15] = new memory<TH1F>(labels[15], "", frdphi, mpthf); // sub_pjet_es_u_dphi

    new_histograms[16] = new memory<TH1F>(labels[16], "", frdphi, mpthf); // pjet_wta_u_dphi
    new_histograms[17] = new memory<TH1F>(labels[17], "", frdphi, mpthf); // mix_pjet_wta_u_dphi
    new_histograms[18] = new memory<TH1F>(labels[18], "", frdphi, mpthf); // sub_pjet_wta_u_dphi

    new_histograms[19] = new memory<TH1F>(labels[19], "", frdr, mpthf); // pjet_u_dr
    new_histograms[20] = new memory<TH1F>(labels[20], "", frdr, mpthf); // mix_pjet_u_dr
    new_histograms[21] = new memory<TH1F>(labels[21], "", frdr, mpthf); // sub_pjet_u_dr

    /* read inputs */
    std::vector<TFile*> files(inputs.size(), nullptr);
    zip([&](auto& file, auto const& input) {
        file = new TFile(input.data(), "read");
    }, files, inputs);

    /* load histograms and combine */
    for (size_t i = 0; i < labels.size(); ++i) {
        std::vector<history<TH1F>*> histograms(inputs.size(), nullptr);

        zip([&](auto const file, auto& hist) {
            auto name = tag + "_"s + labels[i];
            hist = new history<TH1F>(file, name);
        }, files, histograms);


        for (int64_t j = 0; j < histograms[0]->size(); ++j) {
            (*new_histograms[i])[ x{j, 0}] = (TH1F*) (*histograms[0])[j]->Clone();
            (*new_histograms[i])[ x{j, 1}] = (TH1F*) (*histograms[1])[j]->Clone();
            (*new_histograms[i])[ x{j, 2}] = (TH1F*) (*histograms[2])[j]->Clone();
            (*new_histograms[i])[ x{j, 3}] = (TH1F*) (*histograms[3])[j]->Clone();
        }

        new_histograms[i]->rename();
    }

    /* save histograms */
    in(output, [&]() {
        for (auto new_histogram : new_histograms) {
            new_histogram->save(tag);
        }
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return hallucinate(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
