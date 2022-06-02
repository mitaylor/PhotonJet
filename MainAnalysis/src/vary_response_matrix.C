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
#include "TRandom3.h"

using namespace std::literals::string_literals;
using namespace std::placeholders;

int vary_response_matrix(char const* config, const char* output) {
    auto conf = new configurer(config);

    auto input_truth = conf->get<std::string>("input_truth");
    auto truth_label_g = conf->get<std::string>("truth_label_g");
    auto truth_label_r = conf->get<std::string>("truth_label_r");
    auto truth_label_c = conf->get<std::string>("truth_label_c");

    auto direction = conf->get<bool>("direction");

    auto tag = conf->get<std::string>("tag");

    auto rdr = conf->get<std::vector<float>>("drg_range");
    auto rpt = conf->get<std::vector<float>>("ptg_range");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    /* create intervals and multivals */
    auto ipt= new interval(rpt);
    auto idr = new interval(rdr);
    auto mg = new multival(rdr, rpt);

    // auto ihf = new interval(dhf);
    // auto mpthf = new multival(dpt, dhf);

    /* load history objects */
    TFile* ftruth = new TFile(input_truth.data(), "read");

    auto h_truth_g = new history<TH1F>(ftruth, tag + "_"s + truth_label_g);
    auto h_truth_r = new history<TH1F>(ftruth, tag + "_"s + truth_label_r);
    auto h_truth_c = new history<TH1F>(ftruth, tag + "_"s + truth_label_c);

    auto size = h_truth_c->size();

    auto rng = new TRandom3(180);

    for (int64_t i = 0; i < size; ++i) {
        for (int64_t j = 0; j < (*h_truth_c)[i]->GetNbinsX(); ++j) {
            for (int64_t k = 0; k < (*h_truth_c)[i]->GetNbinsY(); ++k) {
                auto center = (*h_truth_c)[i]->GetBinContent(j + 1, k + 1);
                auto error = (*h_truth_c)[i]->GetBinError(j + 1, k + 1);

                if (center == 0) { continue; }
                auto new_center = center + rng->Gaus(0, error);

                if (new_center > 0) (*h_truth_g)[i]->SetBinContent(j + 1, k + 1, new_center);
                else                (*h_truth_g)[i]->SetBinContent(j + 1, k + 1, 0);
            }
        }
    }

    in(output, [&]() {
        h_truth_g->save();
        h_truth_r->save();
        h_truth_c->save();
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return vary_response_matrix(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
