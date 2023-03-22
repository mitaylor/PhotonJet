#ifndef SPECIFICS_H
#define SPECIFICS_H


template <typename T>
double get_avg_rho(T* pjt, double eta_min, double eta_max) {
    double result = 0;
    double count = 0;

    if(pjt->etaMin == nullptr)
        return -1;

    int NBin = pjt->etaMin->size();
    if(NBin == 0)
        return -1;

    for(int i = 0; i < NBin; i++) {
        double fraction = 1;

        if ((*pjt->etaMax)[i-1] > eta_max)
            continue;
        if ((*pjt->etaMin)[i+1] < eta_min)
            continue;
        
        if ((*pjt->etaMax)[i] > eta_max) {
            fraction = (eta_max - (*pjt->etaMax)[i-1]) / ((*pjt->etaMax)[i] - (*pjt->etaMax)[i-1]);
        }

        if ((*pjt->etaMin)[i] < eta_min) {
            fraction = ((*pjt->etaMin)[i+1] - eta_min) / ((*pjt->etaMin)[i+1] - (*pjt->etaMin)[i]);
        }

        count += fraction;
        result += (*pjt->evtRho)[i] * fraction;
    }

    if (count > 0) return result/count;
    else return -1;
}

bool in_pho_failure_region(float phoEta, float phoPhi) {
    auto ex_1 = phoEta < -1.3 && 
                phoPhi < -0.7 && phoPhi > -1.6;
    auto ex_2 = phoEta < 1.5 && phoEta > 0.1 &&
                phoPhi < 1.0 && phoPhi > 0.2;

    return ex_1 || ex_2;
}

template <typename T>
bool in_pho_failure_region(T* t, int64_t i) {
    auto ex_1 = (*t->phoEta)[i] < -1.3 && 
                (*t->phoPhi)[i] < -0.7 && (*t->phoPhi)[i] > -1.6;
    auto ex_2 = (*t->phoEta)[i] < 1.5 && (*t->phoEta)[i] > 0.1 &&
                (*t->phoPhi)[i] < 1.0 && (*t->phoPhi)[i] > 0.2;

    return ex_1 || ex_2;
}

float pho_failure_region_fraction(float eta_abs) {
    float area = 0;
    if (eta_abs > 1.3) {
        area += (-1.3 + eta_abs) * (-0.7 + 1.6);
    }
    if (eta_abs > 0.1) {
        area += (eta_abs -  0.1) * (1.0 - 0.2);
    }
    
    return (area)/(eta_abs * 2 * 3.14159 * 2);
}

bool in_jet_failure_region(float jetEta, float jetPhi) {
    auto ex_1 = jetEta < -1.3 && 
                jetPhi < -0.8 && jetPhi > -1.7;
    auto ex_2 = jetEta < 1.5 && jetEta > -0.5 &&
                jetPhi < 1.0 && jetPhi > 0;
    auto ex_3 = jetEta < 1.6 && jetEta > 1.5 &&
                jetPhi < -1.0 && jetPhi > -1.5;

    return ex_1 || ex_2 || ex_3;
}

template <typename T>
bool in_jet_failure_region(T* t, int64_t i) {
    auto ex_1 = (*t->jteta)[i] < -1.3 && 
                (*t->jtphi)[i] < -0.8 && (*t->jtphi)[i] > -1.7;
    auto ex_2 = (*t->jteta)[i] < 1.5 && (*t->jteta)[i] > -0.5 &&
                (*t->jtphi)[i] < 1.0 && (*t->jtphi)[i] > 0;
    auto ex_3 = (*t->jteta)[i] < 1.6 && (*t->jteta)[i] > 1.5 &&
                (*t->jtphi)[i] < -1.0 && (*t->jtphi)[i] > -1.5;

    return ex_1 || ex_2 || ex_3;
}

template <typename T>
bool passes_basic_electron_selections(T* t, int64_t i) {
    return (*t->eleMissHits)[i] <= 1 && (*t->eleIP3D)[i] < 0.03;
}

enum ip { incl, cent, peri, nip };
enum det { barrel, endcap, ndet };
enum wp { veto, loose, medium, tight, claustro, nwp };
enum var { hoe, see, deta, dphi, eop, nele,
        /* hoe, see, */ iso = 2, npho};

