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

int combine_binning(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto base = conf->get<std::string>("base");
    auto inputs = conf->get<std::vector<std::string>>("inputs");
    auto group = conf->get<std::string>("group");
    auto labels = conf->get<std::vector<std::string>>("labels");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");

    /* open input files */
    std::vector<TFile*> files(inputs.size(), nullptr);
    zip([&](auto& file, auto const& input) {
        auto file_name = base + "/" + set + "/" + input;
        file = new TFile(file_name.data(), "read");
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
        auto hist = new history<TH2F>(files[0], name); 

        for (size_t i = 1; i < files.size(); ++i) {
            auto hist_add = new history<TH2F>(files[i], name);
            auto hist_mix_add = new history<TH2F>(files[i], name_mix);

            *hist += *hist_add;
            *hist_mix += *hist_mix_add;

            delete hist_add; delete nevt_add;
        }

        hist->apply([](TH1* h) { 
            for (int i = 0; i < h->GetNbinsX(); ++i) {
                if (h->GetBinContent(i + 1) == 0) {
                    h->SetBinError(i + 1, 1);
                }
            }
        });


        auto hist_sub = new history<TH2F>(*hist, "sub");
        hist_sub->rename(group + "_sub_" + label);
        *hist_sub -= *hist_mix;

        hist->save();
        hist_mix->save();
        hist_sub->save();
    }

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return combine_binning(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}