
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include <boost/program_options.hpp>

namespace opts = boost::program_options;
constexpr double DT = -0.01;
constexpr int BATCH_FREQ = 100;

struct Settings {
    int dim = 30;
    double tol = 1e-6;
    int limit = 1000000;
    std::string path = "result.dat";
};

Settings process_cli(int argc, char** argv) {
    Settings cfg;
    opts::options_description spec("Solver parameters");
    spec.add_options()
        ("help,h", "Display usage")
        ("size,s", opts::value<int>(&cfg.dim)->default_value(30), "Grid dimension NxN")
        ("accuracy,a", opts::value<double>(&cfg.tol)->default_value(1e-6), "Convergence threshold")
        ("max-iter,i", opts::value<int>(&cfg.limit)->default_value(1000000), "Iteration limit")
        ("output,o", opts::value<std::string>(&cfg.path)->default_value("result.dat"), "Output file path");

    opts::variables_map vm;
    opts::store(opts::parse_command_line(argc, argv, spec), vm);
    opts::notify(vm);

    if (vm.count("help")) {
        std::cout << spec << std::endl;
        std::exit(0);
    }
    return cfg;
}

using Field = std::vector<double>;
inline int get_pos(int r, int c, int n) { return r * n + c; }
Field create_field(int n) { return Field(n * n, 0.0); }

void apply_boundaries(double* buf, int n) {
    if (n < 2) return;
    const double v00 = 10.0, v10 = 20.0, v01 = 20.0, v11 = 30.0;
    const double inv = 1.0 / (n - 1);

    for (int c = 0; c < n; ++c) {
        double t = c * inv;
        buf[get_pos(0, c, n)]       = v00 + t * (v10 - v00);
        buf[get_pos(n - 1, c, n)]   = v01 + t * (v11 - v01);
    }
    for (int r = 0; r < n; ++r) {
        double t = r * inv;
        buf[get_pos(r, 0, n)]       = v00 + t * (v01 - v00);
        buf[get_pos(r, n - 1, n)]   = v10 + t * (v11 - v10);
    }
}

void advance_field(const double* src, double* dst, int n) {
    const int total = n * n;
    #pragma acc parallel loop collapse(2) present(src[0:total], dst[0:total])
    for (int r = 1; r < n - 1; ++r) {
        for (int c = 1; c < n - 1; ++c) {
            const int p = r * n + c;
            const double lap = -4.0 * src[p]
                             + src[(r - 1) * n + c]
                             + src[(r + 1) * n + c]
                             + src[p - 1]
                             + src[p + 1];
            dst[p] = src[p] - DT * lap;
        }
    }
}

double advance_and_check(const double* src, double* dst, int n) {
    double err = 0.0;
    const int total = n * n;

    #pragma acc parallel loop collapse(2) present(src[0:total], dst[0:total]) reduction(max:err)
    for (int r = 1; r < n - 1; ++r) {
        for (int c = 1; c < n - 1; ++c) {
            const int p = get_pos(r, c, n);
            const double lap = -4.0 * src[p]
                             + src[get_pos(r - 1, c, n)]
                             + src[get_pos(r + 1, c, n)]
                             + src[get_pos(r, c - 1, n)]
                             + src[get_pos(r, c + 1, n)];
            const double val = src[p] - DT * lap;
            dst[p] = val;
            double diff = std::abs(val - src[p]);
            if (diff > err) err = diff;
        }
    }
    return err;
}

void dump_binary(const Field& data, const std::string& path) {
    std::ofstream out(path, std::ios::binary);
    out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size() * sizeof(double)));
}

void render_matrix(const Field& data) {
    int n = static_cast<int>(std::sqrt(data.size()));
    std::cout << std::fixed << std::setprecision(2);
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            std::cout << std::setw(7) << data[get_pos(r, c, n)] << ' ';
        }
        std::cout << '\n';
    }
}

int main(int argc, char** argv) {
    Settings cfg = process_cli(argc, argv);

    Field buf_a = create_field(cfg.dim);
    Field buf_b = create_field(cfg.dim);
    apply_boundaries(buf_a.data(), cfg.dim);
    apply_boundaries(buf_b.data(), cfg.dim);

    double* ptr_a = buf_a.data();
    double* ptr_b = buf_b.data();
    const int total = cfg.dim * cfg.dim;
    int steps = 0;
    double err = 0.0;

    auto t0 = std::chrono::steady_clock::now();
    #pragma acc data copy(ptr_a[0:total], ptr_b[0:total])
    {
        while (steps < cfg.limit) {
            int batch = std::min(BATCH_FREQ, cfg.limit - steps);

            for (int k = 0; k < batch - 1; ++k) {
                advance_field(ptr_a, ptr_b, cfg.dim);
                std::swap(ptr_a, ptr_b);
                ++steps;
            }

            err = advance_and_check(ptr_a, ptr_b, cfg.dim);
            std::swap(ptr_a, ptr_b);
            ++steps;

            if (err < cfg.tol) break;
        }
    }
    auto t1 = std::chrono::steady_clock::now();
    double dt = std::chrono::duration<double>(t1 - t0).count();

    std::cout << "Time: " << dt << " s\n";
    std::cout << "Steps: " << steps << "\n";
    std::cout << "Error: " << err << "\n";

    dump_binary(buf_a, cfg.path);
    std::cout << "Saved: " << cfg.path << "\n";

    if (cfg.dim <= 15) render_matrix(buf_a);
    return 0;
}