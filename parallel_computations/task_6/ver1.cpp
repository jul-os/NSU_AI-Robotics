#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <cstdlib>

#include <boost/program_options.hpp>

namespace opts = boost::program_options;
constexpr double TIMESTEP = -0.01;

struct RunConfig {
    int dim = 30;
    double tol = 1e-6;
    int max_steps = 1000000;
    std::string path = "result.dat";
};

RunConfig process_arguments(int argc, char** argv) {
    RunConfig cfg;
    opts::options_description spec("Solver parameters");
    spec.add_options()
        ("help,h", "Display usage")
        ("size,s", opts::value<int>(&cfg.dim)->default_value(30), "Grid dimension NxN")
        ("accuracy,a", opts::value<double>(&cfg.tol)->default_value(1e-6), "Convergence threshold")
        ("max-iter,i", opts::value<int>(&cfg.max_steps)->default_value(1000000), "Iteration limit")
        ("output,o", opts::value<std::string>(&cfg.path)->default_value("result.dat"), "Output file path");

    opts::variables_map storage;
    opts::store(opts::parse_command_line(argc, argv, spec), storage);
    opts::notify(storage);

    if (storage.count("help")) {
        std::cout << spec << std::endl;
        std::exit(0);
    }
    return cfg;
}

using Field = std::vector<double>;
inline int get_pos(int r, int c, int n) { return r * n + c; }
Field create_field(int n) { return Field(n * n, 0.0); }

void setup_edges(double* buf, int n) {
    if (n < 2) return;
    const double v00 = 10.0, v10 = 20.0, v01 = 20.0, v11 = 30.0;
    const double inv = 1.0 / (n - 1);

    for (int c = 0; c < n; ++c) {
        double f = c * inv;
        buf[get_pos(0, c, n)]       = v00 + f * (v10 - v00);
        buf[get_pos(n - 1, c, n)]   = v01 + f * (v11 - v01);
    }
    for (int r = 0; r < n; ++r) {
        double f = r * inv;
        buf[get_pos(r, 0, n)]       = v00 + f * (v01 - v00);
        buf[get_pos(r, n - 1, n)]   = v10 + f * (v11 - v10);
    }
}

void execute_relaxation(const double* src, double* dst, int n) {
    int total = n * n;
    #pragma acc parallel loop collapse(2) present(src[0:total], dst[0:total])
    for (int r = 1; r < n - 1; ++r) {
        for (int c = 1; c < n - 1; ++c) {
            int p = get_pos(r, c, n);
            double lap = -4.0 * src[p]
                       + src[get_pos(r - 1, c, n)]
                       + src[get_pos(r + 1, c, n)]
                       + src[get_pos(r, c - 1, n)]
                       + src[get_pos(r, c + 1, n)];
            dst[p] = src[p] - TIMESTEP * lap;
        }
    }
}

double compute_residual(const double* src, const double* dst, int n) {
    double err = 0.0;
    int total = n * n;
    #pragma acc parallel loop collapse(2) present(src[0:total], dst[0:total]) reduction(max:err)
    for (int r = 1; r < n - 1; ++r) {
        for (int c = 1; c < n - 1; ++c) {
            int p = get_pos(r, c, n);
            double d = std::abs(dst[p] - src[p]);
            if (d > err) err = d;
        }
    }
    return err;
}

void export_data(const Field& data, const std::string& path) {
    std::ofstream ofs(path, std::ios::binary);
    ofs.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size() * sizeof(double)));
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
    RunConfig cfg = process_arguments(argc, argv);

    Field buf_a = create_field(cfg.dim);
    Field buf_b = create_field(cfg.dim);
    setup_edges(buf_a.data(), cfg.dim);
    setup_edges(buf_b.data(), cfg.dim);

    double* ptr_src = buf_a.data();
    double* ptr_dst = buf_b.data();
    const int total = cfg.dim * cfg.dim;
    int steps = 0;
    double err = 1.0;

    auto start = std::chrono::steady_clock::now();
    #pragma acc data copy(ptr_src[0:total], ptr_dst[0:total])
    {
        do {
            execute_relaxation(ptr_src, ptr_dst, cfg.dim);
            err = compute_residual(ptr_src, ptr_dst, cfg.dim);
            std::swap(ptr_src, ptr_dst);
            ++steps;
        } while (err >= cfg.tol && steps < cfg.max_steps);
    }
    auto end = std::chrono::steady_clock::now();
    double dt = std::chrono::duration<double>(end - start).count();

    std::cout << "Elapsed: " << dt << " s\n";
    std::cout << "Iterations: " << steps << "\n";
    std::cout << "Residual: " << err << "\n";

    export_data(buf_a, cfg.path);
    std::cout << "Saved to " << cfg.path << "\n";

    if (cfg.dim <= 15) {
        render_matrix(buf_a);
    }
    return 0;
}