// Pemakaian: ./paket [--echo] < input.txt
#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include "paket.h"

namespace {

const int MAXTOK = 8;

struct Statistik {
    long jumlah;
    long berat;
};

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

void cmdMasuk(Queue& A, const Queue& B, const std::string tok[], int n) {
    if (n != 3) { std::cout << "DITOLAK: format MASUK kode berat\n"; return; }
    const std::string& kode = tok[1];
    if (kode.size() < 1 || kode.size() > 10) {
        std::cout << "DITOLAK: kode harus 1-10 karakter\n"; return;
    }
    long berat;
    if (!parseInt(tok[2], berat) || berat < 1 || berat > 5000) {
        std::cout << "DITOLAK: berat harus bilangan bulat 1-5000 gram\n"; return;
    }
    if (findElm(A, kode) != nullptr) {
        std::cout << "DITOLAK: kode " << kode << " masih aktif di antrean A\n"; return;
    }
    if (findElm(B, kode) != nullptr) {
        std::cout << "DITOLAK: kode " << kode << " masih aktif di antrean B\n"; return;
    }
    adrPaket P = alokasi(kode, static_cast<int>(berat));
    if (P == nullptr) { std::cout << "DITOLAK: alokasi memori gagal\n"; return; }
    if (!enqueue(A, P)) {
        dealokasi(P);
        std::cout << "DITOLAK: node gagal disambung ke A\n"; return;
    }
    std::cout << "MASUK berhasil: " << kode << " (" << berat << " gram) di belakang A\n";
}

void cmdPindah(Queue& A, Queue& B, const std::string tok[], int n) {
    if (n != 2) { std::cout << "DITOLAK: format PINDAH n\n"; return; }
    long k;
    if (!parseInt(tok[1], k) || k <= 0) {
        std::cout << "DITOLAK: n harus bilangan bulat positif\n"; return;
    }
    int tersedia = nbList(A);
    if (k > tersedia) {
        std::cout << "DITOLAK: PINDAH " << k << " melebihi isi A (A hanya berisi "
                  << tersedia << " paket)\n";
        return;
    }
    std::cout << "PINDAH berhasil: " << k << " paket A -> B:";
    for (long i = 0; i < k; ++i) {
        adrPaket P = dequeue(A);      // lepas dari A (alamat tetap)
        enqueue(B, P);                // sambung ke belakang B
        std::cout << (i == 0 ? " " : ", ") << P->kode;
    }
    std::cout << "\n";
}

void cmdKirim(Queue& B, Statistik& st, const std::string tok[], int n) {
    if (n != 2) { std::cout << "DITOLAK: format KIRIM n\n"; return; }
    long k;
    if (!parseInt(tok[1], k) || k <= 0) {
        std::cout << "DITOLAK: n harus bilangan bulat positif\n"; return;
    }
    int tersedia = nbList(B);
    if (k > tersedia) {
        std::cout << "DITOLAK: KIRIM " << k << " melebihi isi B (B hanya berisi "
                  << tersedia << " paket)\n";
        return;
    }
    long gram = 0;
    for (long i = 0; i < k; ++i) {
        adrPaket P = dequeue(B);
        std::cout << "Dikirim: " << P->kode << " (" << P->berat << " gram)\n";
        st.jumlah += 1;
        st.berat  += P->berat;
        gram      += P->berat;
        dealokasi(P);                 // data dicatat dulu, baru dibebaskan
    }
    std::cout << "KIRIM berhasil: " << k << " paket, " << gram << " gram. "
              << "Total terkirim: " << st.jumlah << " paket, " << st.berat << " gram\n";
}

void cmdCari(const Queue& A, const Queue& B, const std::string tok[], int n) {
    if (n != 2) { std::cout << "DITOLAK: format CARI kode\n"; return; }
    adrPaket P = findElm(A, tok[1]);
    char lok = 'A';
    const Queue* Q = &A;
    if (P == nullptr) { P = findElm(B, tok[1]); lok = 'B'; Q = &B; }
    if (P == nullptr) { std::cout << "TIDAK DITEMUKAN\n"; return; }
    std::cout << "Ditemukan: " << P->kode << " di " << lok << ", posisi "
              << posisi(*Q, P) << ", berat " << P->berat << " gram\n";
}

// Tambahan: bukti alamat node tetap, keanggotaan berubah.
void cmdAlamat(const Queue& A, const Queue& B, const std::string tok[], int n) {
    if (n != 2) { std::cout << "DITOLAK: format ALAMAT kode\n"; return; }
    adrPaket P = findElm(A, tok[1]);
    if (P == nullptr) P = findElm(B, tok[1]);
    if (P == nullptr) { std::cout << "TIDAK DITEMUKAN\n"; return; }
    std::cout << "Alamat node " << tok[1] << " = " << static_cast<const void*>(P)
              << " | fFindElm(A) = " << (fFindElm(A, P) ? "ya" : "tidak")
              << " | fFindElm(B) = " << (fFindElm(B, P) ? "ya" : "tidak") << "\n";
}

void cmdLihat(const Queue& A, const Queue& B) {
    std::cout << "Antrean A: "; printInfo(A); std::cout << "\n";
    std::cout << "Antrean B: "; printInfo(B); std::cout << "\n";
}

void cmdRekap(const Queue& A, const Queue& B, const Statistik& st) {
    std::cout << "REKAP\n";
    std::cout << "  A        : " << nbList(A) << " paket, total berat " << totalBerat(A) << " gram\n";
    std::cout << "  B        : " << nbList(B) << " paket, total berat " << totalBerat(B) << " gram\n";
    std::cout << "  Terkirim : " << st.jumlah << " paket, total berat " << st.berat << " gram\n";
}

void cmdBersihkan(Queue& A, Queue& B) {
    int a = nbList(A), b = nbList(B);
    delAll(A);
    delAll(B);
    std::cout << "BERSIHKAN: " << a << " paket dari A dan " << b
              << " paket dari B dihapus (statistik terkirim tidak berubah)\n";
}

// Tambahan: verifikasi invarian.
void cmdCek(const Queue& A, const Queue& B) {
    bool ok = cekInvarian(A) && cekInvarian(B);
    for (adrPaket P = A.head; P != nullptr && ok; P = P->next)
        if (fFindElm(B, P)) ok = false;
    std::cout << "CEK invarian (head/tail, tail->next, keanggotaan tunggal): "
              << (ok ? "OK" : "GAGAL") << "\n";
}

} // namespace

