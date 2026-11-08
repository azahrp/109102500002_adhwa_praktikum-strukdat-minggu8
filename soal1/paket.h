// Soal 1 - Sistem Sortir dan Pengiriman Paket (ADT Queue, SLL)
// Nama: .............................  NIM: .............................
#ifndef PAKET_H
#define PAKET_H

#include <string>

struct Paket {
    std::string kode;   // satu kata, 1-10 karakter
    int berat;          // 1-5000 gram
    Paket* next;
};
typedef Paket* adrPaket;

struct Queue {
    adrPaket head;
    adrPaket tail;
};

void     createQueue(Queue& Q);
bool     isEmptyQueue(const Queue& Q);
adrPaket alokasi(const std::string& kode, int berat);
void     dealokasi(adrPaket& P);
bool     enqueue(Queue& Q, adrPaket P);
adrPaket dequeue(Queue& Q);
adrPaket findElm(const Queue& Q, const std::string& kode);
bool     fFindElm(const Queue& Q, adrPaket P);
int      nbList(const Queue& Q);
void     printInfo(const Queue& Q);
void     delAll(Queue& Q);

// fungsi bantu
int      posisi(const Queue& Q, adrPaket P);
long     totalBerat(const Queue& Q);
bool     cekInvarian(const Queue& Q);

#endif