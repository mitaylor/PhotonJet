#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"

#include <memory>
#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;


int populate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto data = conf->get<std::string>("data");
    auto mc = conf->get<std::string>("mc");
    auto tag = conf->get<std::string>("tag");
    std::string histname = tag + "_pjet_es_f_dphi"s;

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto diso = conf->get<std::vector<float>>("iso_diff");
    auto djtmin = conf->get<std::vector<float>>("jtmin_diff");

    /* convert to integral angle units (cast to double) */
    convert_in_place_pi(rdphi);

    /* push back vectors for the photon isolation cone and min jet pT */
    diso.push_back(999999);
    djtmin.push_back(999999);

    /* exclude most peripheral events */
    auto ipt = new interval(dpt);
    auto ihf = new interval(dhf);
    auto iiso = new interval(diso);
    auto ijtmin = new interval(djtmin);

    auto mindex = new multival(dpt, dhf, diso, djtmin);
    
    TFile* fd = new TFile(data.data(), "read");
    auto data_pjet_es_f_dphi = new history<TH1F>(fd, histname);

    TFile* fm = new TFile(data.data(), "read");
    auto mc_pjet_es_f_dphi = new history<TH1F>(fm, histname);


    auto p = new pencil();
    hb->category("type", "data", "mc");

    auto canvases = ipt->size() * ihf->size();
    std::vector<paper*> c(canvases, nullptr);
    std::vector<int64_t> shape { ijtmin->size(), iiso->size() };

    std::function<void(int64_t, float)> iso_info = [&](int64_t x, float pos) {
        info_text(x, pos, "R_{iso,#gamma} = %.1f", diso); };

    std::function<void(int64_t, float)> jt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Jet min p_{T} = %.0f", djtmin); };

    auto iso_jt_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, shape, jt_info, iso_info); };

    for (int i = 0; i < ipt->size(); ++i) {
        for (int j = 0; j < ihf->size(); ++j) {
            auto canvas = i * ihf->size() + j;
            auto suffix = "_" + std::to_string(i) + "_" + std::to_string(j);

            auto index = mindex->index_for(x{i, j, 0, 0});
            auto data_max = (*data_pjet_es_f_dphi)[index]->GetMaximum();
            auto data_min = (*data_pjet_es_f_dphi)[index]->GetMinimum();

            auto mc_max = (*mc_pjet_es_f_dphi)[index]->GetMaximum();
            auto mc_min = (*mc_pjet_es_f_dphi)[index]->GetMinimum();

            auto max = (data_max > mc_max) ? data_max : mc_max;
            auto min = (data_min < mc_min) ? data_min : mc_min;

            c[canvas] = new paper(tag + "_" + type + "_es_dphi" + suffix, p);
            apply_style(c[canvas], "");
            c[canvas]->divide(-1, iiso->size());

            c[canvas]->accessory(iso_jt_info);

            for (int k = 0; k < iiso->size(); ++k) {
                for (int l = 0; l < ijtmin->size(); ++l) {
                    index = mindex->index_for(x{i, j, k, l});
                    (*data_pjet_es_f_dphi)[index]->SetAxisRange(min, max, "Z");
                    (*mc_pjet_es_f_dphi)[index]->SetAxisRange(min, max, "Z");

                    c[canvas]->add((*data_pjet_es_f_dphi)[index], "data");
                    c[canvas]->adjust((*data_pjet_es_f_dphi)[index]);
                    c[canvas]->stack((*mc_pjet_es_f_dphi)[index]), "mc";
                    c[canvas]->adjust((*mc_pjet_es_f_dphi)[index]);
                }
            }
        }
    }

    hb->set_binary("type");
    p->sketch();

    for (auto canvas : c)
        canvas->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return populate(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
