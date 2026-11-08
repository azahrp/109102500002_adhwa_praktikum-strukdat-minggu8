// Soal 2 - Buffer Pencatatan Hasil Ukur
// Nama: .............................  NIM: .............................
// Pilih versi saat kompilasi: -DVERSI=1 | -DVERSI=2 | -DVERSI=3
#ifndef BUFFER_H
#define BUFFER_H

#ifndef VERSI
#define VERSI 1
#endif

const int KAPASITAS = 6;
const int NILAI_MIN = -1000;
const int NILAI_MAX = 1000;

struct Rekaman {
    int nomor;
    int nilai;
};

struct Buffer {
    Rekaman data[KAPASITAS];
    int  head;
#if VERSI == 3
    int  count;       // versi 3: head + count, tanpa tail
#else
    int  tail;        // versi 1 & 2: head + tail
#endif
    int  nextNo;
    long geser;
};

const char* namaVersi();
void createBuffer(Buffer& B);
void kosongkan(Buffer& B);                    // nextNo & geser tidak direset
int  jumlah(const Buffer& B);
int  ruangKosong(const Buffer& B);
bool enqueue(Buffer& B, int nilai);
bool dequeue(Buffer& B, Rekaman& hasil);
Rekaman elemenKe(const Buffer& B, int i);     // 0 = depan
int  nomorBerikut(const Buffer& B);
long totalGeser(const Buffer& B);
void printFisik(const Buffer& B);

#endif