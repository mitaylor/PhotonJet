#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TTree.h"

#include <ctime>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename... T>
void scale(double factor, T*... args) {
    (void)(int [sizeof...(T)]) { (args->scale(factor), 0)... };
}

template <typename... T>
void scale_bin_width(T*... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1., "width"); }), 0)... };
}

float res(float c, float s, float n, float pt) {
    return std::sqrt(c*c + s*s / pt + n*n / (pt * pt));
}


int posFromFloatVect(float val, std::vector<double>* vect)
{
  if(val < (*vect)[0] || val >= (*vect)[vect->size()-1]){
    std::cout << "posFromFloatVect: val " << val << " outside of range " << (*vect)[0] << "-" << (*vect)[vect->size()-1] << ". return -1" << std::endl;
    return -1;
  }

  int min = 0;
  int max = vect->size()-1;
  int pos = (min + max)/2;
  bool valFound = val >= (*vect)[pos] && val < (*vect)[pos+1];
  while(!valFound){
    if(val < (*vect)[pos]){
      max = pos;
      pos = (min + pos)/2;
    }
    else{
      min = pos;
      pos = (max + pos)/2;
    }
    
    valFound = val >= (*vect)[pos] && val < (*vect)[pos+1];
  }

  return pos;
}

void fill_axes(pjtree* pjt, 
               std::vector<int64_t>& pthf_x, std::vector<float>& weights, 
               float pho_cor, float photon_eta, int64_t photon_phi, 
               bool exclude, 
               bool jet_cor, float jet_pt_min, float jet_eta_abs, float jet_dr_max,
               float dphi_min_numerator, float dphi_min_denominator,
               multival* mdr, interval* idphi,
               memory<TH1F>* nevt,
               memory<TH1F>* pjet_f_dr,
               memory<TH1F>* pjet_f_jpt,
               memory<TH1F>* pjet_u_dr,
               history<TH2F>* acceptance, history<TH2F>* total) {//, int entry) {
    
    zip([&](auto const& index, auto const& weight) {
        (*nevt)[index]->Fill(1., weight * pho_cor);
    }, pthf_x, weights);

    //    if(entry >= 33589 - 10) std::cout << "Filling jet axes" << std::endl;
    for (int64_t j = 0; j < pjt->nref; ++j) {
        auto jet_pt = (*pjt->jtpt)[j];
        if (jet_cor) jet_pt = (*pjt->jtptCor)[j];

        auto jet_eta = (*pjt->jteta)[j];
        if (std::abs(jet_eta) >= jet_eta_abs) { continue; }

        auto jet_phi = convert_radian((*pjt->jtphi)[j]);

        /* hem failure region exclusion */
        if (exclude && in_jet_failure_region(pjt,j)) { continue; }

        auto jet_wta_eta = (*pjt->WTAeta)[j];
        auto jet_wta_phi = convert_radian((*pjt->WTAphi)[j]);

        auto photon_jet_dphi = std::abs(photon_phi - jet_phi);

        /* require back-to-back jets */
        if (photon_jet_dphi < convert_pi(dphi_min_numerator/dphi_min_denominator)) { continue; }

        /* do acceptance weighting */
        double cor = 1;
        if (exclude) {
            auto dphi_x = idphi->index_for(revert_pi(photon_jet_dphi));
            auto bin = (*total)[dphi_x]->FindBin(jet_eta, photon_eta);
            cor = (*total)[dphi_x]->GetBinContent(bin) / (*acceptance)[dphi_x]->GetBinContent(bin);
            if (cor < 1) { std::cout << "error" << std::endl; }
        }

        double jt_deta = jet_eta - jet_wta_eta;
        double jt_dphi = revert_radian(jet_phi - jet_wta_phi);
        double jt_dr = std::sqrt(jt_deta * jt_deta + jt_dphi * jt_dphi);

	/*
	if(entry >= 33589 - 10){
	 	  std::cout << " j, pt, dr, val:" << j << ", " << jet_pt << ", " << jt_dr << ", " << mdr->index_for(v{jt_dr, jet_pt}) << std::endl;
		  }*/
	
        zip([&](auto const& index, auto const& weight) {
            if (jet_pt < 200 && jet_pt >= jet_pt_min && jt_dr < jet_dr_max) {
                (*pjet_u_dr)[index]->Fill(mdr->index_for(v{jt_dr, jet_pt}), cor * weight);
                (*pjet_f_jpt)[index]->Fill(jet_pt, cor * weight);
                (*pjet_f_dr)[index]->Fill(jt_dr, cor * weight);
            }
            else if (jet_pt < jet_pt_min) {
                (*pjet_u_dr)[index]->Fill(-1, cor * weight);
                (*pjet_f_jpt)[index]->Fill(jet_pt, cor * weight);
                (*pjet_f_dr)[index]->Fill(-1, cor * weight);
            }
            else if (jet_pt >= 200) {
                (*pjet_u_dr)[index]->Fill(10000, cor * weight);
                (*pjet_f_jpt)[index]->Fill(jet_pt, cor * weight);
                (*pjet_f_dr)[index]->Fill(10000, cor * weight);
            }
            else if (jt_dr >= jet_dr_max) {
                (*pjet_u_dr)[index]->Fill(10000, cor * weight);
                (*pjet_f_jpt)[index]->Fill(10000, cor * weight);
                (*pjet_f_dr)[index]->Fill(jt_dr, cor * weight);
            }
        }, pthf_x, weights);
    }
}


