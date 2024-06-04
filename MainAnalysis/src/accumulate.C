#include "../include/text.h"
#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLatex.h"
#include "TLine.h"

#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename T>
T* null(int64_t, std::string const&, std::string const&) {
    return nullptr;
}

TH2F* variance(TH1* flat, multival const* m) {
    auto cov = new TH2F("cov", "", m->size(), 0, m->size(),
        m->size(), 0, m->size());

    for (int64_t i = 0; i < m->size(); ++i) {
        auto err = flat->GetBinError(i + 1);
        cov->SetBinContent(i + 1, i + 1, err * err);
    }

    return cov;
}

TH1F* fold(TH1* flat, TH2* covariance, multival const* m, int64_t axis,
           std::vector<int64_t>& offsets) {
    auto name = std::string(flat->GetName()) + "_fold" + std::to_string(axis);
    auto hfold = m->axis(axis).book<TH1F, 2>(0, name, "",
        { offsets[axis << 1], offsets[(axis << 1) + 1] });

    auto shape = m->shape();

    auto front = std::vector<int64_t>(m->dims(), 0);
    auto back = std::vector<int64_t>(m->dims(), 0);
    for (int64_t i = 0; i < m->dims(); ++i) {
        front[i] = offsets[i << 1];
        back[i] = shape[i] - offsets[(i << 1) + 1];
    }

    auto size = back[axis] - front[axis];
    auto list = new std::vector<int64_t>[size];

    for (int64_t i = 0; i < m->size(); ++i) {
        auto indices = m->indices_for(i);

        bool flag = false;
        zip([&](int64_t index, int64_t f, int64_t b) {
            flag = flag || index < f || index >= b;
        }, indices, front, back);
        if (flag) { continue; }

        auto index = indices[axis] - front[axis];
        hfold->SetBinContent(index + 1, hfold->GetBinContent(index + 1)
            + flat->GetBinContent(i + 1));

        list[index].push_back(i);
    }

    auto cov = covariance ? (TH2F*)covariance->Clone() : variance(flat, m);

    for (int64_t i = 0; i < size; ++i) {
        auto indices = list[i];
        int64_t count = indices.size();

        auto error = 0.;
        for (int64_t j = 0; j < count; ++j) {
            auto j_x = indices[j] + 1;
            for (int64_t k = 0; k < count; ++k) {
                auto k_x = indices[k] + 1;
                error = error + cov->GetBinContent(j_x, k_x);
            }
        }

        hfold->SetBinError(i + 1, std::sqrt(error));
    }

    delete [] list;
    delete cov;

    hfold->Scale(1., "width");

    return hfold;
}

template <typename... T>
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

