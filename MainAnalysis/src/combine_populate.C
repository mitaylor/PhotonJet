#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom3.h"

#include <memory>
#include <string>
#include <vector>
#include <ctime>

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename... T>
void scale(double factor, T*... args) {
    (void)(int [sizeof...(T)]) { (args->scale(factor), 0)... };
}

template <typename... T>
void scale_bin_width(T*... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1., "width"); }), 0)... };
}

int populate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto inputs = conf->get<std::vector<std::string>>("inputs");
    auto group = conf->get<std::string>("group");
    auto labels = conf->get<std::vector<std::string>>("labels");

    // auto rjpt = conf->get<std::vector<float>>("jpt_range");
    // auto rdphi = conf->get<std::vector<float>>("dphi_range");
    // auto rdr = conf->get<std::vector<float>>("dr_range");

    // auto rrdr = conf->get<std::vector<float>>("rdr_range");
    // auto rrdphi = conf->get<std::vector<float>>("rdphi_range");
    // auto rrpt = conf->get<std::vector<float>>("rpt_range");

    // auto dpt = conf->get<std::vector<float>>("pt_diff");
    // auto dhf = conf->get<std::vector<float>>("hf_diff");

    /* convert to integral angle units (cast to double) */
    // convert_in_place_pi(rdphi);

    // auto ipt = new interval(dpt);
    // auto ihf = new interval(dhf);

    // auto mpthf = new multival(dpt, dhf);

    // auto incl = new interval(""s, 1, 0.f, 9999.f);
    // auto idphi = new interval("#Delta#phi^{#gammaj}"s, rdphi);
    // auto idr = new interval("#deltaj"s, rdr);
    // auto ijpt = new interval("p_{T}^{j}"s, rjpt);

    // auto mdr = new multival(rrdr, rrpt);
    // auto mdphi = new multival(rrdphi, rrpt);

    // auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    // auto fdphi = std::bind(&interval::book<TH1F>, idphi, _1, _2, _3);
    // auto fdr = std::bind(&interval::book<TH1F>, idr, _1, _2, _3);
    // auto fjpt = std::bind(&interval::book<TH1F>, ijpt, _1, _2, _3);

    // auto frdr = [&](int64_t, std::string const& name, std::string const&) {
    //     return new TH1F(name.data(), ";index;", mdr->size(), 0, mdr->size()); };
    // auto frdphi = [&](int64_t, std::string const& name, std::string const&) {
    //     return new TH1F(name.data(), ";index;", mdphi->size(), 0, mdphi->size()); };

    /* open input files */
    std::vector<TFile*> files(inputs.size(), nullptr);
    zip([&](auto& file, auto const& input) {
        file = new TFile(input.data(), "read");
    }, files, inputs);

    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    /* combine nevt information */
    auto nevt = new history<TH1F>(files[0], group + "_nevt"s);
    auto nevt_0 = new history<TH1F>(*nevt, "nominal");

    for (size_t i = 1; i < files.size(); ++i) {
        auto nevt_add = new history<TH1F>(files[i], group + "_nevt"s);
        *nevt += *nevt_add;
    }

    nevt->save();

    for (auto const& label : labels) {
        std::cout << label << std::endl;

        auto name = group + "_"s + label;
        auto hist = new history<TH1F>(files[0], name); 

        auto name_mix = group + "_mix_"s + label;
        auto hist_mix = new history<TH1F>(files[0], name_mix);

        // auto nevt = new history<TH1F>(files[0], group + "_nevt"s);

        hist->multiply(*nevt_0);
        hist_mix->multiply(*nevt_0);

        for (size_t i = 1; i < files.size(); ++i) {
            auto hist_add = new history<TH1F>(files[i], name);
            auto hist_mix_add = new history<TH1F>(files[i], name_mix);
            auto nevt_add = new history<TH1F>(files[i], group + "_nevt"s);

            hist_add->multiply(*nevt_add);
            hist_mix_add->multiply(*nevt_add);

            // std::cout << (*nevt_0)[0]->GetBinContent(1) << " " << (*nevt_add)[0]->GetBinContent(1) << std::endl;

            *hist += *hist_add;
            *hist_mix += *hist_mix_add;

            delete hist_add; delete hist_mix_add; delete nevt_add;
        }

        scale_bin_width(hist, hist_mix);

        hist->divide(*nevt);
        hist_mix->divide(*nevt);

        auto hist_sub = new history<TH1F>(*hist, "sub");
        *hist_sub -= *hist_mix;

        hist->save();
        hist_mix->save();
        hist_sub->save();
    }

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return populate(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}