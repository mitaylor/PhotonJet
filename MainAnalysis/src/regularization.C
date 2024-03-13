#include "../include/lambdas.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename T>
T* null(int64_t, std::string const&, std::string const&) {
    return nullptr;
}

void interpolate(TH1* h) {
    for (int64_t i = 1; i <= h->GetNBinsX(); ++i) {
        h->SetBinError(i, 0);

        if (h->GetBinContent(i) == 0) {
            float y1 = h->GetBinContent(i - 1);
            float x1 = i - 1;

            float y2 = y1;
            float x2 = x1;

            for (int64_t j = i + 1; j <= h->GetNBinsX(); ++j) {
                if (h->GetBinContent(j) == 0) { continue; }
                else {
                    y2 = h->GetBinContent(j);
                    x2 = j;
                }
            }

            for (int64_t j = 1; j < x2 - x1; ++j) {
                float value = y1 + j * (y2 - y1) / (x2 - x1);
                h->SetBinContent(x1 + j, value);
            }
        }
    }
}

int regularization(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto tag = conf->get<std::string>("tag");
    auto label = conf->get<std::string>("label");
    auto prior = conf->get<std::string>("prior");
    auto algorithm = conf->get<std::string>("algorithm");
    auto object = conf->get<std::string>("object");

    auto filenames = conf->get<std::vector<std::string>>("filenames");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");
    
    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto rpt = sel->get<std::vector<float>>("photon_pt_bounds");

    auto mpthf = new multival(rpt, dhf);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    std::vector<TFile*> files(filenames.size(), nullptr);

    zip([&](auto& file, auto const& filename) {
        file = new TFile(("unfolded/Theory/"s + set + "/"s + algorithm + "/"s + prior + "/kErrors/" + filename).data(), "read");
    }, files, filenames);

    /* prepare output from pre-unfolded data */
    TFile* fout = new TFile(output, "recreate");

    /* prepare the mse */
    auto mse = new history<TH1F>("mse", "", null<TH1F>, (int64_t) files.size());
    std::vector<int> choice;

    /* extract chosen histograms */
    for (size_t j = 0; j < files.size(); ++j) {
        auto HMSE = (TH1F*) files[j]->Get("HMSE");
        (*mse)[j] = HMSE;
        choice.push_back(HMSE->GetMinimumBin());
    }

    /* rename histograms */
    mse->rename(tag + "_mse"s);
    
    /* save histograms */
    mse->save();
    fout->Close();

    /* plotting setup */
    auto system_tag = "  #sqrt{s_{NN}} = 5.02 TeV"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;
    auto title = (algorithm == "Bayes") ? "Iterations"s : "k_{reg}"s;

    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", rpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.85, 0.04, mpthf, pt_info, hf_info); };

    auto minimum = [&](int64_t index) {
        auto min = "minimum: "s + to_text(choice[index-1]);

        TLatex* l = new TLatex();
        l->SetTextAlign(11);
        l->SetTextFont(43);
        l->SetTextSize(13);
        l->DrawLatexNDC(0.135, 0.75, min.data());
    };

    /* plot histograms */
    auto hb = new pencil();

    hb->category("type", "MSE", "V", "B^{2}");
    // hb->category("algorithm", "Bayes", "SVD");
    // hb->category("prior", "MC", "Flat");
    // hb->category("object", "Data", "MC");
    // hb->category("label", "Pythia", "JewelAA", "JewelPP", "JewelNoRecoilAA", "PyquenAA", "PyquenPP","PyquenNoWideAA");

    auto p = new paper(set + "_regularization_" + tag + "_" + object + "_" + algorithm + "_" + label + "_" + prior, hb);

    p->divide(files.size(), -1);
    p->accessory(pthf_info);
    p->accessory(minimum);
    apply_style(p, cms, system_tag);
    p->set(paper::flags::logy);

    for (size_t i = 0; i < files.size(); ++i) {
        interpolate((*mse)[i]);
        (*mse)[i]->GetXaxis()->SetTitle(title.data());
        p->add((*mse)[i], "MSE");
        p->adjust((*mse)[i], "l", "l");
    }

    hb->sketch();
    p->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return regularization(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