void fill_axes(std::vector<std::map<std::string, float> > pjt, 
               std::vector<int64_t>& pthf_x, std::vector<float>& weights, 
               float pho_cor, float photon_eta, int64_t photon_phi, 
               bool exclude, 
               float jet_pt_min, float jet_eta_abs, float jet_dr_max,
               float dphi_min_numerator, float dphi_min_denominator,
               multival* mdr, interval* idphi,
               memory<TH1F>* nevt,
               memory<TH1F>* pjet_f_dr,
               memory<TH1F>* pjet_f_jpt,
               memory<TH1F>* pjet_u_dr,
               history<TH2F>* acceptance, history<TH2F>* total) {//, int entry) {
    
    zip([&](auto const& index, auto const& weight) {
        (*nevt)[index]->Fill(1., weight * pho_cor);
    }, pthf_x, weights);

    for(unsigned int j = 0; j < pjt.size(); ++j){
        auto jet_pt = pjt[j]["jet_pt"];

        auto jet_eta = pjt[j]["jet_eta"];
        if (std::abs(jet_eta) >= jet_eta_abs) { continue; }

        auto jet_phi = convert_radian(pjt[j]["jet_phi"]);
        auto jet_wta_eta = pjt[j]["jet_wta_eta"];
        auto jet_wta_phi = convert_radian(pjt[j]["jet_wta_phi"]);

        auto photon_jet_dphi = std::abs(photon_phi - jet_phi);

        /* require back-to-back jets */
        if (photon_jet_dphi < convert_pi(dphi_min_numerator/dphi_min_denominator)) { continue; }

        /* do acceptance weighting */
        double cor = 1;
        if (exclude) {
            auto dphi_x = idphi->index_for(revert_pi(photon_jet_dphi));
            auto bin = (*totaal)[dphi_x]->FindBin(jet_eta, photon_eta);
            cor = (*total)[dphi_x]->GetBinContent(bin) / (*acceptance)[dphi_x]->GetBinContent(bin);
            if (cor < 1) { std::cout << "error" << std::endl; }
        }

        double jt_deta = jet_eta - jet_wta_eta;
        double jt_dphi = revert_radian(jet_phi - jet_wta_phi);
        double jt_dr = std::sqrt(jt_deta * jt_deta + jt_dphi * jt_dphi);

	/*
	if(entry >= 33589 - 10){
	 	  std::cout << " j, pt, dr, val:" << j << ", " << jet_pt << ", " << jt_dr << ", " << mdr->index_for(v{jt_dr, jet_pt}) << std::endl;
		  }*/
	
        zip([&](auto const& index, auto const& weight) {
            if (jet_pt < 200 && jet_pt >= jet_pt_min && jt_dr < jet_dr_max) {
                (*pjet_u_dr)[index]->Fill(mdr->index_for(v{jt_dr, jet_pt}), cor * weight);
                (*pjet_f_jpt)[index]->Fill(jet_pt, cor * weight);
                (*pjet_f_dr)[index]->Fill(jt_dr, cor * weight);
            }
            else if (jet_pt < jet_pt_min) {
                (*pjet_u_dr)[index]->Fill(-1, cor * weight);
                (*pjet_f_jpt)[index]->Fill(jet_pt, cor * weight);
                (*pjet_f_dr)[index]->Fill(-1, cor * weight);
            }
            else if (jet_pt >= 200) {
                (*pjet_u_dr)[index]->Fill(10000, cor * weight);
                (*pjet_f_jpt)[index]->Fill(jet_pt, cor * weight);
                (*pjet_f_dr)[index]->Fill(10000, cor * weight);
            }
            else if (jt_dr >= jet_dr_max) {
                (*pjet_u_dr)[index]->Fill(10000, cor * weight);
                (*pjet_f_jpt)[index]->Fill(10000, cor * weight);
                (*pjet_f_dr)[index]->Fill(jt_dr, cor * weight);
            }
        }, pthf_x, weights);
    }
}


