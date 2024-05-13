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
    auto reta = conf->get<std::vector<float>>("eta_range");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto deta = conf->get<std::vector<float>>("eta_diff");
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
    auto ideta = new interval(deta);
    auto idhf = new interval(dhf);

    auto plot_size = idhf->size() > 1 ? idhf->size()/2 : 1;
    auto hf_shape = x{ idhf->size() };
    auto pthf_shape = x{ idpt->size(), idhf->size() };
    auto etahf_shape = x{ ideta->size(), idhf->size() };

    auto incl = new interval(""s, 1, 0., 1.);
    auto ipt = new interval("jet p_{T}"s, rpt);
    auto ieta = new interval("jet #eta"s, reta);

    auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    auto fpt = std::bind(&interval::book<TH1F>, ipt, _1, _2, _3);
    auto feta = std::bind(&interval::book<TH1F>, ieta, _1, _2, _3);

    auto title = "#sigma("s + label + ")";

    /* fully differential (pt, eta, hf) */
    auto s = new history<TH1F>("s"s, "", fincl, obj->shape());
    auto r = new history<TH1F>("r"s, "", fincl, obj->shape());

    auto s_f_pt = new history<TH1F>("s_f_pt"s, label.data(), fpt, etahf_shape);
    auto r_f_pt = new history<TH1F>("r_f_pt"s, title.data(), fpt, etahf_shape);

    /* differential in pt, hf */
    auto obj_dpthf = obj->sum(1);

    auto s_dpthf = new history<TH1F>("s_dpthf", "", fincl, pthf_shape);
    auto r_dpthf = new history<TH1F>("r_dpthf", "", fincl, pthf_shape);

    auto s_dhf_f_pt = new history<TH1F>("s_dhf_f_pt"s,
        label.data(), fpt, hf_shape);
    auto r_dhf_f_pt = new history<TH1F>("r_dhf_f_pt"s,
        title.data(), fpt, hf_shape);

    /* differential in eta, hf */
    auto resize = x{ idpt->size() - 1, ideta->size(), idhf->size() };
    auto obj_detahf = obj->shrink("valid", resize, remove)->sum(0);

    auto s_detahf = new history<TH1F>("s_detahf", "", fincl, etahf_shape);
    auto r_detahf = new history<TH1F>("r_detahf", "", fincl, etahf_shape);

    auto s_dhf_f_eta = new history<TH1F>("s_dhf_f_eta"s,
        label.data(), feta, hf_shape);
    auto r_dhf_f_eta = new history<TH1F>("r_dhf_f_eta"s,
        title.data(), feta, hf_shape);

    /* load fitting parameters */
    auto fl = new std::vector<float>*[idhf->size()];
    auto fh = new std::vector<float>*[idhf->size()];

    auto flp = new std::vector<float>[idhf->size()];
    auto fhp = new std::vector<float>[idhf->size()];
    auto fle = new std::vector<float>[idhf->size()];
    auto fhe = new std::vector<float>[idhf->size()];

    for (int64_t i = 0; i < idhf->size(); ++i) {
        auto hf_str = std::to_string(i);

        fl[i] = new std::vector<float>[ideta->size()];
        fh[i] = new std::vector<float>[ideta->size()];

        for (int64_t j = 0; j < ideta->size(); ++j) {
            auto eta_str = std::to_string(j);
            fl[i][j] = conf->get<std::vector<float>>(
                "fl_"s + hf_str + "_"s + eta_str);
            fh[i][j] = conf->get<std::vector<float>>(
                "fh_"s + hf_str + "_"s + eta_str);
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
        else if (!heavyion || csn.empty()) {
            f->SetParameters(0.085, 0.6, 0.);
        } else {
            f->SetParameters(csn[0], csn[1], csn[2]);
        }

        return f;
    };

    /* info text */
    auto system_tag = system + "  #sqrt{s_{NN}} = 5.02 TeV"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Simulation}}"s;
    if (heavyion) cms += "                  anti-k_{T} R = 0.3, p_{T}^{jet} > 15 GeV, |#eta^{jet}| < 1.6";

    std::function<void(int64_t, float)> eta_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.1f < #eta < %.1f", deta, false); };

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

    auto etahf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, obj_detahf, eta_info, hf_info); };

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
std::cout << __LINE__ << std::endl; std::cout << flp[hf_x][pt_x] << " " << fhp[hf_x][pt_x] << std::endl; h->Print("all");
        auto label = "f_obj_dpthf_"s + std::to_string(index);
        TF1* f = new TF1(label.data(), pdf.data());
        mold(f, value); std::cout << f->Eval(0) << std::endl; std::cout << __LINE__ << std::endl;
        h->Fit(label.data(), "WLMQ", "", flp[hf_x][pt_x], fhp[hf_x][pt_x]);
