#include "../include/trunk.h"

#include "TFile.h"

TH1F* frame(float xmin, float xmax, float ymin, float ymax) {
    auto h = new TH1F("frame", "", 1, xmin, xmax);
    h->SetAxisRange(ymin, ymax, "Y");

    return h;
}

void in(std::string const& name, std::function<void()> f) {
    auto fout = new TFile(name.data(), "recreate");

    f();

    fout->Close();
}
