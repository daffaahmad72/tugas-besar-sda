#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TUGAS 100
#define MAX_SUBTUGAS 10

// Struktur untuk sub-tugas
typedef struct {
    char nama[100];
} SubTugas;

// Struktur untuk tugas utama
typedef struct {
    char nama[100];
    char deadline[20]; // Format: DD/MM/YYYY
    SubTugas subTugas[MAX_SUBTUGAS];
    int jumlahSubTugas;
} Tugas;

// Struktur untuk antrian prioritas
typedef struct {
    Tugas data[MAX_TUGAS];
    int size;
} PriorityQueue;

// Fungsi untuk inisialisasi antrian
void initQueue(PriorityQueue *queue) {
    queue->size = 0;
}

// Fungsi untuk validasi format tanggal
int isValidDate(const char *date) {
    int day, month, year;
    if (sscanf(date, "%d/%d/%d", &day, &month, &year) != 3) {
        return 0;
    }
    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 2023) {
        return 0;
    }
    return 1;
}

// Fungsi untuk membandingkan deadline
int compareDeadline(const char *d1, const char *d2) {
    int day1, month1, year1;
    int day2, month2, year2;

    sscanf(d1, "%d/%d/%d", &day1, &month1, &year1);
    sscanf(d2, "%d/%d/%d", &day2, &month2, &year2);

    if (year1 != year2) return year1 - year2;
    if (month1 != month2) return month1 - month2;
    return day1 - day2;
}

// Fungsi untuk menghitung kesulitan otomatis
int hitungKesulitan(Tugas tugas) {
    if (tugas.jumlahSubTugas == 0) return 1; // Minimal 1 jika tidak ada sub-tugas
    int kesulitan = tugas.jumlahSubTugas * 2; // Setiap sub-tugas menambah 2 poin
    return (kesulitan > 10) ? 10 : kesulitan; // Maksimal 10
}

// Fungsi untuk menambahkan sub-tugas
void tambahSubTugas(Tugas *tugas) {
    if (tugas->jumlahSubTugas >= MAX_SUBTUGAS) {
        printf("Maksimal sub-tugas tercapai!\n");
        return;
    }

    SubTugas sub;
    printf("\nMasukkan nama sub-tugas: ");
    fgets(sub.nama, sizeof(sub.nama), stdin);
    sub.nama[strcspn(sub.nama, "\n")] = 0;

    tugas->subTugas[tugas->jumlahSubTugas] = sub;
    tugas->jumlahSubTugas++;

    printf("Sub-tugas berhasil ditambahkan!\n");
}

// Fungsi untuk menambahkan tugas ke antrian dengan prioritas
void enqueue(PriorityQueue *queue, Tugas newTugas) {
    if (queue->size >= MAX_TUGAS) {
        printf("Antrian penuh!\n");
        return;
    }

    int kesulitan = hitungKesulitan(newTugas);
    int pos = queue->size;

    while (pos > 0) {
        int cmp = compareDeadline(newTugas.deadline, queue->data[pos - 1].deadline);
        int kesulitanLama = hitungKesulitan(queue->data[pos - 1]);

        if (cmp < 0 || (cmp == 0 && kesulitan > kesulitanLama)) {
            queue->data[pos] = queue->data[pos - 1];
            pos--;
        } else {
            break;
        }
    }

    queue->data[pos] = newTugas;
    queue->size++;
}

// Fungsi untuk mengambil tugas dari antrian
Tugas dequeue(PriorityQueue *queue) {
    Tugas kosong = {"", "", {{"", 0}}, 0};

    if (queue->size == 0) {
        printf("Antrian kosong!\n");
        return kosong;
    }

    Tugas tugas = queue->data[0];

    for (int i = 0; i < queue->size - 1; i++) {
        queue->data[i] = queue->data[i + 1];
    }

    queue->size--;
    return tugas;
}

// Fungsi untuk menampilkan daftar tugas
void displayQueue(PriorityQueue *queue) {
    if (queue->size == 0) {
        printf("\nTidak ada tugas dalam antrian.\n");
        return;
    }

    printf("\nDaftar Tugas (Prioritas Deadline & Kesulitan):\n");
    printf("------------------------------------------------------------------\n");
    printf("No. | Nama Tugas           | Deadline     | Sub-Tugas | Kesulitan\n");
    printf("------------------------------------------------------------------\n");

    for (int i = 0; i < queue->size; i++) {
        Tugas t = queue->data[i];
        printf("%-3d | %-20s | %-12s | %-9d | %d\n",
               i + 1, t.nama, t.deadline, t.jumlahSubTugas,
               hitungKesulitan(t));
    }
    printf("------------------------------------------------------------------\n");
}