std::cout << __LINE__ << std::endl;
        (*s_dpthf)[index]->SetBinContent(1, f->GetParameter(1));
        (*s_dpthf)[index]->SetBinError(1, f->GetParError(1));
        (*r_dpthf)[index]->SetBinContent(1, f->GetParameter(2));
        (*r_dpthf)[index]->SetBinError(1, f->GetParError(2));
std::cout << __LINE__ << std::endl;
        ++pt_x;
std::cout << __LINE__ << std::endl;
        (*s_dhf_f_pt)[hf_x]->SetBinContent(pt_x, f->GetParameter(1));
        (*s_dhf_f_pt)[hf_x]->SetBinError(pt_x, f->GetParError(1));
        (*r_dhf_f_pt)[hf_x]->SetBinContent(pt_x, f->GetParameter(2));
        (*r_dhf_f_pt)[hf_x]->SetBinError(pt_x, f->GetParError(2));
std::cout << __LINE__ << std::endl;
        c1->add(h, "mc");
    });
std::cout << __LINE__ << std::endl;
    auto c2 = new paper(tag_object + "_dhf_f_pt_s", hb);
    apply_style(c2, cms, system_tag);
    if (!heavyion) c2->accessory(std::bind(kinematics, _1, 0.79));
    c2->accessory(std::bind(hf_info, _1, 0.75));
    c2->accessory(guide_lines);
    c2->divide(plot_size, -1);
    c2->set(paper::flags::logx);

    s_dhf_f_pt->apply([&](TH1* h, int64_t index) {
        h->SetAxisRange(s_range[0], s_range[1], "Y");
std::cout << __LINE__ << std::endl;
        if (fit) {
            auto label = "f_s_dhf_f_pt_"s + std::to_string(index);
            TF1* f = new TF1(label.data(), func.data());
            h->Fit(f, "MEQ", "", 20, rpt.back());
        }
std::cout << __LINE__ << std::endl;
        c2->add(h, "mc");
    });
std::cout << __LINE__ << std::endl;
    auto c3 = new paper(tag_object + "_dhf_f_pt_r", hb);
    apply_style(c3, cms, system_tag);
    if (!heavyion) c3->accessory(std::bind(kinematics, _1, 0.79));
    c3->accessory(std::bind(hf_info, _1, 0.75));
    c3->divide(plot_size, -1);
    c3->set(paper::flags::logx);

    r_dhf_f_pt->apply([&](TH1* h, int64_t index) {
        h->SetAxisRange(r_range[0], r_range[1], "Y");
std::cout << __LINE__ << std::endl;
        auto label = "f_r_dhf_f_pt_"s + std::to_string(index);
        auto f = resolution_function(label.data());
        h->Fit(label.data(), "MEQ", "", 20, rpt.back());
std::cout << __LINE__ << std::endl;
        csn[0] = f->GetParameter(0);
        csn[1] = f->GetParameter(1);
        csn[2] = f->GetParameter(2);
std::cout << __LINE__ << std::endl;
        printf("%i - %i%%: %.3f, %.3f, %.3f\n",
            dcent[index + 1], dcent[index], csn[0], csn[1], csn[2]);

        c3->add(h, "mc");
    });
