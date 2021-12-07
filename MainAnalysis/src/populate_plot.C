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

void scale_TH2(history<TH2F>* hist, history<TH1F>* scale) {
    auto size = hist->size();

    for (int64_t i = 0; i < size; ++i) {
        auto factor = (*scale)[i]->GetBinContent(1);
        (*hist)[i]->Scale(1/factor);
    }
}

double get_max(TH1* h1, TH1* h2) {
    auto max_1 = h1->GetMaximum();
    auto max_2 = h2->GetMaximum();
    auto max = (max_1 > max_2) ? max_1 : max_2;
    max *= 1.2;
    return max;
}

double get_min(TH1* h1, TH1* h2) {
    auto min_1 = h1->GetMinimum();
    auto min_2 = h2->GetMinimum();
    auto min = (min_1 < min_2) ? min_1 : min_2;
    min = min > 0 ? 0 : min * 1.2;
    return min;
}

int populate(char const* config) {
    auto conf = new configurer(config);

    auto data = conf->get<std::string>("data");
    auto mc = conf->get<std::string>("mc");
    auto tag = conf->get<std::string>("tag");
    auto system = conf->get<std::string>("system");
    auto matching = conf->get<std::string>("matching");

    auto rdphi = conf->get<std::vector<float>>("dphi_range");
    auto rdr = conf->get<std::vector<float>>("dr_range");

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
    
    /* load data */
    TFile* fd = new TFile(data.data(), "read");
    auto data_pjet_es_f_dphi = new history<TH1F>(fd, tag + "_sub_pjet_es_f_dphi"s);
    auto data_pjet_es_f_dr = new history<TH1F>(fd, tag + "_sub_pjet_f_dr"s);
    auto data_pjet_dphi_deta = new history<TH2F>(fd, tag + "_sub_pjet_dphi_deta"s);
    auto data_nevt = new history<TH1F>(fd, tag + "_nevt"s);

    TFile* fm = new TFile(mc.data(), "read");
    auto mc_pjet_es_f_dphi = new history<TH1F>(fm, "qcd_sub_pjet_es_f_dphi"s);
    auto mc_pjet_es_f_dr = new history<TH1F>(fm, "qcd_sub_pjet_f_dr"s);
    auto mc_pjet_dphi_deta = new history<TH2F>(fm, "qcd_sub_pjet_dphi_deta"s);
    auto mc_nevt = new history<TH1F>(fm, "qcd_nevt"s);

    scale_TH2(data_pjet_dphi_deta, data_nevt);
    scale_TH2(mc_pjet_dphi_deta, mc_nevt);

    /* plot info */
    std::vector<int64_t> shape { ijtmin->size(), iiso->size() };

    std::function<void(int64_t, float)> iso_info = [&](int64_t x, float pos) {
        info_text(x, pos, "R_{iso,#gamma} = %.1f", diso); };
    std::function<void(int64_t, float)> jt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Jet min p_{T} = %.0f", djtmin); };
    auto iso_jt_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, shape, jt_info, iso_info); };

    /* do plotting */
    auto p = new pencil();
    p->category("type", "data", "mc");

    auto canvases = ipt->size() * ihf->size();
    std::vector<paper*> c1(canvases, nullptr);
    std::vector<paper*> c2(canvases, nullptr);
    std::vector<paper*> c3(canvases, nullptr);
    std::vector<paper*> c4(canvases, nullptr);

    auto redraw_dphi_axis = [&](TH1* h, int64_t) {
        transform_axis(h, [](int64_t val) -> float { return std::abs(revert_radian(val)); }); };

    for (int i = 0; i < ipt->size(); ++i) {
        for (int j = 0; j < ihf->size(); ++j) {
            auto canvas = i * ihf->size() + j;
            auto suffix = "_" + std::to_string(i) + "_" + std::to_string(j);

            auto index = mindex->index_for(x{i, j, 0, 0});

            auto max_dphi = get_max((*data_pjet_es_f_dphi)[index], (*mc_pjet_es_f_dphi)[index]);
            auto min_dphi = get_min((*data_pjet_es_f_dphi)[index], (*mc_pjet_es_f_dphi)[index]);
            auto max_dr = get_max((*data_pjet_es_f_dr)[index], (*mc_pjet_es_f_dr)[index]);
            auto min_dr = get_min((*data_pjet_es_f_dr)[index], (*mc_pjet_es_f_dr)[index]);
            auto max_dpde = get_max((*data_pjet_dphi_deta)[index], (*mc_pjet_dphi_deta)[index]);
            auto min_dpde = get_min((*data_pjet_dphi_deta)[index], (*mc_pjet_dphi_deta)[index]);

            c1[canvas] = new paper(system + "_" + tag + "_es_dphi_" + matching + suffix, p);
            c2[canvas] = new paper(system + "_" + tag + "_dr_" + matching + suffix, p);
            c3[canvas] = new paper(system + "_" + tag + "_mc_dphi_deta_" + matching + suffix, p);
            c4[canvas] = new paper(system + "_" + tag + "_data_dphi_deta_" + matching + suffix, p);

            apply_style(c1[canvas], "");
            apply_style(c2[canvas], "");
            apply_style(c3[canvas], "");
            apply_style(c4[canvas], "");

            c1[canvas]->divide(-1, iiso->size());
            c2[canvas]->divide(-1, iiso->size());
            c3[canvas]->divide(-1, iiso->size());
            c4[canvas]->divide(-1, iiso->size());

            c1[canvas]->accessory(std::bind(line_at, _1, 0.f, rdphi[0], rdphi[1]));
            c2[canvas]->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

            c1[canvas]->jewellery(redraw_dphi_axis);

            c1[canvas]->accessory(iso_jt_info);
            c2[canvas]->accessory(iso_jt_info);
            c3[canvas]->accessory(iso_jt_info);
            c4[canvas]->accessory(iso_jt_info);

            for (int k = 0; k < iiso->size(); ++k) {
                for (int l = 0; l < ijtmin->size(); ++l) {
                    index = mindex->index_for(x{i, j, k, l});

                    (*data_pjet_es_f_dphi)[index]->SetAxisRange(min_dphi, max_dphi, "Y");
                    (*mc_pjet_es_f_dphi)[index]->SetAxisRange(min_dphi, max_dphi, "Y");

                    (*data_pjet_es_f_dr)[index]->SetAxisRange(min_dr, max_dr, "Y");
                    (*mc_pjet_es_f_dr)[index]->SetAxisRange(min_dr, max_dr, "Y");

                    (*data_pjet_dphi_deta)[index]->SetAxisRange(min_dpde, max_dpde, "Z");
                    (*mc_pjet_dphi_deta)[index]->SetAxisRange(min_dpde, max_dpde, "Z");

                    c1[canvas]->add((*data_pjet_es_f_dphi)[index], "data");
                    c1[canvas]->stack((*mc_pjet_es_f_dphi)[index], "mc");

                    c2[canvas]->add((*data_pjet_es_f_dr)[index], "data");
                    c2[canvas]->stack((*mc_pjet_es_f_dr)[index], "mc");

                    c3[canvas]->add((*mc_pjet_dphi_deta)[index]);
                    c3[canvas]->adjust((*mc_pjet_dphi_deta)[index], "colz", "");

                    c4[canvas]->add((*data_pjet_dphi_deta)[index]);
                    c4[canvas]->adjust((*data_pjet_dphi_deta)[index], "colz", "");
                }
            }
        }
    }

    p->set_binary("type");
    p->sketch();

    for (auto canvas : c1)
        canvas->draw("pdf");
    for (auto canvas : c2)
        canvas->draw("pdf");
    for (auto canvas : c3)
        canvas->draw("pdf");
    for (auto canvas : c4)
        canvas->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 2)
        return populate(argv[1]);

    printf("usage: %s [config]\n", argv[0]);
    return 1;
}