int main(int argc, char* argv[]) {
    bool echo = false;
    for (int i = 1; i < argc; ++i)
        if (std::string(argv[i]) == "--echo") echo = true;

    Queue A, B;
    createQueue(A);
    createQueue(B);
    Statistik st = {0, 0};

    std::cout << "=== Sistem Sortir & Pengiriman Paket ===\n"
              << "Perintah: MASUK kode berat | PINDAH n | KIRIM n | CARI kode | LIHAT | REKAP\n"
              << "          BERSIHKAN | SELESAI   (tambahan: ALAMAT kode, CEK)\n";

    std::string line;
    while (std::getline(std::cin, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::string tok[MAXTOK];
        int n = splitTokens(line, tok, MAXTOK);
        if (n == 0) continue;
        if (tok[0][0] == '#') {
            if (echo) std::cout << line << "\n";
            continue;
        }
        if (echo) std::cout << "> " << line << "\n";

        std::string cmd = tok[0];
        for (std::size_t i = 0; i < cmd.size(); ++i)
            cmd[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(cmd[i])));

        if      (cmd == "MASUK")     cmdMasuk(A, B, tok, n);
        else if (cmd == "PINDAH")    cmdPindah(A, B, tok, n);
        else if (cmd == "KIRIM")     cmdKirim(B, st, tok, n);
        else if (cmd == "CARI")      cmdCari(A, B, tok, n);
        else if (cmd == "ALAMAT")    cmdAlamat(A, B, tok, n);
        else if (cmd == "LIHAT")     cmdLihat(A, B);
        else if (cmd == "REKAP")     cmdRekap(A, B, st);
        else if (cmd == "BERSIHKAN") cmdBersihkan(A, B);
        else if (cmd == "CEK")       cmdCek(A, B);
        else if (cmd == "SELESAI") {
            std::cout << "SELESAI: " << (nbList(A) + nbList(B))
                      << " node sisa dibebaskan. Program berakhir.\n";
            break;
        }
        else std::cout << "DITOLAK: perintah tidak dikenal\n";
    }
    delAll(A);
    delAll(B);
    return 0;
}