#include "paket.h"
#include <iostream>
#include <new>

void createQueue(Queue& Q) {
    Q.head = nullptr;
    Q.tail = nullptr;
}

bool isEmptyQueue(const Queue& Q) {
    return Q.head == nullptr;
}

adrPaket alokasi(const std::string& kode, int berat) {
    adrPaket P = new (std::nothrow) Paket;
    if (P != nullptr) {
        P->kode = kode;
        P->berat = berat;
        P->next = nullptr;
    }
    return P;
}

void dealokasi(adrPaket& P) {
    if (P != nullptr) {
        delete P;
        P = nullptr;                         // cegah delete kedua
    }
}

bool enqueue(Queue& Q, adrPaket P) {
    if (P == nullptr) return false;
    if (P->next != nullptr) return false;    // node masih tersambung
    if (Q.tail == P) return false;
    if (isEmptyQueue(Q)) {
        Q.head = P;
        Q.tail = P;
    } else {
        Q.tail->next = P;
        Q.tail = P;
    }
    return true;
}

adrPaket dequeue(Queue& Q) {
    if (isEmptyQueue(Q)) return nullptr;
    adrPaket P = Q.head;
    Q.head = P->next;
    if (Q.head == nullptr) {
        Q.tail = nullptr;
    }
    P->next = nullptr;                       // node lepas total
    return P;
}

adrPaket findElm(const Queue& Q, const std::string& kode) {
    for (adrPaket P = Q.head; P != nullptr; P = P->next) {
        if (P->kode == kode) return P;
    }
    return nullptr;
}

bool fFindElm(const Queue& Q, adrPaket P) {
    if (P == nullptr) return false;
    for (adrPaket C = Q.head; C != nullptr; C = C->next) {
        if (C == P) return true;
    }
    return false;
}

int nbList(const Queue& Q) {
    int n = 0;
    for (adrPaket P = Q.head; P != nullptr; P = P->next) ++n;
    return n;
}

void printInfo(const Queue& Q) {
    if (isEmptyQueue(Q)) {
        std::cout << "(kosong)";
        return;
    }
    for (adrPaket P = Q.head; P != nullptr; P = P->next) {
        std::cout << P->kode << "(" << P->berat << "g)";
        if (P->next != nullptr) std::cout << " -> ";
    }
}

void delAll(Queue& Q) {
    while (!isEmptyQueue(Q)) {
        adrPaket P = dequeue(Q);
        dealokasi(P);
    }
}

int posisi(const Queue& Q, adrPaket P) {
    int pos = 1;
    for (adrPaket C = Q.head; C != nullptr; C = C->next, ++pos) {
        if (C == P) return pos;
    }
    return 0;
}

long totalBerat(const Queue& Q) {
    long t = 0;
    for (adrPaket P = Q.head; P != nullptr; P = P->next) t += P->berat;
    return t;
}

bool cekInvarian(const Queue& Q) {
    if (Q.head == nullptr) return Q.tail == nullptr;
    if (Q.tail == nullptr) return false;
    adrPaket P = Q.head;
    int langkah = 0;
    while (P->next != nullptr) {
        P = P->next;
        if (++langkah > 1000000) return false;
    }
    return P == Q.tail && Q.tail->next == nullptr;
}