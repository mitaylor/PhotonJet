#include "../include/triggers.h"

triggers::triggers(std::vector<std::string> const& paths)
    : accepts(new int32_t[paths.size()]),
      _count(static_cast<int64_t>(paths.size())),
      _paths(paths) { }

void triggers::read(TTree* t) {
    for (int64_t i = 0; i < static_cast<int64_t>(_paths.size()); ++i) {
        t->SetBranchStatus(_paths[i].data(), 1);
        t->SetBranchAddress(_paths[i].data(), accepts + i);

        _map[_paths[i]] = i;
    }
}

void triggers::reset() {
    for (int64_t i = 0; i < _count; ++i)
        accepts[i] = -1;
}

int32_t triggers::accept() const {
    for (int64_t i = 0; i < _count; ++i)
        if (accepts[i] == 1) { return 1; }

    return 0;
}

template <>
int32_t triggers::accept<int64_t>(int64_t const& index) const {
    return accepts[index];
}

template <>
int32_t triggers::accept<std::string>(std::string const& path) const {
    auto pos = _map.find(path);
    if (pos == std::end(_map)) { return -1; }
    return accepts[pos->second];
}
