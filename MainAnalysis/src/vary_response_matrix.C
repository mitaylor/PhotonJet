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

int vary_response_matrix(char const* config, char const* selections, const char* output) {
    auto conf = new configurer(config);

    auto input_truth = conf->get<std::string>("input_truth");
    auto truth_label_g = conf->get<std::string>("truth_label_g");
    auto truth_label_r = conf->get<std::string>("truth_label_r");
    auto truth_label_c = conf->get<std::string>("truth_label_c");

    auto tag = conf->get<std::string>("tag");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    /* load history objects */
    TFile* ftruth = new TFile((base + input_truth).data(), "read");

    auto h_truth_g = new history<TH1F>(ftruth, tag + "_"s + truth_label_g);
    auto h_truth_r = new history<TH1F>(ftruth, tag + "_"s + truth_label_r);
    auto h_truth_c = new history<TH1F>(ftruth, tag + "_"s + truth_label_c);

    auto h_truth_g_merge = new history<TH1F>(ftruth, tag + "_"s + truth_label_g + "_merge");
    auto h_truth_r_merge = new history<TH1F>(ftruth, tag + "_"s + truth_label_r + "_merge");
    auto h_truth_c_merge = new history<TH1F>(ftruth, tag + "_"s + truth_label_c + "_merge");

    auto size = h_truth_c->size();

    auto rng = new TRandom3(180);

    for (int64_t i = 0; i < size; ++i) {
        for (int64_t j = 0; j < (*h_truth_c)[i]->GetNbinsX(); ++j) {
            for (int64_t k = 0; k < (*h_truth_c)[i]->GetNbinsY(); ++k) {
                auto center = (*h_truth_c)[i]->GetBinContent(j + 1, k + 1);
                auto error = (*h_truth_c)[i]->GetBinError(j + 1, k + 1);

                if (center == 0) { continue; }
                auto new_center = center + rng->Gaus(0, error/2);

                if (new_center > 0) (*h_truth_c)[i]->SetBinContent(j + 1, k + 1, new_center);
                else                (*h_truth_c)[i]->SetBinContent(j + 1, k + 1, 0);
            }
        }
    }

    for (int64_t j = 0; j < (*h_truth_c_merge)[0]->GetNbinsX(); ++j) {
        for (int64_t k = 0; k < (*h_truth_c_merge)[0]->GetNbinsY(); ++k) {
            auto center = (*h_truth_c_merge)[0]->GetBinContent(j + 1, k + 1);
            auto error = (*h_truth_c_merge)[0]->GetBinError(j + 1, k + 1);

            if (center == 0) { continue; }
            auto new_center = center + rng->Gaus(0, error/2);

            if (new_center > 0) (*h_truth_c_merge)[0]->SetBinContent(j + 1, k + 1, new_center);
            else                (*h_truth_c_merge)[0]->SetBinContent(j + 1, k + 1, 0);
        }
    }

    in(output, [&]() {
        h_truth_g->save();
        h_truth_r->save();
        h_truth_c->save();

        h_truth_g_merge->save();
        h_truth_r_merge->save();
        h_truth_c_merge->save();
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return vary_response_matrix(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
