#include "buffer.h"
#include <iostream>
#if VERSI != 3
#error "buffer_v3.cpp harus dikompilasi dengan -DVERSI=3"
#endif

const char* namaVersi() { return "Versi 3 - Circular queue"; }

void createBuffer(Buffer& B) {
    B.head = 0; B.count = 0;
    B.nextNo = 1; B.geser = 0;
}
void kosongkan(Buffer& B) { B.head = 0; B.count = 0; }

int  jumlah(const Buffer& B)      { return B.count; }
int  ruangKosong(const Buffer& B) { return KAPASITAS - B.count; }

bool enqueue(Buffer& B, int nilai) {
    if (B.count == KAPASITAS) return false;
    int tulis = (B.head + B.count) % KAPASITAS;   // dihitung, tidak disimpan
    B.data[tulis].nomor = B.nextNo++;
    B.data[tulis].nilai = nilai;
    B.count++;
    return true;
}

bool dequeue(Buffer& B, Rekaman& hasil) {
    if (B.count == 0) return false;
    hasil = B.data[B.head];
    B.head = (B.head + 1) % KAPASITAS;
    B.count--;
    if (B.count == 0) B.head = 0;
    return true;
}

Rekaman elemenKe(const Buffer& B, int i) { return B.data[(B.head + i) % KAPASITAS]; }
int  nomorBerikut(const Buffer& B) { return B.nextNo; }
long totalGeser(const Buffer& B)   { return B.geser; }   // selalu 0

void printFisik(const Buffer& B) {
    std::cout << "Fisik:";
    for (int i = 0; i < KAPASITAS; ++i) {
        std::cout << " [" << i << "]=";
        int jarak = (i - B.head + KAPASITAS) % KAPASITAS;
        if (jarak < B.count)
            std::cout << B.data[i].nomor << ":" << B.data[i].nilai;
        else std::cout << "kosong";
    }
    std::cout << "\nPenanda: head = " << B.head << ", count = " << B.count;
    if (B.count > 0)
        std::cout << " (indeks rekaman terakhir dihitung = "
                  << (B.head + B.count - 1) % KAPASITAS << ")";
    std::cout << "\n";
}