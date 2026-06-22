#pragma once

template <class S> struct edge {
    int from, to;
    S weight;
    edge(int from_, int to_, S weight_) : from(from_), to(to_), weight(weight_) {}
    edge() : from(-1), to(-1), weight() {}
};
