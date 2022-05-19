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

int quantitate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto tag = conf->get<std::string>("tag");

    auto before = conf->get<std::string>("before");
    auto before_label = conf->get<std::string>("before_label");
    auto before_figures = conf->get<std::vector<std::string>>("before_figures");

    // auto after = conf->get<std::vector<std::string>>("after");
    // auto after_labels = conf->get<std::vector<std::string>>("after_labels");
    // auto after_figures = conf->get<std::vector<std::string>>("after_figures");

    auto rdrr = conf->get<std::vector<float>>("drr_range");
    auto rptr = conf->get<std::vector<float>>("ptr_range");

    // auto rdrg = conf->get<std::vector<float>>("drg_range");
    // auto rptg = conf->get<std::vector<float>>("ptg_range");

    /* create intervals and multivals */
    auto idrr = new interval(xlabel, rdrr);
    auto iptr = new interval("p_{T}^{j}"s, rptr);

    // auto idrg = new interval(xlabel, rdrg);
    // auto iptg = new interval("p_{T}^{j}"s, rptg);

    auto mr = new multival(*idrr, *iptr);
    // auto mg = new multival(*idrg, *iptg);

    /* set offsets for folding pre and post unfolding so that jets between 30-120 are used */
    std::array<int64_t, 4> osr = { 0, 0, 1, 3 };
    // std::array<int64_t, 4> osg = { 0, 0, 2, 0 };

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    TFile* fbefore = new TFile(before.data(), "read");

    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    zip([&](auto const& figure) {
        auto stub = "_"s + figure;

        auto hin = new history<TH1F>(fbefore, tag + "_"s + before_label + stub);
        hin->save();

    }, before_figures);

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return quantitate(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
