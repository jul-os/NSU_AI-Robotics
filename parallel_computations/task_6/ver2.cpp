#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include <boost/program_options.hpp>

namespace bpo = boost::program_options;
constexpr double DT = -0.01;

struct Params {
    int n = 30;
    double tol = 1e-6;
    int limit = 1000000;
    std::string output_path = "result.dat";
};

Params read_cli(int argc, char** argv) {
    Params cfg;
    bpo::options_description spec("Solver parameters");
    spec.add_options()
        ("help,h", "Display usage")
        ("size,s", bpo::value<int>(&cfg.n)->default_value(30), "Grid dimension NxN")
        ("accuracy,a", bpo::value<double>(&cfg.tol)->default_value(1e-6), "Convergence threshold")
        ("max-iter,i", bpo::value<int>(&cfg.limit)->default_value(1000000), "Iteration limit")
        ("output,o", bpo::value<std::string>(&cfg.output_path)->default_value("result.dat"), "Output file path");

    bpo::variables_map storage;
    bpo::store(bpo::parse_command_line(argc, argv, spec), storage);
    bpo::notify(storage);

    if (storage.count("help")) {
        std::cout << spec << std::endl;
        std::exit(0);
    }
    return cfg;
}

using Buffer = std::vector<double>;
inline int coord(int r, int c, int dim) { return r * dim + c; }
Buffer alloc_buffer(int dim) { return Buffer(dim * dim, 0.0); }

void apply_bc(double* buf, int dim) {
    if (dim < 2) return;
    const double v00 = 10.0, v10 = 20.0, v01 = 20.0, v11 = 30.0;
    const double inv = 1.0 / (dim - 1);

    for (int c = 0; c < dim; ++c) {
        double f = c * inv;
        buf[coord(0, c, dim)]         = v00 + f * (v10 - v00);
        buf[coord(dim - 1, c, dim)]   = v01 + f * (v11 - v01);
    }
    for (int r = 0; r < dim; ++r) {
        double f = r * inv;
        buf[coord(r, 0, dim)]         = v00 + f * (v01 - v00);
        buf[coord(r, dim - 1, dim)]   = v10 + f * (v11 - v10);
    }
}

double relax_and_check(const double* src, double* dst, int dim) {
    double err = 0.0;
    int total = dim * dim;

    #pragma acc parallel loop collapse(2) present(src[0:total], dst[0:total]) reduction(max:err)
    for (int r = 1; r < dim - 1; ++r) {
        for (int c = 1; c < dim - 1; ++c) {
            int p = coord(r, c, dim);
            double lap = -4.0 * src[p]
                       + src[coord(r - 1, c, dim)]
                       + src[coord(r + 1, c, dim)]
                       + src[coord(r, c - 1, dim)]
                       + src[coord(r, c + 1, dim)];
            double new_val = src[p] - DT * lap;
            dst[p] = new_val;
            double diff = std::abs(new_val - src[p]);
            if (diff > err) err = diff;
        }
    }
    return err;
}

void dump_binary(const Buffer& data, const std::string& path) {
    std::ofstream ofs(path, std::ios::binary);
    ofs.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size() * sizeof(double)));
}

void show_matrix(const Buffer& data) {
    int dim = static_cast<int>(std::sqrt(data.size()));
    std::cout << std::fixed << std::setprecision(2);
    for (int r = 0; r < dim; ++r) {
        for (int c = 0; c < dim; ++c) {
            std::cout << std::setw(7) << data[coord(r, c, dim)] << ' ';
        }
        std::cout << '\n';
    }
}

int main(int argc, char** argv) {
    Params cfg = read_cli(argc, argv);

    Buffer a = alloc_buffer(cfg.n);
    Buffer b = alloc_buffer(cfg.n);
    apply_bc(a.data(), cfg.n);
    apply_bc(b.data(), cfg.n);

    double* ptr_a = a.data();
    double* ptr_b = b.data();
    const int total = cfg.n * cfg.n;
    int steps = 0;
    double err = 1.0;

    auto t0 = std::chrono::steady_clock::now();
    #pragma acc data copy(ptr_a[0:total], ptr_b[0:total])
    {
        do {
            err = relax_and_check(ptr_a, ptr_b, cfg.n);
            std::swap(ptr_a, ptr_b);
            ++steps;
        } while (err >= cfg.tol && steps < cfg.limit);
    }
    auto t1 = std::chrono::steady_clock::now();
    double dt = std::chrono::duration<double>(t1 - t0).count();

    std::cout << "Time: " << dt << " s\n";
    std::cout << "Steps: " << steps << "\n";
    std::cout << "Max diff: " << err << "\n";

    dump_binary(a, cfg.output_path);
    std::cout << "Written: " << cfg.output_path << "\n";

    if (cfg.n <= 15) {
        show_matrix(a);
    }
    return 0;
}