int populate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::vector<std::string>>("input");
    auto mb = conf->get<std::vector<std::string>>("mb");

    auto acc_file = conf->get<std::string>("acc_file");
    auto acc_label_ref = conf->get<std::string>("acc_label_ref");
    auto acc_label_acc = conf->get<std::string>("acc_label_acc");

    auto eff_file = conf->get<std::string>("eff_file");
    auto eff_label = conf->get<std::string>("eff_label");

    auto rho_file = conf->get<std::string>("rho_file");
    auto rho_label = conf->get<std::string>("rho_label");

    auto modulo = conf->get<int64_t>("modulo");
    auto parity = conf->get<bool>("parity");

    auto mix = conf->get<int64_t>("mix");
    auto frequency = conf->get<int64_t>("frequency");
    auto tag = conf->get<std::string>("tag");

    /* options */
    auto gen_iso = conf->get<bool>("generator_isolation");
    //    auto ele_rej = conf->get<bool>("electron_rejection");
    auto exclude = conf->get<bool>("exclude");
    //    auto apply_er = conf->get<bool>("apply_er");
    auto no_jes = conf->get<bool>("no_jes");
    auto apply_es = conf->get<bool>("apply_es");

    auto dhf = conf->get<std::vector<float>>("hf_diff");

    /* selections */
    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto heavyion = sel->get<bool>("heavyion");

    //    auto const photon_pt_min = sel->get<float>("photon_pt_min");
    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");
    //    auto const hovere_max = sel->get<float>("hovere_max");
    //    auto see_min = sel->get<float>("see_min");
    //    auto see_max = sel->get<float>("see_max");
    //    auto const iso_max = sel->get<float>("iso_max");
    //    auto const gen_iso_max = sel->get<float>("gen_iso_max");

    auto const jet_pt_min = sel->get<float>("jet_pt_min");
    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");
    auto const jet_dr_max = sel->get<float>("jet_dr_max");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto rjpt = sel->get<std::vector<float>>("jpt_range");
    auto rdphi = sel->get<std::vector<float>>("dphi_range"); // used for the acceptance weighting
    auto rdr = sel->get<std::vector<float>>("dr_range"); // used for the not-unfolded histogram

    auto rdrr = sel->get<std::vector<float>>("drr_range");
    auto rptr = sel->get<std::vector<float>>("ptr_range");

    auto dpt = sel->get<std::vector<float>>("photon_pt_diff");

    auto photon_pt_es = sel->get<std::vector<float>>("photon_pt_es");

    auto alter_base = conf->get<std::string>("alter_base"); // get offset to paths from the original configuration file

    /* fix sigma eta eta range for background distributions */
    //    if (tag == "bkg") see_min = 0.012;
    //    if (tag == "bkg") see_max = 0.02;

    /* make histograms */
    auto ipt = new interval(dpt);
    auto ihf = new interval(dhf);

    auto mpthf = new multival(dpt, dhf);

    auto incl = new interval(""s, 1, 0.f, 9999.f);
    auto idphi = new interval("#Delta#phi^{#gammaj}"s, rdphi);
    auto idr = new interval("#deltaj"s, rdr);
    auto ijpt = new interval("p_{T}^{j}"s, rjpt);

    auto mdr = new multival(rdrr, rptr);

    auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    auto fdr = std::bind(&interval::book<TH1F>, idr, _1, _2, _3);
    auto fjpt = std::bind(&interval::book<TH1F>, ijpt, _1, _2, _3);

    auto frdr = [&](int64_t, std::string const& name, std::string const&) {
        return new TH1F(name.data(), ";index;", mdr->size(), 0, mdr->size()); };

    auto nevt = new memory<TH1F>("nevt"s, "", fincl, mpthf);
    auto nmix = new memory<TH1F>("nmix"s, "", fincl, mpthf);

    auto pjet_f_dr = new memory<TH1F>("pjet_f_dr"s,
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthf);
    auto pjet_f_jpt = new memory<TH1F>("pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf);
    auto pjet_u_dr = new memory<TH1F>("pjet_u_dr"s, "", frdr, mpthf);

    auto mix_pjet_f_dr = new memory<TH1F>("mix_pjet_f_dr",
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthf);
    auto mix_pjet_f_jpt = new memory<TH1F>("mix_pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf);
    auto mix_pjet_u_dr = new memory<TH1F>("mix_pjet_u_dr"s, "", frdr, mpthf);

    /* random number for smearing and mb selection */
    auto rng = new TRandom3(144);
    auto rngPho = new TRandom3(385);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    int index_m = rng->Integer(mb.size());
    TFile* fm = new TFile(mb[index_m].data(), "read");
    TTree* tm = (TTree*)fm->Get("pj");
    auto pjtm = new pjtree(gen_iso, false, heavyion, tm, { 1, 1, 1, 1, 1, 0, heavyion, 0, 0 });
    int64_t mentries = static_cast<int64_t>(tm->GetEntries()); std::cout << mentries << std::endl;


    
    //CM 2023.12.19
    //Build a mixed event map
    //    std::cout << "HF Bounds?: " << dhf.front() << "-" << dhf.back() << std::endl;
    //Create a binning of +10% intervals
    
    //Bin construction originally using dhf.front() as min and dhf.back() as max
    double hfBinMin = dhf.front();
    double hfBinMax = dhf.back();
    //Hard-coding now because range is greater than back/front
    double hfBinMinUnderflow = 9.6;
    double hfBinMaxOverflow = 6450;

    const Double_t binFactor = 1.1;
    std::vector<double> hfBins = {hfBinMinUnderflow, hfBinMin};
    while(hfBins[hfBins.size()-1] < hfBinMax){
      hfBins.push_back(hfBins[hfBins.size()-1]*binFactor);
    }
    
    //Check which of the last two bins is close to dhf.back
    //if its second to last, erase and replace, else just keep the enlarged last bin    
    if(hfBinMax - hfBins[hfBins.size()-2] < hfBins[hfBins.size()-1] - hfBinMax){
      hfBins.erase(hfBins.begin() + hfBins.size()-1);
      hfBins[hfBins.size()-1] = hfBinMax;
    }
    hfBins.push_back(hfBinMaxOverflow);

    //Build our map
    //Vector (events), Vector (jets / event), map (strings -> jet kin)
    std::map<ULong64_t, std::vector<std::vector<std::map<std::string, float> > > > mapHFToJets;
    std::map<ULong64_t, ULong64_t> mapHFCounter;
    for(unsigned int hfI = 0; hfI < hfBins.size()-1; ++hfI){
      mapHFToJets[hfI] = {};
      mapHFCounter[hfI] = 0;
    }

    bool jet_cor = heavyion && !no_jes;
    std::cout << "Building map from mixed event TTree..." << std::endl;
    std::cout << " RhoFile: " << rho_file.empty() << std::endl;
    ULong64_t nMixEntries = tm->GetEntries();

    double minHF = 1000000.0;
    double maxHF = -1.0;

    for(unsigned int entry = 0; entry < nMixEntries; ++entry){
      tm->GetEntry(entry);

      if (std::abs(pjtm->vz) > 15) { continue; }

      //Get hf pos
      float hf = pjtm->hiHF;

      if(hf < minHF) minHF = hf;
      if(hf > maxHF) maxHF = hf;
      
      if (rho_file.empty() && hf <= dhf.front()) { continue; }
      if (rho_file.empty() && hf >= dhf.back()) { continue; }
      
      ULong64_t hfPos = posFromFloatVect(hf, &hfBins);
      
      std::vector<std::map<std::string, float> > jetKinMapVect;

      for (int64_t j = 0; j < pjtm->nref; ++j) {
	// hem failure region exclusion 
        if (exclude && in_jet_failure_region(pjtm,j)) { continue; }

        auto jet_pt = (*pjtm->jtpt)[j];
        if (jet_cor) jet_pt = (*pjtm->jtptCor)[j];	
	auto jet_eta = (*pjtm->jteta)[j];

        if (std::abs(jet_eta) >= jet_eta_abs) { continue; }

	auto jet_phi = (*pjtm->jtphi)[j];
	auto jet_wta_eta = (*pjtm->WTAeta)[j];
	auto jet_wta_phi = (*pjtm->WTAphi)[j];

	std::map<std::string, float> jetKinMap;
	
	jetKinMap["jet_pt"] = jet_pt;
	jetKinMap["jet_eta"] = jet_eta;
	jetKinMap["jet_phi"] = jet_phi;
	jetKinMap["jet_wta_eta"] = jet_wta_eta;
	jetKinMap["jet_wta_phi"] = jet_wta_phi;

	jetKinMapVect.push_back(jetKinMap);
      }
      
      mapHFToJets[hfPos].push_back(jetKinMapVect);
    }
    std::cout << "MIN HF, MAX HF: " << minHF << ", " << maxHF << std::endl;
    
    std::cout << "Map Building Complete" << std::endl;
    std::cout << "Bin: Events" << std::endl;
    for(unsigned int hI = 0; hI < hfBins.size()-1; ++hI){
      std::cout << " " << hI << ", " << hfBins[hI] << "-" << hfBins[hI+1] << ": " << mapHFToJets[hI].size() << std::endl;;
    }


    printf("iterate..\n");

    /* load efficiency correction */
    TFile* fe;
    history<TH1F>* efficiency = nullptr;

    if (!eff_file.empty()) {
        fe = new TFile((alter_base + base + eff_file).data(), "read");
        efficiency = new history<TH1F>(fe, eff_label);
    }

    /* load centrality weighting for MC */
    TFile* fr;
    history<TH1F>* rho_weighting = nullptr;

    if (!rho_file.empty()) {
        fr = new TFile((alter_base + base + rho_file).data(), "read");
        rho_weighting = new history<TH1F>(fr, rho_label);
    }

    /* load acceptance weighting for HI */
    TFile* fa;
    history<TH2F>* acceptance = nullptr;
    history<TH2F>* total = nullptr;

    if (!acc_file.empty()) {
        fa = new TFile(acc_file.data(), "read");
        acceptance = new history<TH2F>(fa, acc_label_acc);
        total = new history<TH2F>(fa, acc_label_ref);
    }

    /* add weight for the number of photons, based on the fraction that are excluded by area */
    auto pho_cor = (exclude) ? 1 / (1 - pho_failure_region_fraction(photon_eta_abs)) : 1;

    if (modulo != 1) { std::cout << "modulo: " << modulo << std::endl; }
    
    int64_t tentries = 0;
    clock_t time = clock();
    clock_t duration = 0;

    
    /* load input */
    for (auto const& file : input) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");

        auto pjt = new pjtree(gen_iso, false, heavyion, t, { 1, 1, 1, 1, 1, 0, heavyion, 0, !heavyion });
        int64_t nentries = 200000;//static_cast<int64_t>(t->GetEntries());

	//CM 2023.12.19 - comment out line w/ m
	//	for (int64_t i = 0, m = 0; i < nentries; ++i) {
	for (int64_t i = 0; i < nentries; ++i) {	
            if (i % frequency == 0) { printf("entry: %li/%li\n", i, nentries); }
            if (i % frequency == 0) { 
                if (tentries != 0) {
                    duration = clock() - time;
                    std::cout << "Time per " << frequency/modulo << " entries: " << (double)(duration)/CLOCKS_PER_SEC << " seconds" << std::endl;
                    std::cout << "Entries: " << tentries << std::endl;
                    tentries = 0;
                    time = clock();
                }
            }

            if ((i + parity) % modulo != 0) { continue; }

            t->GetEntry(i);

            double hf = pjt->hiHF;
            auto hf_x = ihf->index_for(hf);

	    //	    std::cout << "Passing parity" << std::endl;
	    
            if (rho_file.empty() && hf <= dhf.front()) { continue; }

	    //	    std::cout << "Passing hf front" << std::endl;

            if (rho_file.empty() && hf >= dhf.back()) { continue; }

	    //	    std::cout << "Passing hf back" << std::endl;

            if (std::abs(pjt->vz) > 15) { continue; }

	    //	    std::cout << "SIGNAL HF: " << pjt->hiHF << std::endl;
	    
	    //	    std::cout << "Passing hf vz" << std::endl;
	    
            auto photon_es = (apply_es) ? photon_pt_es[hf_x] : 1;

            int64_t leading = 0;
            float leading_pt = rngPho->Uniform(60.0, 350.0);

	    /*
	    for (int64_t j = 0; j < pjt->nPho; ++j) {
                if ((*pjt->phoEt)[j] <= 30) { continue; }
                if (std::abs((*pjt->phoSCEta)[j]) >= photon_eta_abs) { continue; }
                if ((*pjt->phoHoverE)[j] > hovere_max) { continue; }

                auto pho_et = (*pjt->phoEt)[j];
                if (heavyion && apply_er) pho_et = (*pjt->phoEtErNew)[j];
                if (!heavyion && apply_er) pho_et = (*pjt->phoEtEr)[j];

                pho_et *= photon_es;

                if (pho_et < photon_pt_min) { continue; }

                if (pho_et > leading_pt) {
                    leading = j;
                    leading_pt = pho_et;
                }
            }
	    */
	    
            /* require leading photon */
            if (leading < 0) { continue; }

	    /*
            if ((*pjt->phoSigmaIEtaIEta_2012)[leading] > see_max
                    || (*pjt->phoSigmaIEtaIEta_2012)[leading] < see_min)
                continue;
	    */
            /* hem failure region exclusion */
	    //            if (exclude && in_pho_failure_region(pjt, leading)) { continue; }

            /* isolation requirement */
	    /*
            if (gen_iso) {
                auto gen_index = (*pjt->pho_genMatchedIndex)[leading];
                if (gen_index == -1) { continue; }

                float isolation = (*pjt->mcCalIsoDR04)[gen_index];
                if (isolation > gen_iso_max) { continue; }
            } else {
                float isolation = (*pjt->pho_ecalClusterIsoR3)[leading]
                    + (*pjt->pho_hcalRechitIsoR3)[leading]
                    + (*pjt->pho_trackIsoR3PtCut20)[leading];
                if (isolation > iso_max) { continue; }
            }
	    */
	    
            /* leading photon axis */
	    //            auto photon_eta = (*pjt->phoEta)[leading];
	    //            auto photon_phi = convert_radian((*pjt->phoPhi)[leading]);
            auto photon_eta = rngPho->Uniform(-1.44, 1.44); //(*pjt->phoEta)[leading];
            auto photon_phi = rngPho->Uniform(-TMath::Pi(), TMath::Pi()); //convert_radian((*pjt->phoPhi)[leading]);

            /* electron rejection */
	    /*
            if (ele_rej) {
                bool electron = false;
                for (int64_t j = 0; j < pjt->nEle; ++j) {
                    if (std::abs((*pjt->eleSCEta)[j]) > 1.4442) { continue; }

                    auto deta = photon_eta - (*pjt->eleEta)[j];
                    if (deta > 0.1) { continue; }

                    auto ele_phi = convert_radian((*pjt->elePhi)[j]);
                    auto dphi = revert_radian(photon_phi - ele_phi);
                    auto dr2 = deta * deta + dphi * dphi;

                    if (dr2 < 0.01 && passes_electron_id<
                                det::barrel, wp::loose, pjtree
                            >(pjt, j, heavyion)) {
                        electron = true; break; }
                }

                if (electron) { continue; }
            }
	    */
	    
            auto pt_x = ipt->index_for(leading_pt);

            std::vector<int64_t> pthf_x;
            if (!rho_file.empty()) {
                for (int64_t k = 0; k < ihf->size(); ++k) {
                    pthf_x.push_back(mpthf->index_for(x{pt_x, k}));
                }
            } else {
                pthf_x.push_back(mpthf->index_for(x{pt_x, hf_x}));
            }

            auto weight = pjt->w;

            if (!eff_file.empty() && leading_pt / photon_es < 70) {
                auto bin = (*efficiency)[1]->FindBin(leading_pt / photon_es);
                auto cor = (*efficiency)[0]->GetBinContent(bin) / (*efficiency)[1]->GetBinContent(bin);
                if (cor < 1) { std::cout << "error" << std::endl; return -1; }
                weight *= cor;
            }

            std::vector<float> weights;
            if (!rho_file.empty()) {
                auto avg_rho = get_avg_rho(pjt, -photon_eta_abs, photon_eta_abs);

                for (int64_t k = 0; k < ihf->size(); ++k) {
                    auto bin = (*rho_weighting)[k]->FindBin(avg_rho);
                    auto cor = (*rho_weighting)[k]->GetBinContent(bin);
                    weights.push_back(weight * cor);
                }
            } else {
                weights.push_back(weight);
            }

            fill_axes(pjt, pthf_x, weights, pho_cor,
                photon_eta, photon_phi, exclude, heavyion && !no_jes,
                jet_pt_min, jet_eta_abs, jet_dr_max, 
                dphi_min_numerator, dphi_min_denominator,
                mdr, idphi, nevt,
                pjet_f_dr, pjet_f_jpt, pjet_u_dr,
		      acceptance, total);//, i);
	
            if (mix > 0) {
	      
	      //Get the HFPos
	      ULong64_t signalHFPos = posFromFloatVect(hf, &hfBins);
	      ULong64_t mapPos = mapHFCounter[signalHFPos];
	     
	      
	      for (int64_t k = 0; k < mix; ++k) {
		std::vector<std::map<std::string, float> > eventJets = mapHFToJets[signalHFPos][mapPos];

		fill_axes(eventJets, pthf_x, weights, pho_cor,
			  photon_eta, photon_phi, exclude,// heavyion && !no_jes,
			  jet_pt_min, jet_eta_abs, jet_dr_max,
			  dphi_min_numerator, dphi_min_denominator,
			  mdr, idphi, nmix,
			  mix_pjet_f_dr, mix_pjet_f_jpt, mix_pjet_u_dr,
			  acceptance, total);//, i);
		
		++mapPos;
		//If you looped around go to 0 - maybe throw an error for not using unique events
		if(mapPos >= mapHFToJets[signalHFPos].size()) mapPos = 0;
	      }
	      
	      mapHFCounter[signalHFPos] = mapPos;
	      
	      //Commenting out below	      

	      /* mixing events in minimum bias */
	      /*
	      for (int64_t k = 0; k < mix; m++) {
                    if ((m + 1) % mentries == 0) {
                        std::cout << "Switch MB file" << std::endl;
                        m = -1;

                        fm->Close();

                        delete fm; delete pjtm;
                        
                        index_m = rng->Integer(mb.size());
                        fm = new TFile(mb[index_m].data(), "read");
                        tm = (TTree*)fm->Get("pj");
                        pjtm = new pjtree(gen_iso, false, heavyion, tm, { 1, 1, 1, 1, 1, 0, heavyion, 0, 0 });

                        mentries = static_cast<int64_t>(tm->GetEntries()); std::cout << mentries << std::endl;
                    }

		    //		    std::cout << "ENTRY, MIXENTRY: " << i << ", " << m << std::endl;
                    tm->GetEntry(m);


		    if (std::abs(pjtm->vz) > 15) { continue; }

		    //		    std::cout << "MIX HF: " << pjtm->hiHF << std::endl;
		    
		    if (std::abs(pjtm->hiHF / hf - 1.) > 0.1) { continue; }

                    fill_axes(pjtm, pthf_x, weights, pho_cor,
                            photon_eta, photon_phi, exclude, heavyion && !no_jes,
                            jet_pt_min, jet_eta_abs, jet_dr_max,
                            dphi_min_numerator, dphi_min_denominator,
                            mdr, idphi, nmix,
                            mix_pjet_f_dr, mix_pjet_f_jpt, mix_pjet_u_dr,
			      acceptance, total);//, i);

                    ++k;
                }
	      */	      

                tentries++;
            }


	    //CM Testing - check entry by entry for > 0.000001 deviations
	    // 3_3 shows discrepancies - run w/ index = 15
	    /*
	    const int testIndex = 15;
	    bool allMatch = true;
	    for(Int_t bIX = 0; bIX < (*pjet_u_dr)[testIndex]->GetXaxis()->GetNbins(); ++bIX){
	      Double_t diff = TMath::Abs((*pjet_u_dr)[testIndex]->GetBinContent(bIX+1) - (*mix_pjet_u_dr)[testIndex]->GetBinContent(bIX+1));

	      if(diff > TMath::Power(10, -10)){
		allMatch = false;
		break;
	      }

	    }
	
	    if(!allMatch){
	      std::cout << "MISMATCH AT ENTRY: " << i << std::endl;
	      return 0;
	   }
	    */
	    
        }

        f->Close();
    }

    /* normalise histograms */
    if (mix > 0)
        scale(1. / mix,
            mix_pjet_f_dr,
            mix_pjet_f_jpt,
            mix_pjet_u_dr);

    // /* scale by bin width */
    // scale_bin_width(
    //     pjet_f_dr,
    //     pjet_f_jpt,
    //     mix_pjet_f_dr,
    //     mix_pjet_f_jpt);

    /* normalise by number of photons (events) */
    pjet_f_dr->divide(*nevt);
    pjet_f_jpt->divide(*nevt);
    pjet_u_dr->divide(*nevt);

    mix_pjet_f_dr->divide(*nevt);
    mix_pjet_f_jpt->divide(*nevt);
    mix_pjet_u_dr->divide(*nevt);

    /* subtract histograms */
    auto sub_pjet_f_dr = new memory<TH1F>(*pjet_f_dr, "sub");
    auto sub_pjet_f_jpt = new memory<TH1F>(*pjet_f_jpt, "sub");
    auto sub_pjet_u_dr = new memory<TH1F>(*pjet_u_dr, "sub");

    *sub_pjet_f_dr -= *mix_pjet_f_dr;
    *sub_pjet_f_jpt -= *mix_pjet_f_jpt;
    *sub_pjet_u_dr -= *mix_pjet_u_dr;

    /* save histograms */
    in(output, [&]() {
        nevt->save(tag);
        nmix->save(tag);

        pjet_f_dr->save(tag);
        pjet_f_jpt->save(tag);
        pjet_u_dr->save(tag);

        mix_pjet_f_dr->save(tag);
        mix_pjet_f_jpt->save(tag);
        mix_pjet_u_dr->save(tag);

        sub_pjet_f_dr->save(tag);
        sub_pjet_f_jpt->save(tag);
        sub_pjet_u_dr->save(tag);
    });

    printf("destroying objects..\n");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return populate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
