#ifndef TRAIN_H
#define TRAIN_H

#include "TChain.h"

#include <string>
#include <vector>
#include <unordered_map>

class train {
  public:
    train(std::vector<std::string> const& files);

    train(train const&) = delete;
    train& operator=(train const&) = delete;
    ~train() = default;

    TChain* attach(std::string const& carriage, bool active);
    TChain* operator[](std::string const& carriage) const;

    void operator()();

    int64_t count() const;
    void get(int64_t index) const;

  private:
    std::vector<std::string> _files;

    std::unordered_map<std::string, TChain*> _train;
};

#endif /* TRAIN_H */
