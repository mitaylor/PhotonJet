#include "../include/lambdas.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TColor.h"
#include "TFile.h"
#include "TH1.h"
#include "TLatex.h"
#include "TLine.h"

#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

static auto const red = TColor::GetColor("#f2777a");
static auto const blue = TColor::GetColor("#6699cc");

int congratulate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto inputs = conf->get<std::vector<std::string>>("inputs");
    auto tags = conf->get<std::vector<std::string>>("tags");
    auto new_tags = conf->get<std::vector<std::string>>("new_tags");

    auto ymin = conf->get<float>("ymin");
    auto ymax = conf->get<float>("ymax");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");
    auto ddr = conf->get<std::vector<float>>("dr_diff");

    auto ihf = new interval(dhf);
    auto idr = new interval(ddr);

    auto mdrhf = new multival(*idr, *ihf);

    std::vector<float>::const_iterator first = ddr.begin();
    std::vector<float>::const_iterator last = ddr.begin() + idr->index_for(0.2);
    std::vector<float> ddr_short(first, last);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* open input files */
    std::vector<TFile*> files(inputs.size(), nullptr);
    zip([&](auto& file, auto const& input) {
        file = new TFile(input.data(), "read");
    }, files, inputs);

    /* load histograms */
    std::vector<history<TH1F>*> hists(6, nullptr);

    zip([&](auto& hist, auto const file, auto const& tag, auto const& new_tag) {
        hist = new history<TH1F>(file, tag + "_angle_r_f_pt");
        hist->rename(new_tag + "_angle_r_f_pt");
    }, hists, files, tags, new_tags);

    /* prepare plots */
    auto hb = new pencil();
    hb->category("system", "pp", "aa", "ss");

    hb->alias("aa", "PbPb");
    hb->alias("ss", "pp (smeared)");

    std::function<void(int64_t, float)> dr_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.2f < #deltaj < %.2f", ddr_short, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%i - %i%%", dcent, true); };

    auto drhf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, hists[0], dr_info, hf_info); };

    auto collisions = "#sqrt{s_{NN}} = 5.02 TeV"s;

    /* prepare paper */
    auto width = idr->index_for(0.2) + 1;
    auto s = new paper("smeared_pp_dj_resolution", hb);
    apply_style(s, collisions, ymin, ymax);
    s->accessory(drhf_info);
    s->divide(width, ihf->size());

    /* draw histograms with uncertainties */
    hists[0]->apply([&](TH1* h, int64_t index) {
        auto indices = mdrhf->indices_for(index);
        if (indices[0] <= idr->index_for(0.2)) {
            std::cout << indices[0] << " " << indices[1] << std::endl;
            s->add(h, "aa");
        }
    });

    for (int64_t i = 0; i < 4; ++i) {
        hists[i + 2]->apply([&](TH1* h, int64_t index) {
            if (index <= idr->index_for(0.2)) {
                s->stack(i * width + index + 1, h, "ss");
            }
        });
    }

    auto pp_style = [](TH1* h) {
        h->SetLineColor(1);
        h->SetMarkerStyle(25);
        h->SetMarkerSize(0.60);
    };

    auto aa_style = [](TH1* h) {
        h->SetLineColor(1);
        h->SetMarkerStyle(20);
        h->SetMarkerSize(0.60);
    };

    hb->style("pp", pp_style);
    hb->style("aa", aa_style);
    hb->style("ss", pp_style);
    hb->sketch();

    s->draw("pdf");

    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return congratulate(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
