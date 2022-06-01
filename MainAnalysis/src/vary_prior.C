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

using namespace std::literals::string_literals;
using namespace std::placeholders;

int data_mc_comparison(char const* config, const char* output) {
    auto conf = new configurer(config);

    auto input_data = conf->get<std::string>("input_data");
    auto data_label = conf->get<std::string>("data_label");

    auto input_truth = conf->get<std::string>("input_truth");
    auto truth_label = conf->get<std::string>("truth_label");

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
    TFile* fdata = new TFile(input_data.data(), "read");
    TFile* ftruth = new TFile(input_truth.data(), "read");

    auto h_data = new history<TH1F>(fdata, tag + "_"s + data_label);
    auto h_truth = new history<TH1F>(ftruth, tag + "_"s + truth_label);

    auto h_truth_up = new history<TH1F>(*h_truth, "up"s);
    auto h_truth_down = new history<TH1F>(*h_truth, "down"s);

    if (hdata->size() != htruth->size()) {
        std::cout << "Size mismatch" << std::endl;
        return;
    }

    auto size = h_data->size();

    for (int64_t i = 0; i < size; ++i) {
        for (int64_t j = 0; j < ipt->size(); ++j) {
            for (int64_t k = 0; k < idr->size(); ++k) {
                auto bin = mg->index_for(x{k, j});
                auto center = (*h_truth)[i]->GetBinContent(bin + 1);
                auto error = (*h_data)[i]->GetBinError(bin + 1);

                float scale = 1.0 - (double) k / (double) idr->size() * 2.0;

                (*h_truth_up)->SetBinContent(center + error * scale);
                (*h_truth_down)->SetBinContent(center - error * scale);
            }
        }
    }


    in(output, [&]() {
        h_truth_up->save();
        h_truth_down->save();
        h_truth->save();
        h_data->save();
    });

    // /* set up figures */
    // auto collisions = "#sqrt{s_{NN}} = 5.02 TeV"s;

    // std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
    //     info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    // std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
    //     info_text(x, pos, "%i - %i%%", dcent, true); };

    // auto pthf_info = [&](int64_t index) {
    //     stack_text(index, 0.75, 0.04, mpthf, pt_info, hf_info); };

    // auto hb = new pencil();
    // hb->category("type", "nominal", "vary_up", "vary_down");

    // /* (1) unfolded MC vs reco truth dr */
    // auto p1 = new paper(tag + "_dj_unfolded_mc_vs_truth_reco_iso", hb);
    // p1->divide(ihf->size(), -1);
    // p1->accessory(pthf_info);
    // apply_style(p1, collisions, -2., 27.);
    // p1->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));
    
    // h_r_qcd_after->apply([&](TH1* h) { p1->add(h, "qcd_after"); });
    // h_r_truth_reco_iso->apply([&](TH1* h, int64_t index) { p1->stack(index + 1, h, "truth_reco_iso"); });

    // p1->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return data_mc_comparison(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
