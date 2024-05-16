#include "../include/lambdas.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"

#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TLatex.h"
#include "TLine.h"

#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

void mold(TF1* f, std::vector<double> const& value) {
    if (!value.empty()) { f->SetParameters(value.data()); }
}

int distillate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto object = conf->get<std::string>("object");
    auto label = conf->get<std::string>("label");
    auto pdf = conf->get<std::string>("pdf");
    auto value = conf->get<std::vector<double>>("value");

    auto heavyion = conf->get<bool>("heavyion");
    auto fit = conf->get<bool>("fit");
    auto func = conf->get<std::string>("func");

    auto rpt = conf->get<std::vector<float>>("pt_range");
    auto rdr = conf->get<std::vector<float>>("dr_range");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto ddr = conf->get<std::vector<float>>("dr_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto remove = conf->get<std::vector<int64_t>>("remove");
    auto csn = conf->get<std::vector<float>>("csn");

    auto smeared = conf->get<bool>("smeared");

    auto s_range = conf->get<std::vector<float>>("s_range");
    auto s_lines = conf->get<std::vector<float>>("s_lines");
    auto r_range = conf->get<std::vector<float>>("r_range");

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    TFile* f = new TFile(input.data(), "read");
    auto obj = new history<TH1F>(f, tag + "_" + object);

    /* prepare histograms */
    auto idpt = new interval(dpt);
    auto iddr = new interval(ddr);
    auto idhf = new interval(dhf);

    auto plot_size = idhf->size() > 1 ? idhf->size()/2 : 1;
    auto hf_shape = x{ idhf->size() };
    auto pthf_shape = x{ idpt->size(), idhf->size() };
    auto drhf_shape = x{ iddr->size(), idhf->size() };

    auto incl = new interval(""s, 1, 0., 1.);
    auto ifit = new interval(""s, 3, 0., 3.);
    auto ipt = new interval("jet p_{T}"s, rpt);
    auto idr = new interval("reco #deltaj"s, rdr);

    auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    auto fifit = std::bind(&interval::book<TH1F>, ifit, _1, _2, _3);
    auto fpt = std::bind(&interval::book<TH1F>, ipt, _1, _2, _3);
    auto fdr = std::bind(&interval::book<TH1F>, idr, _1, _2, _3);

    auto title = "#sigma("s + label + ")";

    /* fully differential (pt, dr, hf) */
    auto s = new history<TH1F>("s"s, "", fincl, obj->shape());
    auto r = new history<TH1F>("r"s, "", fincl, obj->shape());

    auto s_f_pt = new history<TH1F>("s_f_pt"s, label.data(), fpt, drhf_shape);
    auto r_f_pt = new history<TH1F>("r_f_pt"s, title.data(), fpt, drhf_shape);
    auto r_f_pt_fits = new history<TH1F>("r_f_pt_fits"s, "", fifit, drhf_shape);

    /* differential in pt, hf */
    auto obj_dpthf = obj->sum(1);

    auto s_dpthf = new history<TH1F>("s_dpthf", "", fincl, pthf_shape);
    auto r_dpthf = new history<TH1F>("r_dpthf", "", fincl, pthf_shape);

    auto s_dhf_f_pt = new history<TH1F>("s_dhf_f_pt"s,
        label.data(), fpt, hf_shape);
    auto r_dhf_f_pt = new history<TH1F>("r_dhf_f_pt"s,
        title.data(), fpt, hf_shape);

    /* differential in dr, hf */
    auto resize = x{ idpt->size() - 1, iddr->size(), idhf->size() };
    auto obj_ddrhf = obj->shrink("valid", resize, remove)->sum(0);

    auto s_ddrhf = new history<TH1F>("s_ddrhf", "", fincl, drhf_shape);
    auto r_ddrhf = new history<TH1F>("r_ddrhf", "", fincl, drhf_shape);

    auto s_dhf_f_dr = new history<TH1F>("s_dhf_f_dr"s,
        label.data(), fdr, hf_shape);
    auto r_dhf_f_dr = new history<TH1F>("r_dhf_f_dr"s,
        title.data(), fdr, hf_shape);

    /* load fitting parameters */
    auto fl = new std::vector<float>*[idhf->size()];
    auto fh = new std::vector<float>*[idhf->size()];

    auto flp = new std::vector<float>[idhf->size()];
    auto fhp = new std::vector<float>[idhf->size()];
    auto fle = new std::vector<float>[idhf->size()];
    auto fhe = new std::vector<float>[idhf->size()];

    for (int64_t i = 0; i < idhf->size(); ++i) {
        auto hf_str = std::to_string(i);

        fl[i] = new std::vector<float>[iddr->size()];
        fh[i] = new std::vector<float>[iddr->size()];

        for (int64_t j = 0; j < iddr->size(); ++j) {
            auto dr_str = std::to_string(j);
            fl[i][j] = conf->get<std::vector<float>>(
                "fl_"s + hf_str + "_"s + dr_str);
            fh[i][j] = conf->get<std::vector<float>>(
                "fh_"s + hf_str + "_"s + dr_str);
        }

        flp[i] = conf->get<std::vector<float>>("flp_"s + hf_str);
        fhp[i] = conf->get<std::vector<float>>("fhp_"s + hf_str);
        fle[i] = conf->get<std::vector<float>>("fle_"s + hf_str);
        fhe[i] = conf->get<std::vector<float>>("fhe_"s + hf_str);
    }

    auto resolution_function = [&](char const* label) {
        TF1* f = new TF1(label, "sqrt([0]*[0]+[1]*[1]/x+[2]*[2]/(x*x))");

        if (smeared) { 
            f->SetParameters(csn[0], csn[1], csn[2]);
            f->FixParameter(0, csn[0]);
            f->FixParameter(1, csn[1]);
            f->FixParameter(2, csn[2]);
        }
        else if (csn.empty()) {
            f->SetParameters(0.097, 0.6, 0.);
        }

        return f;
    };

    /* info text */
    auto system_tag = system + "  #sqrt{s_{NN}} = 5.02 TeV"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Simulation}}"s;
    if (heavyion) cms += "                  anti-k_{T} R = 0.3, p_{T}^{jet} > 15 GeV, |#eta^{jet}| < 1.6";

    std::function<void(int64_t, float)> dr_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.2f < #deltaj < %.2f", ddr, false); };

    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{jet} < %.0f", dpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    std::function<void(int64_t, float)> kinematics = [&](int64_t x, float pos) {
        char buffer[128] = { '\0' };
        sprintf(buffer, "anti-k_{T} R = 0.3, p_{T}^{jet} > 15 GeV, |#eta^{jet}| < 1.6");

        TLatex* l = new TLatex();
        l->SetTextAlign(31);
        l->SetTextFont(43);
        l->SetTextSize(13);
        l->DrawLatexNDC(0.865, pos, buffer);
        x++;
    };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, obj_dpthf, pt_info, hf_info); };

    auto drhf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, obj_ddrhf, dr_info, hf_info); };

    auto guide_lines = [&](int64_t index) {
        if (index > idhf->size()) { return; }

        for (auto val : s_lines) {
            TLine* l = new TLine(rpt.front(), val, rpt.back(), val);
            l->SetLineStyle(7);
            l->Draw();
        }
    };

    auto tag_object = tag + "_" + object;

    /* draw plots */
    auto hb = new pencil();
    hb->category("sample", "mc");

    hb->alias("mc", "AllQCDPhoton");

    auto c1 = new paper(tag_object + "_dpthf_sr_fits", hb);
    apply_style(c1, cms, system_tag);
    c1->accessory(pthf_info);
    c1->divide(plot_size, -1);

    /* fit obj and resolution */
    obj_dpthf->apply([&](TH1* h, int64_t index) {
        auto indices = obj_dpthf->indices_for(index);
        auto pt_x = indices[0];
        auto hf_x = indices[1];

        auto label = "f_obj_dpthf_"s + std::to_string(index);
        TF1* f = new TF1(label.data(), pdf.data());
        mold(f, value);
        h->Fit(label.data(), "MQ", "", flp[hf_x][pt_x], fhp[hf_x][pt_x]);

        auto value = -1/f->GetParameter(1);
        auto error = std::abs(value + 1/(f->GetParameter(1) + f->GetParError(1)));

        std::cout << value << " " << error << std::endl;

        value = h->GetMean();
        error = h->GetMeanError();

        (*s_dpthf)[index]->SetBinContent(1, 0);
        (*s_dpthf)[index]->SetBinError(1, 0);
        (*r_dpthf)[index]->SetBinContent(1, value);
        (*r_dpthf)[index]->SetBinError(1, error);

        ++pt_x;

        (*s_dhf_f_pt)[hf_x]->SetBinContent(pt_x, 0);
        (*s_dhf_f_pt)[hf_x]->SetBinError(pt_x, 0);
        (*r_dhf_f_pt)[hf_x]->SetBinContent(pt_x, value);
        (*r_dhf_f_pt)[hf_x]->SetBinError(pt_x, error);

        c1->add(h, "mc");
    });

    auto c2 = new paper(tag_object + "_dhf_f_pt_s", hb);
    apply_style(c2, cms, system_tag);
    if (!heavyion) c2->accessory(std::bind(kinematics, _1, 0.79));
    c2->accessory(std::bind(hf_info, _1, 0.75));
    c2->accessory(guide_lines);
    c2->divide(plot_size, -1);
    c2->set(paper::flags::logx);

    s_dhf_f_pt->apply([&](TH1* h, int64_t index) {
        h->SetAxisRange(s_range[0], s_range[1], "Y");

        if (fit) {
            auto label = "f_s_dhf_f_pt_"s + std::to_string(index);
            TF1* f = new TF1(label.data(), func.data());
            h->Fit(f, "MEQ", "", 20, rpt.back());
        }

        c2->add(h, "mc");
    });

    auto c3 = new paper(tag_object + "_dhf_f_pt_r", hb);
    apply_style(c3, cms, system_tag);
    if (!heavyion) c3->accessory(std::bind(kinematics, _1, 0.79));
    c3->accessory(std::bind(hf_info, _1, 0.75));
    c3->divide(plot_size, -1);
    c3->set(paper::flags::logx);

    r_dhf_f_pt->apply([&](TH1* h, int64_t index) {
        h->SetAxisRange(r_range[0], r_range[1], "Y");

        auto label = "f_r_dhf_f_pt_"s + std::to_string(index);
        auto f = resolution_function(label.data());
        h->Fit(label.data(), "MEQ", "", 20, rpt.back());

        csn[0] = f->GetParameter(0);
        csn[1] = f->GetParameter(1);
        csn[2] = f->GetParameter(2);

        printf("%i - %i%%: %.3f, %.3f, %.3f\n",
            dcent[index + 1], dcent[index], csn[0], csn[1], csn[2]);

        c3->add(h, "mc");
    });

    auto c4 = new paper(tag_object + "_ddrhf_sr_fits", hb);
    apply_style(c4, cms, system_tag);
    c4->accessory(drhf_info);
    c4->divide(plot_size, -1);

    /* fit mean and resolution */
    obj_ddrhf->apply([&](TH1* h, int64_t index) {
        auto indices = obj_ddrhf->indices_for(index);
        auto dr_x = indices[0];
        auto hf_x = indices[1];

        auto label = "f_obj_ddrhf_"s + std::to_string(index);
        TF1* f = new TF1(label.data(), pdf.data());
        mold(f, value);
        h->Fit(label.data(), "MQ", "", fle[hf_x][dr_x], fhe[hf_x][dr_x]);
        
        auto value = -1/f->GetParameter(1);
        auto error = std::abs(value + 1/(f->GetParameter(1) + f->GetParError(1)));

        std::cout << value << " " << error << std::endl;

        value = h->GetMean();
        error = h->GetMeanError();

        (*s_ddrhf)[index]->SetBinContent(1, 0);
        (*s_ddrhf)[index]->SetBinError(1, 0);
        (*r_ddrhf)[index]->SetBinContent(1, value);
        (*r_ddrhf)[index]->SetBinError(1, error);

        ++dr_x;

        (*s_dhf_f_dr)[hf_x]->SetBinContent(dr_x, 0);
        (*s_dhf_f_dr)[hf_x]->SetBinError(dr_x, 0);
        (*r_dhf_f_dr)[hf_x]->SetBinContent(dr_x, value);
        (*r_dhf_f_dr)[hf_x]->SetBinError(dr_x, error);

        c4->add(h, "mc");
    });

    auto c5 = new paper(tag_object + "_dhf_f_dr_s", hb);
    apply_style(c5, cms, system_tag);
    if (!heavyion) c5->accessory(std::bind(kinematics, _1, 0.79));
    c5->accessory(std::bind(hf_info, _1, 0.75));
    c5->divide(plot_size, -1);

    s_dhf_f_dr->apply([&](TH1* h) {
        h->SetAxisRange(s_range[0], s_range[1], "Y");
        c5->add(h, "mc"); });

    auto c6 = new paper(tag_object + "_dhf_f_dr_r", hb);
    apply_style(c6, cms, system_tag);
    if (!heavyion) c6->accessory(std::bind(kinematics, _1, 0.79));
    c6->accessory(std::bind(hf_info, _1, 0.75));
    c6->divide(plot_size, -1);

    r_dhf_f_dr->apply([&](TH1* h) {
        h->SetAxisRange(r_range[0], r_range[1], "Y");
        c6->add(h, "mc"); });

    auto c7 = std::vector<paper*>(iddr->size());
    auto c8 = std::vector<paper*>(iddr->size());
    auto c9 = std::vector<paper*>(iddr->size());

    for (int64_t i = 0; i < iddr->size(); ++i) {
        c7[i] = new paper(tag_object + "_sr_fits_s" + std::to_string(i), hb);
        apply_style(c7[i], cms, system_tag);
        if (!heavyion) c7[i]->accessory(std::bind(kinematics, _1, 0.79));
        c7[i]->accessory(pthf_info);
        c7[i]->ornaments(std::bind(dr_info, i + 1, 0.67));
        c7[i]->divide(plot_size, -1);

        c8[i] = new paper(tag_object + "_f_pt_s_s" + std::to_string(i), hb);
        apply_style(c8[i], cms, system_tag);
        if (!heavyion) c8[i]->accessory(std::bind(kinematics, _1, 0.79));
        c8[i]->accessory(std::bind(hf_info, _1, 0.75));
        c8[i]->accessory(guide_lines);
        c8[i]->ornaments(std::bind(dr_info, i + 1, 0.71));
        c8[i]->divide(plot_size, -1);
        c8[i]->set(paper::flags::logx);

        c9[i] = new paper(tag_object + "_f_pt_r_s" + std::to_string(i), hb);
        apply_style(c9[i], cms, system_tag);
        if (!heavyion) c9[i]->accessory(std::bind(kinematics, _1, 0.79));
        c9[i]->accessory(std::bind(hf_info, _1, 0.75));
        c9[i]->ornaments(std::bind(dr_info, i + 1, 0.71));
        c9[i]->divide(plot_size, -1);
        c9[i]->set(paper::flags::logx);
    }

    /* fit mean and resolution */
    obj->apply([&](TH1* h, int64_t index) {
        auto indices = obj->indices_for(index);
        auto pt_x = indices[0];
        auto dr_x = indices[1];
        auto hf_x = indices[2];

        auto label = "f_obj_"s + std::to_string(index);
        TF1* f = new TF1(label.data(), pdf.data());
        mold(f, value);
        h->Fit(label.data(), "MQ", "",
            fl[hf_x][dr_x][pt_x], fh[hf_x][dr_x][pt_x]);

        auto value = -1/f->GetParameter(1);
        auto error = std::abs(value + 1/(f->GetParameter(1) + f->GetParError(1)));

        std::cout << value << " " << error << std::endl;

        value = h->GetMean();
        error = h->GetMeanError();

        (*s)[index]->SetBinContent(1, 0);
        (*s)[index]->SetBinError(1, 0);
        (*r)[index]->SetBinContent(1, value);
        (*r)[index]->SetBinError(1, error);

        ++pt_x;

        (*s_f_pt)[x{dr_x, hf_x}]->SetBinContent(pt_x, 0);
        (*s_f_pt)[x{dr_x, hf_x}]->SetBinError(pt_x, 0);
        (*r_f_pt)[x{dr_x, hf_x}]->SetBinContent(pt_x, value);
        (*r_f_pt)[x{dr_x, hf_x}]->SetBinError(pt_x, error);

        c7[dr_x]->add(h, "mc");
    });

    s_f_pt->apply([&](TH1* h, int64_t index) {
        h->SetAxisRange(s_range[0], s_range[1], "Y");

        if (fit) {
            auto label = "f_s_f_pt_"s + std::to_string(index);
            TF1* f = new TF1(label.data(), func.data());
            h->Fit(f, "MEQ", "", 20, rpt.back());
        }

        auto dr_x = s_f_pt->indices_for(index)[0];
        c8[dr_x]->add(h, "mc");
    });

    r_f_pt->apply([&](TH1* h, int64_t index) {
        h->SetAxisRange(r_range[0], r_range[1], "Y");

        auto label = "f_r_f_pt_"s + std::to_string(index);
        auto f = resolution_function(label.data());
        h->Fit(label.data(), "MEQ", "", 20, rpt.back());

        (*r_f_pt_fits)[index]->SetBinContent(1, f->GetParameter(0));
        (*r_f_pt_fits)[index]->SetBinContent(2, f->GetParameter(1));
        (*r_f_pt_fits)[index]->SetBinContent(3, f->GetParameter(2));

        auto dr_x = r_f_pt->indices_for(index)[0];
        c9[dr_x]->add(h, "mc");
    });

    hb->sketch();

    for (auto const& p : { c1, c2, c3, c4, c5, c6 })
        p->draw("pdf");
    for (auto const& c : { c7, c8, c9 })
        for (auto p : c)
            p->draw("pdf");

    /* save output */
    in(output, [&]() {
        obj_dpthf->save(tag_object);
        obj_ddrhf->save(tag_object);

        s->save(tag_object);
        r->save(tag_object);
        s_f_pt->save(tag_object);
        r_f_pt->save(tag_object);
        r_f_pt_fits->save(tag_object);

        s_dpthf->save(tag_object);
        r_dpthf->save(tag_object);
        s_dhf_f_pt->save(tag_object);
        r_dhf_f_pt->save(tag_object);

        s_ddrhf->save(tag_object);
        r_ddrhf->save(tag_object);
        s_dhf_f_dr->save(tag_object);
        r_dhf_f_dr->save(tag_object);
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return distillate(argv[1], argv[2]);

    return 0;
}
