#pragma once
#include <bits/stdc++.h>

class Timer {
public:
    using clock = std::chrono::steady_clock;

    Timer() { reset(); }
    explicit Timer(double time_limit_seconds) : time_limit_seconds_(time_limit_seconds) { reset(); }

    void reset() { start_ = clock::now(); }
    void set_time_limit(double seconds) { time_limit_seconds_ = seconds; }
    double time_limit() const { return time_limit_seconds_; }

    long long elapsed_ns() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - start_).count();
    }
    long long elapsed_us() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - start_).count();
    }
    long long elapsed_ms() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start_).count();
    }
    double elapsed() const {
        return std::chrono::duration<double>(clock::now() - start_).count();
    }
    double elapsed_sec() const { return elapsed(); }

    bool expired() const { return expired(time_limit_seconds_); }
    bool expired(double seconds) const { return elapsed() >= seconds; }
    double remaining() const { return remaining(time_limit_seconds_); }
    double remaining(double seconds) const { return std::max(0.0, seconds - elapsed()); }
    double progress() const {
        if (time_limit_seconds_ <= 0.0) return 0.0;
        return elapsed() / time_limit_seconds_;
    }
    double progress_clamped() const { return std::min(1.0, progress()); }

private:
    clock::time_point start_;
    double time_limit_seconds_ = 0.0;
};

class ScopedTimer {
public:
    explicit ScopedTimer(std::string label = "elapsed", std::ostream& os = std::cerr)
        : label_(std::move(label)), os_(&os) {}
    ~ScopedTimer() {
        if (os_) *os_ << label_ << ": " << timer_.elapsed_ms() << " ms\n";
    }
    Timer& timer() { return timer_; }
    const Timer& timer() const { return timer_; }

private:
    std::string label_;
    std::ostream* os_;
    Timer timer_;
};