std::cout << __LINE__ << std::endl;
    auto c4 = new paper(tag_object + "_detahf_sr_fits", hb);
    apply_style(c4, cms, system_tag);
    c4->accessory(etahf_info);
    c4->divide(plot_size, -1);

    /* fit mean and resolution */
    obj_detahf->apply([&](TH1* h, int64_t index) {
        auto indices = obj_detahf->indices_for(index);
        auto eta_x = indices[0];
        auto hf_x = indices[1];
std::cout << __LINE__ << std::endl;
        auto label = "f_obj_detahf_"s + std::to_string(index);
        TF1* f = new TF1(label.data(), pdf.data());
        mold(f, value);
        h->Fit(label.data(), "WLMQ", "", fle[hf_x][eta_x], fhe[hf_x][eta_x]);
std::cout << __LINE__ << std::endl;
        (*s_detahf)[index]->SetBinContent(1, f->GetParameter(1));
        (*s_detahf)[index]->SetBinError(1, f->GetParError(1));
        (*r_detahf)[index]->SetBinContent(1, f->GetParameter(2));
        (*r_detahf)[index]->SetBinError(1, f->GetParError(2));

        ++eta_x;
std::cout << __LINE__ << std::endl;
        (*s_dhf_f_eta)[hf_x]->SetBinContent(eta_x, f->GetParameter(1));
        (*s_dhf_f_eta)[hf_x]->SetBinError(eta_x, f->GetParError(1));
        (*r_dhf_f_eta)[hf_x]->SetBinContent(eta_x, f->GetParameter(2));
        (*r_dhf_f_eta)[hf_x]->SetBinError(eta_x, f->GetParError(2));

        c4->add(h, "mc");
    });
std::cout << __LINE__ << std::endl;
    auto c5 = new paper(tag_object + "_dhf_f_eta_s", hb);
    apply_style(c5, cms, system_tag);
    if (!heavyion) c5->accessory(std::bind(kinematics, _1, 0.79));
    c5->accessory(std::bind(hf_info, _1, 0.75));
    c5->divide(plot_size, -1);

    s_dhf_f_eta->apply([&](TH1* h) {
        h->SetAxisRange(s_range[0], s_range[1], "Y");
        c5->add(h, "mc"); });
std::cout << __LINE__ << std::endl;
    auto c6 = new paper(tag_object + "_dhf_f_eta_r", hb);
    apply_style(c6, cms, system_tag);
    if (!heavyion) c6->accessory(std::bind(kinematics, _1, 0.79));
    c6->accessory(std::bind(hf_info, _1, 0.75));
    c6->divide(plot_size, -1);

    r_dhf_f_eta->apply([&](TH1* h) {
        h->SetAxisRange(r_range[0], r_range[1], "Y");
        c6->add(h, "mc"); });
std::cout << __LINE__ << std::endl;
    auto c7 = std::vector<paper*>(ideta->size());
    auto c8 = std::vector<paper*>(ideta->size());
    auto c9 = std::vector<paper*>(ideta->size());
std::cout << __LINE__ << std::endl;
    for (int64_t i = 0; i < ideta->size(); ++i) {
        c7[i] = new paper(tag_object + "_sr_fits_s" + std::to_string(i), hb);
        apply_style(c7[i], cms, system_tag);
        if (!heavyion) c7[i]->accessory(std::bind(kinematics, _1, 0.79));
        c7[i]->accessory(pthf_info);
        c7[i]->ornaments(std::bind(eta_info, i + 1, 0.67));
        c7[i]->divide(plot_size, -1);

        c8[i] = new paper(tag_object + "_f_pt_s_s" + std::to_string(i), hb);
        apply_style(c8[i], cms, system_tag);
        if (!heavyion) c8[i]->accessory(std::bind(kinematics, _1, 0.79));
        c8[i]->accessory(std::bind(hf_info, _1, 0.75));
        c8[i]->accessory(guide_lines);
        c8[i]->ornaments(std::bind(eta_info, i + 1, 0.71));
        c8[i]->divide(plot_size, -1);
        c8[i]->set(paper::flags::logx);

        c9[i] = new paper(tag_object + "_f_pt_r_s" + std::to_string(i), hb);
        apply_style(c9[i], cms, system_tag);
        if (!heavyion) c9[i]->accessory(std::bind(kinematics, _1, 0.79));
        c9[i]->accessory(std::bind(hf_info, _1, 0.75));
        c9[i]->ornaments(std::bind(eta_info, i + 1, 0.71));
        c9[i]->divide(plot_size, -1);
        c9[i]->set(paper::flags::logx);
    }
