#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/train.h"
#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TLine.h"
#include "TTree.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int regression_checks(char const* config) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto max_entries = conf->get<int64_t>("max_entries");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto heavyion = conf->get<bool>("heavyion");

    auto const eta_abs = conf->get<float>("eta_abs");
    auto const hovere_max = conf->get<float>("hovere_max");
    auto const see_min = conf->get<float>("see_min");
    auto const see_max = conf->get<float>("see_max");
    auto const iso_max = conf->get<float>("iso_max");
    auto geniso_max = conf->get<float>("geniso_max");

    /* load forest */
    TFile* f = new TFile(input.data(), "read");
    TTree* t = (TTree*)f->Get("pj");
    auto p = new pjtree(true, true, false, t, { 1, 1, 1, 1, 0, 1, 0 });

    auto hscale = new TH1F("photon_energy_scale","Photon Energy Scale",50,0,10);
    auto hscale_cor = new TH1F("photon_energy_scale_cor","Corrected Photon Energy Scale",50,0,10);

    /* iterate */
    auto nentries = static_cast<int64_t>(t->GetEntries());
    if (max_entries) nentries = std::min(nentries, max_entries);
    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 100000 == 0)
            printf("entry: %li/%li\n", i, nentries);

        t->GetEntry(i);

        if (std::abs(p->vz) > 15) { continue; }

        int64_t leading = -1;
        float leading_pt = 0;
        int64_t leading_cor = -1;
        float leading_pt_cor = 0;

        for (int64_t j = 0; j < p->nPho; ++j) {
            if ((*p->phoEt)[j] <= 25) { continue; }
            if (std::abs((*p->phoSCEta)[j]) >= eta_abs) { continue; }
            if ((*p->phoHoverE)[j] > hovere_max) { continue; }

            if ((*p->phoEt)[j] > leading_pt) {
                leading = j;
                leading_pt = (*p->phoEt)[j];
            }

            if ((*p->phoEtErNew)[j] > leading_pt_cor) {
                leading_cor = j;
                leading_pt_cor = (*p->phoEtErNew)[j];
            }
        }

        /* require leading photon */
        if (leading < 0) { continue; }

        if ((*p->phoSigmaIEtaIEta_2012)[leading] < see_max
                || (*p->phoSigmaIEtaIEta_2012)[leading] > see_min) {

            /* hem failure region exclusion */
            if (!heavyion || within_hem_failure_region(p, leading)) { 

                /* isolation requirement */
                float isolation = (*p->pho_ecalClusterIsoR3)[leading]
                    + (*p->pho_hcalRechitIsoR3)[leading]
                    + (*p->pho_trackIsoR3PtCut20)[leading];

                if (isolation < iso_max) { 

                    int64_t gen_index = (*p->pho_genMatchedIndex)[leading];

                    if (gen_index == -1) { continue; }

                    auto pid = (*p->mcPID)[gen_index];
                    auto mpid = (*p->mcMomPID)[gen_index];

                    if (pid != 22 || (std::abs(mpid) > 22 && mpid != -999)) { continue; }
                    if ((*p->mcCalIsoDR04)[gen_index] > geniso_max) { continue; }

                    auto ratio = (*p->phoEt)[leading] / (*p->mcEt)[gen_index];

                    hscale->Fill(ratio,p->weight);
                }
            }
        }

        if ((*p->phoSigmaIEtaIEta_2012)[leading_cor] < see_max
                || (*p->phoSigmaIEtaIEta_2012)[leading_cor] > see_min) {

            /* hem failure region exclusion */
            if (!heavyion || within_hem_failure_region(p, leading_cor)) { 

                /* isolation requirement */
                float isolation = (*p->pho_ecalClusterIsoR3)[leading_cor]
                    + (*p->pho_hcalRechitIsoR3)[leading_cor]
                    + (*p->pho_trackIsoR3PtCut20)[leading_cor];

                if (isolation < iso_max) { 

                    int64_t gen_index = (*p->pho_genMatchedIndex)[leading_cor];

                    if (gen_index == -1) { continue; }

                    auto pid = (*p->mcPID)[gen_index];
                    auto mpid = (*p->mcMomPID)[gen_index];

                    if (pid != 22 || (std::abs(mpid) > 22 && mpid != -999)) { continue; }
                    if ((*p->mcCalIsoDR04)[gen_index] > geniso_max) { continue; }

                    auto ratio = (*p->phoEtErNew)[leading_cor] / (*p->mcEt)[gen_index];

                    hscale_cor->Fill(ratio,p->weight);
                }
            }
        }
    }

    /* calculate efficiency */
    auto hframe = frame(hscale->GetXaxis(), hscale->GetYaxis());
    hframe->GetYaxis()->SetTitle("counts");
    hframe->GetXaxis()->SetTitle("photon energy scale");

    /* draw efficiency */
    auto hb = new pencil();
    hb->category("type", "Uncorrected", "Corrected");

    auto c1 = new paper(tag + "photon_energy_resolution", hb);
    apply_style(c1, system + " #sqrt{s} = 5.02 TeV"s);

    c1->add(hframe);
    c1->stack(hscale, "Uncorrected");
    c1->stack(hscale_cor, "Corrected");

    hb->sketch();
    c1->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 2)
        return regression_checks(argv[1]);

    return 0;
}
