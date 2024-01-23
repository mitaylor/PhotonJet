#include "../include/lambdas.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

using namespace std::literals::string_literals;
using namespace std::placeholders;

int vary_prior(char const* config, char const* selections, const char* output) {
    auto conf = new configurer(config);

    auto input_data = conf->get<std::string>("input_data");
    auto data_label = conf->get<std::string>("data_label");

    auto input_truth = conf->get<std::string>("input_truth");
    auto truth_label_g = conf->get<std::string>("truth_label_g");
    auto truth_label_r = conf->get<std::string>("truth_label_r");
    auto truth_label_c = conf->get<std::string>("truth_label_c");

    auto direction = conf->get<bool>("direction");

    auto tag = conf->get<std::string>("tag");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto rdr = sel->get<std::vector<float>>("drg_range");
    auto rpt = sel->get<std::vector<float>>("ptg_range");

    /* create intervals and multivals */
    auto ipt= new interval(rpt);
    auto idr = new interval(rdr);
    auto mg = new multival(rdr, rpt);

    /* load history objects */
    TFile* fdata = new TFile((base + input_data).data(), "read");
    TFile* ftruth = new TFile((base + input_truth).data(), "read");

    auto h_data = new history<TH1F>(fdata, tag + "_"s + data_label + "_sum0_unfolded"s);
    auto h_truth_g = new history<TH1F>(ftruth, tag + "_"s + truth_label_g);
    auto h_truth_r = new history<TH1F>(ftruth, tag + "_"s + truth_label_r);
    auto h_truth_c = new history<TH1F>(ftruth, tag + "_"s + truth_label_c);

    auto h_data_merge = new history<TH1F>(fdata, tag + "_"s + data_label + "_merge_unfolded"s);
    auto h_truth_g_merge = new history<TH1F>(ftruth, tag + "_"s + truth_label_g + "_merge"s);
    auto h_truth_r_merge = new history<TH1F>(ftruth, tag + "_"s + truth_label_r + "_merge"s);
    auto h_truth_c_merge = new history<TH1F>(ftruth, tag + "_"s + truth_label_c + "_merge"s);

    if (h_data->size() != h_truth_g->size()) {
        std::cout << "Size mismatch" << std::endl;
        return -1;
    }

    auto size = h_data->size();

    for (int64_t i = 0; i < size; ++i) {
        for (int64_t j = 0; j < ipt->size(); ++j) {
            for (int64_t k = 0; k < idr->size(); ++k) {
                auto bin = mg->index_for(x{k, j});
                auto center = (*h_truth_g)[i]->GetBinContent(bin + 1);
                auto error = (*h_data)[i]->GetBinError(bin + 1);

                float scale = 1.0 - (double) k / (double) idr->size() * 2.0;

                auto new_center = (direction) ? center + error * scale : center - error * scale;

                if (new_center > 0) (*h_truth_g)[i]->SetBinContent(bin + 1, new_center);
                else                (*h_truth_g)[i]->SetBinContent(bin + 1, 0);
            }
        }
    }

    for (int64_t j = 0; j < ipt->size(); ++j) {
        for (int64_t k = 0; k < idr->size(); ++k) {
            auto bin = mg->index_for(x{k, j});
            auto center = (*h_truth_g_merge)[0]->GetBinContent(bin + 1);
            auto error = (*h_data_merge)[0]->GetBinError(bin + 1);

            float scale = 1.0 - (double) k / (double) idr->size() * 2.0;

            auto new_center = (direction) ? center + error * scale : center - error * scale;

            if (new_center > 0) (*h_truth_g_merge)[0]->SetBinContent(bin + 1, new_center);
            else                (*h_truth_g_merge)[0]->SetBinContent(bin + 1, 0);
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
        return vary_prior(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}