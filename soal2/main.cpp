// Pemakaian: ./vX [--echo] < input.txt
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cctype>
#include "buffer.h"

namespace {

const int MAXTOK = 16;

int splitTokens(const std::string& line, std::string tok[], int maxTok) {
    std::istringstream iss(line);
    std::string t;
    int n = 0;
    while (iss >> t) {
        if (n < maxTok) tok[n] = t;
        ++n;
    }
    return n;
}

bool parseInt(const std::string& s, long& out) {
    if (s.empty()) return false;
    std::size_t i = 0;
    bool neg = false;
    if (s[0] == '+' || s[0] == '-') { neg = (s[0] == '-'); i = 1; }
    if (i >= s.size()) return false;
    long v = 0;
    int digits = 0;
    for (; i < s.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
        if (v == 0 && s[i] == '0') continue;
        if (digits < 9) v = v * 10 + (s[i] - '0');
        ++digits;
    }
    if (digits > 9) v = 1000000000L;
    out = neg ? -v : v;
    return true;
}

void cmdTulis(Buffer& B, const std::string tok[], int n) {
    if (n < 2) { std::cout << "DITOLAK: format TULIS k v1 ... vk\n"; return; }
    long k;
    if (!parseInt(tok[1], k) || k < 1 || k > KAPASITAS) {
        std::cout << "DITOLAK: k harus bilangan bulat 1-" << KAPASITAS << "\n"; return;
    }
    if (n != k + 2) {
        std::cout << "DITOLAK: jumlah nilai harus tepat k = " << k << " (diberikan " << (n - 2) << ")\n";
        return;
    }
    int nilai[KAPASITAS];
    for (long i = 0; i < k; ++i) {                // validasi seluruh batch dulu
        long v;
        if (!parseInt(tok[2 + i], v) || v < NILAI_MIN || v > NILAI_MAX) {
            std::cout << "DITOLAK: nilai '" << tok[2 + i] << "' di luar rentang "
                      << NILAI_MIN << ".." << NILAI_MAX << " atau bukan bilangan bulat\n";
            return;
        }
        nilai[i] = static_cast<int>(v);
    }
    if (ruangKosong(B) < k) {
        std::cout << "DITOLAK: ruang kosong hanya " << ruangKosong(B) << ", diminta " << k << "\n";
        return;
    }
    std::cout << "TULIS berhasil:";
    for (long i = 0; i < k; ++i) {                // baru sekarang antrean diubah
        int no = nomorBerikut(B);
        enqueue(B, nilai[i]);
        std::cout << (i == 0 ? " " : ", ") << no << ":" << nilai[i];
    }
    std::cout << "\n";
}

void cmdAmbil(Buffer& B, const std::string tok[], int n) {
    if (n != 2) { std::cout << "DITOLAK: format AMBIL k\n"; return; }
    long k;
    if (!parseInt(tok[1], k) || k < 1 || k > KAPASITAS) {
        std::cout << "DITOLAK: k harus bilangan bulat 1-" << KAPASITAS << "\n"; return;
    }
    if (jumlah(B) < k) {
        std::cout << "DITOLAK: data hanya " << jumlah(B) << " rekaman, diminta " << k << "\n";
        return;
    }
    std::cout << "AMBIL berhasil:";
    for (long i = 0; i < k; ++i) {
        Rekaman r;
        dequeue(B, r);
        std::cout << (i == 0 ? " " : ", ") << r.nomor << ":" << r.nilai;
    }
    std::cout << "\n";
}

void cmdLihat(const Buffer& B) {
    std::cout << "[" << namaVersi() << "]\n";
    std::cout << "FIFO (" << jumlah(B) << " rekaman):";
    if (jumlah(B) == 0) std::cout << " (kosong)";
    for (int i = 0; i < jumlah(B); ++i) {
        Rekaman r = elemenKe(B, i);
        std::cout << (i == 0 ? " " : ", ") << r.nomor << ":" << r.nilai;
    }
    std::cout << "\n";
    printFisik(B);
    std::cout << "Nomor berikutnya: " << nomorBerikut(B) << "\n";
    std::cout << "Total pergeseran: " << totalGeser(B) << " rekaman\n";
}

void cmdRingkas(const Buffer& B) {
    int c = jumlah(B);
    std::cout << "RINGKAS: jumlah " << c;
    if (c == 0) {
        std::cout << "; total 0; minimum TIDAK ADA; maksimum TIDAK ADA; rata-rata TIDAK ADA\n";
        return;
    }
    long total = 0;
    int mn = elemenKe(B, 0).nilai, mx = mn;
    for (int i = 0; i < c; ++i) {
        int v = elemenKe(B, i).nilai;
        total += v;
        if (v < mn) mn = v;
        if (v > mx) mx = v;
    }
    std::cout << "; total " << total << "; minimum " << mn << "; maksimum " << mx
              << "; rata-rata " << std::fixed << std::setprecision(2)
              << static_cast<double>(total) / c << "\n";
    std::cout.unsetf(std::ios::floatfield);
}

} // namespace

int main(int argc, char* argv[]) {
    bool echo = false;
    for (int i = 1; i < argc; ++i)
        if (std::string(argv[i]) == "--echo") echo = true;

    Buffer B;
    createBuffer(B);

    std::cout << "=== Buffer Hasil Ukur (kapasitas " << KAPASITAS << ") - " << namaVersi() << " ===\n"
              << "Perintah: TULIS k v1..vk | AMBIL k | LIHAT | RINGKAS | KOSONGKAN | SELESAI\n";

    std::string line;
    while (std::getline(std::cin, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::string tok[MAXTOK];
        int n = splitTokens(line, tok, MAXTOK);
        if (n == 0) continue;
        if (tok[0][0] == '#') { if (echo) std::cout << line << "\n"; continue; }
        if (echo) std::cout << "> " << line << "\n";

        std::string cmd = tok[0];
        for (std::size_t i = 0; i < cmd.size(); ++i)
            cmd[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(cmd[i])));

        if      (cmd == "TULIS")     cmdTulis(B, tok, n);
        else if (cmd == "AMBIL")     cmdAmbil(B, tok, n);
        else if (cmd == "LIHAT")     cmdLihat(B);
        else if (cmd == "RINGKAS")   cmdRingkas(B);
        else if (cmd == "KOSONGKAN") { kosongkan(B); std::cout << "KOSONGKAN: buffer dikosongkan (nomor urut & penghitung pergeseran dipertahankan)\n"; }
        else if (cmd == "SELESAI")   { std::cout << "SELESAI. Program berakhir.\n"; break; }
        else std::cout << "DITOLAK: perintah tidak dikenal\n";
    }
    return 0;
}