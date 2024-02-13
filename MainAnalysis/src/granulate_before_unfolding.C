#include "../git/config/include/configurer.h"

#include "../git/history/include/history.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TH1.h"

#include <functional>
#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int granulate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto file = conf->get<std::string>("file");
    auto label = conf->get<std::string>("label");
    auto tag = conf->get<std::string>("tag");

    auto reference = conf->get<std::string>("reference");
    auto variation = conf->get<std::string>("variation");
    auto reference_label = conf->get<std::string>("reference_label");
    auto variation_label = conf->get<std::string>("variation_label");

    auto scale = conf->get<float>("scale");
    auto figures = conf->get<std::vector<std::string>>("figures");

    auto use_reference_stub = conf->get<bool>("use_reference_stub");
    auto use_variation_stub = conf->get<bool>("use_variation_stub");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input files */
    auto f = new TFile((base + file).data(), "read");
    auto r = new TFile((base + reference).data(), "read");
    auto v = new TFile((base + variation).data(), "read");

    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    /* calculate variations */
    zip([&](auto const& figure) {
        auto stub = "_"s + figure;

        auto base = new history<TH1F>(f, tag + "_"s + label + stub);

        auto reference_stub = use_reference_stub ? stub : "";
        auto variation_stub = use_variation_stub ? stub : "";

        auto ref = new history<TH1F>(r, tag + "_"s + reference_label + reference_stub);
        auto var = new history<TH1F>(v, tag + "_"s + variation_label + variation_stub);

        if ((*ref)[0]->GetNbinsX() == (*var)[0]->GetNbinsX()) {
            /* determine variation */
            var->apply([&](TH1* h, int64_t index) {
                h->Divide((*ref)[index]); });

            /* scale variation */
            if (scale != 0) { var->apply([&](TH1* h) {
                for_contents([&](std::array<double, 1> val) -> float {
                    return 1. + scale * (val[0] - 1.); }, h); }); }

            /* apply variation to base */
            var->apply([&](TH1* h, int64_t index) {
                h->Multiply((*base)[index]); });

            /* save histograms */
            var->save(tag + "_mod");
        }
    }, figures);

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return granulate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
