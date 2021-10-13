#ifndef TRUNK_H
#define TRUNK_H

#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"

#include <array>
#include <functional>
#include <string>

template <typename... T>
void for_contents(std::function<float(std::array<double, sizeof...(T) + 1>)> f,
                  TH1* h, T... hs) {
    for (int64_t i = 1; i < h->GetNbinsX(); ++i) {
        double val = h->GetBinContent(i);
        h->SetBinContent(i, f({val, hs->GetBinContent(i)...}));
    }
}

template <typename... T>
void for_indexed_contents(
        std::function<float(int64_t, std::array<double, sizeof...(T) + 1>)> f,
        TH1* h, T... hs) {
    for (int64_t i = 1; i < h->GetNbinsX(); ++i) {
        double val = h->GetBinContent(i);
        h->SetBinContent(i, f(i - 1, {val, hs->GetBinContent(i)...}));
    }
}

TH1F* frame(float xmin, float xmax, float ymin, float ymax);

template <typename T>
TH1F* frame(T* gx, float ymin, float ymax) {
    return frame(gx->GetXmin(), gx->GetXmax(), ymin, ymax);
}

template <typename T>
TH1F* frame(float xmin, float xmax, T* gy) {
    return frame(xmin, xmax, gy->GetXmin(), gy->GetXmax());
}

template <typename T, typename U>
TH1F* frame(T* gx, U* gy) {
    return frame(gx->GetXmin(), gx->GetXmax(), gy);
}

template <typename T, typename U>
TGraph* graph(std::vector<T> const& abscissa, std::vector<U> const& ordinate) {
    return new TGraph(abscissa.size(), abscissa.data(), ordinate.data());
}

void in(std::string const& name, std::function<void()> f);

template <typename T>
void outline(std::string const& file, T* obj, std::string const& opt) {
    TCanvas* c = new TCanvas("c1", "", 400, 400);
    obj->Draw(opt.data()); c->SaveAs(file.data());

    delete c;
}

#endif /* TRUNK_H */