std::cout << __LINE__ << std::endl;
    /* fit mean and resolution */
    obj->apply([&](TH1* h, int64_t index) {
        auto indices = obj->indices_for(index);
        auto pt_x = indices[0];
        auto eta_x = indices[1];
        auto hf_x = indices[2];
std::cout << __LINE__ << std::endl;
        auto label = "f_obj_"s + std::to_string(index);
        TF1* f = new TF1(label.data(), pdf.data());
        mold(f, value);
        h->Fit(label.data(), "WLMQ", "",
            fl[hf_x][eta_x][pt_x], fh[hf_x][eta_x][pt_x]);
std::cout << __LINE__ << std::endl;
        (*s)[index]->SetBinContent(1, f->GetParameter(1));
        (*s)[index]->SetBinError(1, f->GetParError(1));
        (*r)[index]->SetBinContent(1, f->GetParameter(2));
        (*r)[index]->SetBinError(1, f->GetParError(2));
std::cout << __LINE__ << std::endl;
        ++pt_x;
std::cout << __LINE__ << std::endl;
        (*s_f_pt)[x{eta_x, hf_x}]->SetBinContent(pt_x, f->GetParameter(1));
        (*s_f_pt)[x{eta_x, hf_x}]->SetBinError(pt_x, f->GetParError(1));
        (*r_f_pt)[x{eta_x, hf_x}]->SetBinContent(pt_x, f->GetParameter(2));
        (*r_f_pt)[x{eta_x, hf_x}]->SetBinError(pt_x, f->GetParError(2));
std::cout << __LINE__ << std::endl;
        c7[eta_x]->add(h, "mc");
    });
std::cout << __LINE__ << std::endl;
    s_f_pt->apply([&](TH1* h, int64_t index) {
        h->SetAxisRange(s_range[0], s_range[1], "Y");
std::cout << __LINE__ << std::endl;
        if (fit) {
            auto label = "f_s_f_pt_"s + std::to_string(index);
            TF1* f = new TF1(label.data(), func.data());
            h->Fit(f, "MEQ", "", 20, rpt.back());
        }
std::cout << __LINE__ << std::endl;
        auto eta_x = s_f_pt->indices_for(index)[0];
        c8[eta_x]->add(h, "mc");
    });
std::cout << __LINE__ << std::endl;
    r_f_pt->apply([&](TH1* h, int64_t index) {
        h->SetAxisRange(r_range[0], r_range[1], "Y");
std::cout << __LINE__ << std::endl;
        auto label = "f_r_f_pt_"s + std::to_string(index);
        auto f = resolution_function(label.data());
        h->Fit(label.data(), "MEQ", "", 20, rpt.back());
std::cout << __LINE__ << std::endl;
        csn[1] = f->GetParameter(1);
        csn[2] = f->GetParameter(2);

        auto eta_x = r_f_pt->indices_for(index)[0];
        c9[eta_x]->add(h, "mc");
    });

    hb->sketch();
std::cout << __LINE__ << std::endl;
    for (auto const& p : { c1, c2, c3, c4, c5, c6 })
        p->draw("pdf");
    for (auto const& c : { c7, c8, c9 })
        for (auto p : c)
            p->draw("pdf");
std::cout << __LINE__ << std::endl;
    /* save output */
    in(output, [&]() {
        obj_dpthf->save(tag_object);
        obj_detahf->save(tag_object);

        s->save(tag_object);
        r->save(tag_object);
        s_f_pt->save(tag_object);
        r_f_pt->save(tag_object);

        s_dpthf->save(tag_object);
        r_dpthf->save(tag_object);
        s_dhf_f_pt->save(tag_object);
        r_dhf_f_pt->save(tag_object);

        s_detahf->save(tag_object);
        r_detahf->save(tag_object);
        s_dhf_f_eta->save(tag_object);
        r_dhf_f_eta->save(tag_object);
    });
std::cout << __LINE__ << std::endl;
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return distillate(argv[1], argv[2]);

    return 0;
}