constexpr float ecuts[ip::nip][det::ndet][wp::nwp][var::nele] = {
    {    /* ip::incl */
        {    /* det::barrel */
            {    -1.,    -1.,    -1.,    -1.,    -1. }, /* wp::veto */
            {    -1.,    -1.,    -1.,    -1.,    -1. }, /* wp::loose */
            {    -1.,    -1.,    -1.,    -1.,    -1. }, /* wp::medium */
            {    -1.,    -1.,    -1.,    -1.,    -1. }, /* wp::tight */
            {    -1.,    -1.,    -1.,    -1.,    -1. }  /* wp::claustro */
        }, { /* det::endcap */
            {    -1.,    -1.,    -1.,    -1.,    -1. }, /* wp::veto */
            {    -1.,    -1.,    -1.,    -1.,    -1. }, /* wp::loose */
            {    -1.,    -1.,    -1.,    -1.,    -1. }, /* wp::medium */
            {    -1.,    -1.,    -1.,    -1.,    -1. }, /* wp::tight */
            {    -1.,    -1.,    -1.,    -1.,    -1. }  /* wp::claustro */
        }
    }, { /* ip::cent */
        {    /* det::barrel */
            { 0.2733, 0.0147, 0.0041, 0.0853, 0.0367 }, /* wp::veto */
            { 0.1616, 0.0135, 0.0038, 0.0376, 0.0177 }, /* wp::loose */
            { 0.1589, 0.0116, 0.0037, 0.0224, 0.0173 }, /* wp::medium */
            { 0.1459, 0.0104, 0.0029, 0.0206, 0.0105 }, /* wp::tight */
            {    -1.,    -1.,    -1.,    -1.,    -1. }  /* wp::claustro */
        }, { /* det::endcap */
            { 0.1898, 0.0480, 0.0097, 0.2348, 0.0300 }, /* wp::veto */
            { 0.1317, 0.0466, 0.0063, 0.1186, 0.0201 }, /* wp::loose */
            { 0.1092, 0.0418, 0.0062, 0.0373, 0.0133 }, /* wp::medium */
            { 0.0925, 0.0358, 0.0051, 0.0266, 0.0065 }, /* wp::tight */
            {    -1.,    -1.,    -1.,    -1.,    -1. }  /* wp::claustro */
        }
    }, { /* ip::peri */
        {    /* det::barrel */
            { 0.1814, 0.0113, 0.0037, 0.1280, 0.1065 }, /* wp::veto */
            { 0.1268, 0.0107, 0.0035, 0.0327, 0.0774 }, /* wp::loose */
            { 0.0311, 0.0101, 0.0033, 0.0210, 0.0701 }, /* wp::medium */
            { 0.0067, 0.0099, 0.0026, 0.0170, 0.0077 }, /* wp::tight */
            {    -1.,    -1.,    -1.,    -1.,    -1. }  /* wp::claustro */
        }, { /* det::endcap */
            { 0.1138, 0.0376, 0.0074, 0.2085, 0.0237 }, /* wp::veto */
            { 0.0977, 0.0339, 0.0067, 0.0838, 0.0193 }, /* wp::loose */
            { 0.0810, 0.0316, 0.0051, 0.0384, 0.0192 }, /* wp::medium */
            { 0.0655, 0.0288, 0.0044, 0.0266, 0.0123 }, /* wp::tight */
            {    -1.,    -1.,    -1.,    -1.,    -1. }  /* wp::claustro */
        }
    }
};

template <det T, wp U, typename V>
bool passes_electron_id(V* t, int64_t i, bool heavyion) {
    if (!passes_basic_electron_selections(t, i)) { return false; }

    auto iptype = heavyion ? (t->hiBin < 60 ? ip::cent : ip::peri) : ip::incl;
    return (*t->eleHoverEBc)[i] < ecuts[iptype][T][U][var::hoe]
        && (*t->eleSigmaIEtaIEta_2012)[i] < ecuts[iptype][T][U][var::see]
        && std::abs((*t->eledEtaSeedAtVtx)[i]) < ecuts[iptype][T][U][var::deta]
        && std::abs((*t->eledPhiAtVtx)[i]) < ecuts[iptype][T][U][var::dphi]
        && std::abs((*t->eleEoverPInv)[i]) < ecuts[iptype][T][U][var::eop];
}

#endif /* SPECIFICS_H */
