#include "../include/train.h"

train::train(std::vector<std::string> const& files)
    : _files(files) { }

TChain* train::attach(std::string const& carriage, bool active) {
    if (active) {
        _train[carriage] = new TChain(carriage.data());
        _train[carriage]->SetBranchAddress("*", 0);

        return _train[carriage];
    }

    return nullptr;
}

TChain* train::operator[](std::string const& carriage) const {
    auto chain = _train.find(carriage);
    if (chain != std::end(_train))
        return chain->second;

    return nullptr;
}

void train::operator()() {
    for (auto const& f : _files)
        for (auto const& carriage : _train)
            carriage.second->Add(f.data());
}

int64_t train::count() const {
    return static_cast<int64_t>(std::begin(_train)->second->GetEntries());
}

void train::get(int64_t index) const {
    for (auto const& carriage : _train)
        carriage.second->GetEntry(index);
}