int accumulate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto label = conf->get<std::string>("label");
    auto type = conf->get<std::string>("type");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto rjpt = conf->get<std::vector<float>>("jpt_range");
    auto rdr = conf->get<std::vector<float>>("dr_range");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");

    auto plot = conf->get<bool>("plot");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto rdrr = sel->get<std::vector<float>>("drr_range");
    auto rptr = sel->get<std::vector<float>>("ptr_range");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_pt_min = sel->get<float>("jet_pt_min");
    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const photon_pt_min = sel->get<float>("photon_pt_min");
    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");

    auto dpt = sel->get<std::vector<float>>("photon_pt_diff");

    auto ihf = new interval(dhf);

    auto mr = new multival(rdrr, rptr);
    auto mpthf = new multival(dpt, dhf);

    std::vector<int32_t> drange = { dcent.front(), dcent.back() };
    std::vector<int64_t> osr = {0, 0, 0, 0};

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* open input files */
    TFile* f = new TFile((base + input).data(), "read");

    /* load histograms */
    auto nevt = new history<TH1F>(f, label + "_raw_nevt"s);

    auto pjet_f_dr = new history<TH1F>(
        f, label + "_"s + type + "_pjet_f_dr"s);
    auto pjet_f_jpt = new history<TH1F>(
        f, label + "_"s + type + "_pjet_f_jpt"s);
    auto pjet_u_dr_jpt = new history<TH1F>(
        f, label + "_"s + type + "_pjet_u_dr_jpt"s);

    /* rescale by number of signal photons (events) */
    pjet_f_dr->multiply(*nevt);
    pjet_f_jpt->multiply(*nevt);
    pjet_u_dr_jpt->multiply(*nevt);

    /* discard overflow photon pt bin */
    auto discard = [](history<TH1F>*& h, int64_t axis) {
        auto shape = h->shape();
        shape[axis] = shape[axis] - 1;
        h = h->shrink("s", shape, std::vector<int64_t>(h->dims(), 0));
    };
    
    discard(nevt, 0);
    discard(pjet_f_dr, 0);
    discard(pjet_f_jpt, 0);
    discard(pjet_u_dr_jpt, 0);

    /* project onto dj and jet pt */
    auto pjet_u_dr = new history<TH1F>("s_" + label + "_raw_sub_pjet_u_dr"s, "", null<TH1F>, nevt->shape());
    auto pjet_u_jpt = new history<TH1F>("s_" + label + "_raw_sub_pjet_u_jpt"s, "", null<TH1F>, nevt->shape());

    for (int64_t i = 0; i < nevt->size(); ++i) {
        (*pjet_u_dr)[i] = fold((*pjet_u_dr_jpt)[i], nullptr, mr, 0, osr);
        (*pjet_u_jpt)[i] = fold((*pjet_u_dr_jpt)[i], nullptr, mr, 1, osr);
    }

    pjet_f_dr->rename("s_" + label + "_raw_sub_pjet_f_dr"s);
    pjet_f_jpt->rename("s_" + label + "_raw_sub_pjet_f_jpt"s);
    pjet_u_dr_jpt->rename("s_" + label + "_raw_sub_pjet_u_dr_jpt"s);
    pjet_u_dr->rename("s_" + label + "_raw_sub_pjet_u_dr"s);
    pjet_u_jpt->rename("s_" + label + "_raw_sub_pjet_u_jpt"s);

    /* integrate histograms */
    auto nevt_d_pt = nevt->sum(1);
    auto nevt_d_hf = nevt->sum(0);
    auto pjet_f_dr_d_pt = pjet_f_dr->sum(1);
    auto pjet_f_dr_d_hf = pjet_f_dr->sum(0);
    auto pjet_f_jpt_d_pt = pjet_f_jpt->sum(1);
    auto pjet_f_jpt_d_hf = pjet_f_jpt->sum(0);
    auto pjet_u_dr_jpt_d_pt = pjet_u_dr_jpt->sum(1);
    auto pjet_u_dr_jpt_d_hf = pjet_u_dr_jpt->sum(0);
    auto pjet_u_dr_d_pt = pjet_u_dr->sum(1);
    auto pjet_u_dr_d_hf = pjet_u_dr->sum(0);
    auto pjet_u_jpt_d_pt = pjet_u_jpt->sum(1);
    auto pjet_u_jpt_d_hf = pjet_u_jpt->sum(0);

    auto nevt_merge = nevt_d_hf->extend("merge", 0, 1)->sum(1);
    auto pjet_f_dr_merge = pjet_f_dr_d_hf->extend("merge", 0, 1)->sum(1);
    auto pjet_f_jpt_merge = pjet_f_jpt_d_hf->extend("merge", 0, 1)->sum(1);
    auto pjet_u_dr_jpt_merge = pjet_u_dr_jpt_d_hf->extend("merge", 0, 1)->sum(1);
    auto pjet_u_dr_merge = pjet_u_dr_d_hf->extend("merge", 0, 1)->sum(1);
    auto pjet_u_jpt_merge = pjet_u_jpt_d_hf->extend("merge", 0, 1)->sum(1);

    nevt_merge->rename("s_" + label + "_raw_nevt_merge"s);
    pjet_f_dr_merge->rename("s_" + label + "_raw_sub_pjet_f_dr_merge"s);
    pjet_f_jpt_merge->rename("s_" + label + "_raw_sub_pjet_f_jpt_merge"s);
    pjet_u_dr_jpt_merge->rename("s_" + label + "_raw_sub_pjet_u_dr_jpt_merge"s);
    pjet_u_dr_merge->rename("s_" + label + "_raw_sub_pjet_u_dr_merge"s);
    pjet_u_jpt_merge->rename("s_" + label + "_raw_sub_pjet_u_jpt_merge"s);

    /* normalise by number of signal photons (events) */
    pjet_f_dr->divide(*nevt);
    pjet_f_jpt->divide(*nevt);
    pjet_u_dr_jpt->divide(*nevt);
    pjet_u_dr->divide(*nevt);
    pjet_u_jpt->divide(*nevt);

    pjet_f_dr_d_pt->divide(*nevt_d_pt);
    pjet_f_dr_d_hf->divide(*nevt_d_hf);
    pjet_f_jpt_d_pt->divide(*nevt_d_pt);
    pjet_f_jpt_d_hf->divide(*nevt_d_hf);
    pjet_u_dr_jpt_d_pt->divide(*nevt_d_pt);
    pjet_u_dr_jpt_d_hf->divide(*nevt_d_hf);
    pjet_u_dr_d_pt->divide(*nevt_d_pt);
    pjet_u_dr_d_hf->divide(*nevt_d_hf);
    pjet_u_jpt_d_pt->divide(*nevt_d_pt);
    pjet_u_jpt_d_hf->divide(*nevt_d_hf);

    pjet_f_dr_merge->divide(*nevt_merge);
    pjet_f_jpt_merge->divide(*nevt_merge);
    pjet_u_dr_jpt_merge->divide(*nevt_merge);
    pjet_u_dr_merge->divide(*nevt_merge);
    pjet_u_jpt_merge->divide(*nevt_merge);

    title(std::bind(rename_axis, _1, "1/N^{#gamma}dN/d#deltaj"),
        pjet_u_dr,
        pjet_u_dr_d_pt,
        pjet_u_dr_d_hf, 
        pjet_u_dr_merge);

    title(std::bind(rename_axis, _1, "1/N^{#gamma}dN/d#deltaj"),
        pjet_f_dr,
        pjet_f_dr_d_pt,
        pjet_f_dr_d_hf, 
        pjet_f_dr_merge);

    /* save histograms */
    in(output, [&]() {
        nevt->save(tag);

        pjet_f_dr->save(tag);
        pjet_f_jpt->save(tag);
        pjet_u_dr_jpt->save(tag);
        pjet_u_dr->save(tag);
        pjet_u_jpt->save(tag);

        pjet_f_dr_d_pt->save(tag);
        pjet_f_dr_d_hf->save(tag);
        pjet_f_jpt_d_pt->save(tag);
        pjet_f_jpt_d_hf->save(tag);
        pjet_u_dr_jpt_d_pt->save(tag);
        pjet_u_dr_jpt_d_hf->save(tag);
        pjet_u_dr_d_pt->save(tag);
        pjet_u_dr_d_hf->save(tag);
        pjet_u_jpt_d_pt->save(tag);
        pjet_u_jpt_d_hf->save(tag);

        pjet_f_dr_merge->save(tag);
        pjet_f_jpt_merge->save(tag);
        pjet_u_dr_jpt_merge->save(tag);
        pjet_u_dr_merge->save(tag);
        pjet_u_jpt_merge->save(tag);
    });

    /* draw plots */
    if (plot) {
        printf("painting..\n");

        auto system_tag = system + "  #sqrt{s_{NN}} = 5.02 TeV"s;
        system_tag += (system == "PbPb") ? ", 1.69 nb^{-1}"s : ", 302 pb^{-1}"s;
        auto cms = "#bf{#scale[1.4]{CMS}}"s;
        cms += " #it{#scale[1.2]{Preliminary}}"s;
        cms += "                                                                   ";
        cms += "                                                                   ";
        cms += "anti-k_{T} R = 0.3, p_{T}^{jet} > "s + to_text(jet_pt_min) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);
        cms += ", p_{T}^{#gamma} > "s + to_text(photon_pt_min) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs) + ", #Delta#phi_{j#gamma} > "s + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);

        std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
            info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

        std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
            info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

        std::function<void(int64_t, float)> range_info = [&](int64_t x, float pos) {
            info_text(x, pos, "Cent. %i - %i%%", drange, true); };

        auto pthf_info = [&](int64_t index) {
            stack_text(index, 0.75, 0.04, nevt, pt_info, hf_info); };

        auto hb = new pencil();
        hb->category("system", "PbPb", "pp");
        hb->category("axis", "na", "wta", "es");

        hb->alias("na", "");
        hb->alias("es", "E-Scheme");
        hb->alias("wta", "WTA");

        hb->ditto("es", "na");

        auto suffixes = { "d_pthf"s, "d_pt"s, "d_hf"s, "merge"s };
        auto texts = std::vector<std::function<void(int64_t)>> {
            pthf_info, std::bind(pt_info, _1, 0.75), std::bind(hf_info, _1, 0.75), std::bind(range_info, _1, 0.75) };

        std::vector<paper*> c1(4, nullptr);
        std::vector<paper*> c2(4, nullptr);
        std::vector<paper*> c3(4, nullptr);
        std::vector<paper*> c4(4, nullptr);
        std::vector<paper*> c5(4, nullptr);

        zip([&](paper*& c, int64_t rows, std::string const& suffix,
                std::function<void(int64_t)> text) {
            c = new paper(set + "_" + tag + "_dr_" + suffix, hb);
            c->divide(-1, rows);
            c->accessory(text);

            apply_style(c, cms, system_tag, -2., 27.);
            c->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));
        }, c1, x{ ihf->size(), 1L, 1L, 1L }, suffixes, texts);

        pjet_f_dr->apply([&](TH1* h) { c1[0]->add(h, system); });
        pjet_f_dr_d_pt->apply([&](TH1* h) { c1[1]->add(h, system); });
        pjet_f_dr_d_hf->apply([&](TH1* h) { c1[2]->add(h, system); });
        pjet_f_dr_merge->apply([&](TH1* h) { c1[3]->add(h, system); });

        zip([&](paper*& c, int64_t rows, std::string const& suffix,
                std::function<void(int64_t)> text) {
            c = new paper(set + "_" + tag + "_jpt_" + suffix, hb);
            c->divide(-1, rows);
            c->accessory(text);

            apply_style(c, cms, system_tag, -0.001, 0.02);
            c->accessory(std::bind(line_at, _1, 0.f, rjpt[0], rjpt[1]));
        }, c2, x{ ihf->size(), 1L, 1L, 1L }, suffixes, texts);

        pjet_f_jpt->apply([&](TH1* h) { c2[0]->add(h, system); });
        pjet_f_jpt_d_pt->apply([&](TH1* h) { c2[1]->add(h, system); });
        pjet_f_jpt_d_hf->apply([&](TH1* h) { c2[2]->add(h, system); });
        pjet_f_jpt_merge->apply([&](TH1* h) { c2[3]->add(h, system); });

        zip([&](paper*& c, int64_t rows, std::string const& suffix,
                std::function<void(int64_t)> text) {
            c = new paper(set + "_" + tag + "_u_dr_" + suffix, hb);
            c->divide(-1, rows);
            c->accessory(text);

            apply_style(c, cms, system_tag, -0.01, 0.07);
            c->accessory(std::bind(line_at, _1, 0.f, 0, mr->size()));
        }, c3, x{ ihf->size(), 1L, 1L, 1L }, suffixes, texts);

        pjet_u_dr_jpt->apply([&](TH1* h) { c3[0]->add(h, system); });
        pjet_u_dr_jpt_d_pt->apply([&](TH1* h) { c3[1]->add(h, system); });
        pjet_u_dr_jpt_d_hf->apply([&](TH1* h) { c3[2]->add(h, system); });
        pjet_u_dr_jpt_merge->apply([&](TH1* h) { c3[3]->add(h, system); });

        zip([&](paper*& c, int64_t rows, std::string const& suffix,
                std::function<void(int64_t)> text) {
            c = new paper(set + "_" + tag + "_fold_dr_" + suffix, hb);
            c->divide(-1, rows);
            c->accessory(text);

            apply_style(c, cms, system_tag, -2., 27.);
            c->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));
        }, c4, x{ ihf->size(), 1L, 1L, 1L }, suffixes, texts);

        pjet_u_dr->apply([&](TH1* h) { c4[0]->add(h, system); });
        pjet_u_dr_d_pt->apply([&](TH1* h) { c4[1]->add(h, system); });
        pjet_u_dr_d_hf->apply([&](TH1* h) { c4[2]->add(h, system); });
        pjet_u_dr_merge->apply([&](TH1* h) { c4[3]->add(h, system); });

        zip([&](paper*& c, int64_t rows, std::string const& suffix,
                std::function<void(int64_t)> text) {
            c = new paper(set + "_" + tag + "_fold_jpt_" + suffix, hb);
            c->divide(-1, rows);
            c->accessory(text);

            apply_style(c, cms, system_tag, -0.001, 0.02);
            c->accessory(std::bind(line_at, _1, 0.f, rjpt[0], rjpt[1]));
        }, c5, x{ ihf->size(), 1L, 1L, 1L }, suffixes, texts);

        pjet_u_jpt->apply([&](TH1* h) { c5[0]->add(h, system); });
        pjet_u_jpt_d_pt->apply([&](TH1* h) { c5[1]->add(h, system); });
        pjet_u_jpt_d_hf->apply([&](TH1* h) { c5[2]->add(h, system); });
        pjet_u_jpt_merge->apply([&](TH1* h) { c5[3]->add(h, system); });

        hb->set_binary("system");
        hb->sketch();

        for (auto const& c : { c1, c2, c3, c4, c5 })
            for (auto p : c) { p->draw("pdf"); }
    }

    for (int i = 0; i < 4; ++i) {
        std::cout << i << std::endl;
        for (size_t j = 0; j < rptr.size() - 1; ++j) {
            double jet_pt = (rptr[j] + rptr[j + 1]) / 2;
            double x_average = 0;
            double normalization = 0;

            for (int k = 0; k < (int) dpt.size() - 1; ++k) {
                double photon_pt = (dpt[k] + dpt[k + 1]) / 2;
                double x = jet_pt / photon_pt;
                int index = mpthf->index_for(x{k, i});
                x_average += x * pjet_f_jpt[index]->GetBinContent(j + 1);
                normalization += pjet_f_jpt[index]->GetBinContent(j + 1);
            }

            x_average /= normalization;
            std::cout << x_average << " ";
        }

        std::cout << std::endl;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return accumulate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