// Fungsi untuk melihat detail tugas
void lihatDetailTugas(PriorityQueue *queue) {
    if (queue->size == 0) {
        printf("\nTidak ada tugas dalam antrian.\n");
        return;
    }

    displayQueue(queue);
    int taskNum;
    printf("\nMasukkan nomor tugas untuk melihat detail: ");
    scanf("%d", &taskNum);
    getchar();

    if (taskNum < 1 || taskNum > queue->size) {
        printf("Nomor tugas tidak valid!\n");
        return;
    }

    Tugas tugas = queue->data[taskNum - 1];
    printf("\nDetail Tugas:\n");
    printf("Nama      : %s\n", tugas.nama);
    printf("Deadline  : %s\n", tugas.deadline);
    printf("Jumlah Sub-Tugas: %d\n", tugas.jumlahSubTugas);
    printf("Tingkat Kesulitan: %d\n", hitungKesulitan(tugas));

    if (tugas.jumlahSubTugas > 0) {
        printf("\nDaftar Sub-Tugas:\n");
        printf("-----------------------------\n");
        for (int i = 0; i < tugas.jumlahSubTugas; i++) {
            printf("%d. %s\n", i + 1, tugas.subTugas[i].nama);
        }
        printf("-----------------------------\n");
    }
}

int main() {
    PriorityQueue queue;
    initQueue(&queue);
    int choice;

    printf("===== SISTEM MANAJEMEN TUGAS DENGAN SUB-TUGAS =====\n");

    do {
        printf("\nMenu Utama:\n");
        printf("1. Tambah Tugas\n");
        printf("2. Lihat Daftar Tugas\n");
        printf("3. Lihat Detail Tugas\n");
        printf("4. Proses Tugas Terdepan\n");
        printf("5. Keluar\n");
        printf("Pilihan Anda: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: {
                Tugas newTugas;
                newTugas.jumlahSubTugas = 0;

                printf("\nMasukkan Nama Tugas: ");
                fgets(newTugas.nama, sizeof(newTugas.nama), stdin);
                newTugas.nama[strcspn(newTugas.nama, "\n")] = 0;

                printf("Masukkan Deadline (DD/MM/YYYY): ");
                scanf("%s", newTugas.deadline);
                getchar();

                while (!isValidDate(newTugas.deadline)) {
                    printf("Format deadline tidak valid! Gunakan format DD/MM/YYYY\n");
                    printf("Masukkan Deadline (DD/MM/YYYY): ");
                    scanf("%s", newTugas.deadline);
                    getchar();
                }

                int subChoice;
                do {
                    printf("\nMenu Sub-Tugas:\n");
                    printf("1. Tambah Sub-Tugas\n");
                    printf("2. Selesai\n");
                    printf("Pilihan: ");
                    scanf("%d", &subChoice);
                    getchar();

                    switch (subChoice) {
                        case 1:
                            tambahSubTugas(&newTugas);
                            break;
                        case 2:
                            printf("Tugas utama berhasil ditambahkan!\n");
                            break;
                        default:
                            printf("Pilihan tidak valid!\n");
                    }
                } while (subChoice != 2);

                enqueue(&queue, newTugas);
                break;
            }

            case 2:
                displayQueue(&queue);
                break;

            case 3:
                lihatDetailTugas(&queue);
                break;

            case 4: {
                Tugas diproses = dequeue(&queue);
                if (strlen(diproses.nama) > 0) {
                    printf("\nTugas yang diproses:\n");
                    printf("Nama      : %s\n", diproses.nama);
                    printf("Deadline  : %s\n", diproses.deadline);
                    printf("Kesulitan : %d\n", hitungKesulitan(diproses));
                }
                break;
            }

            case 5:
                printf("\nTerima kasih! Program selesai.\n");
                break;

            default:
                printf("Pilihan tidak valid. Silakan coba lagi.\n");
        }
    } while (choice != 5);

    return 0;
}
