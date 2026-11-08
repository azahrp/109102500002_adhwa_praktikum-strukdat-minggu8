#include "buffer.h"
#include <iostream>
#if VERSI != 2
#error "buffer_v2.cpp harus dikompilasi dengan -DVERSI=2"
#endif

const char* namaVersi() { return "Versi 2 - Array dengan pemadatan"; }

void createBuffer(Buffer& B) {
    B.head = -1; B.tail = -1;
    B.nextNo = 1; B.geser = 0;
}
void kosongkan(Buffer& B) { B.head = -1; B.tail = -1; }

int  jumlah(const Buffer& B)      { return (B.head == -1) ? 0 : B.tail - B.head + 1; }
int  ruangKosong(const Buffer& B) { return KAPASITAS - jumlah(B); }

bool enqueue(Buffer& B, int nilai) {
    if (B.head == -1) {
        B.head = 0; B.tail = 0;
    } else if (B.tail == KAPASITAS - 1) {
        if (B.head == 0) return false;            // penuh
        int n = jumlah(B);                        // pemadatan
        for (int i = 0; i < n; ++i) {
            B.data[i] = B.data[B.head + i];
            B.geser++;
        }
        B.head = 0;
        B.tail = n;
    } else {
        B.tail++;
    }
    B.data[B.tail].nomor = B.nextNo++;
    B.data[B.tail].nilai = nilai;
    return true;
}

bool dequeue(Buffer& B, Rekaman& hasil) {
    if (B.head == -1) return false;
    hasil = B.data[B.head];
    if (B.head == B.tail) { B.head = -1; B.tail = -1; }
    else                  { B.head++; }
    return true;
}

Rekaman elemenKe(const Buffer& B, int i) { return B.data[B.head + i]; }
int  nomorBerikut(const Buffer& B) { return B.nextNo; }
long totalGeser(const Buffer& B)   { return B.geser; }

void printFisik(const Buffer& B) {
    std::cout << "Fisik:";
    for (int i = 0; i < KAPASITAS; ++i) {
        std::cout << " [" << i << "]=";
        if (B.head != -1 && i >= B.head && i <= B.tail)
            std::cout << B.data[i].nomor << ":" << B.data[i].nilai;
        else std::cout << "kosong";
    }
    std::cout << "\nPenanda: head = " << B.head << ", tail = " << B.tail << "\n";
}