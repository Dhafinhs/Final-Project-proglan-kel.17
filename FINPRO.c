/*
Program Simulator Kasir Untuk UMKM dengan database
Kelompok 17
2306267145 - Dhafin Hamizan Setiawan
2306250756 - Daffa Bagus Dhiananto

deskripsi program:
Program ini berfungsi sebagai database untuk penjual UMKM agar
efisien dalam mengatur barang dan program kasir untuk membantu perhitungan
pembelian.
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h> // Menambahkan header OpenMP

// Definisi struktur node untuk linked list
typedef struct Barang {
    char nama[50];
    int harga;
    char kode[10];
    int stok;
    struct Barang *next;
} Barang;

// Definisi struktur akun
typedef struct Akun {
    char username[50];
    char password[50];
    int isAdmin;
} Akun;

// Fungsi untuk menambahkan barang ke linked list
void tambahBarang(Barang **head, char *nama, int harga, char *kode, int stok) {
    Barang *newNode = (Barang *)malloc(sizeof(Barang));
    strcpy(newNode->nama, nama);
    newNode->harga = harga;
    strcpy(newNode->kode, kode);
    newNode->stok = stok;
    newNode->next = *head;
    *head = newNode;
}

// Fungsi untuk menyimpan data barang ke file
void simpanDataBarang(Barang *head) {
    FILE *file = fopen("barang.txt", "w");
    if (file == NULL) {
        printf("Gagal membuka file barang.txt\n");
        return;
    }

    Barang *current = head;
    while (current != NULL) {
        fprintf(file, "%s %d %s %d\n", current->nama, current->harga, current->kode, current->stok);
        current = current->next;
    }

    fclose(file);
}

// Fungsi untuk membaca data barang dari file
int bacaDataBarang(Barang **head) {
    FILE *file = fopen("barang.txt", "r");
    if (file == NULL) {
        printf("Gagal membuka file barang.txt\n");
        return 0;
    }

    char nama[50];
    int harga;
    char kode[10];
    int stok;
    int jumlah_barang = 0;

    while (fscanf(file, "%s %d %s %d", nama, &harga, kode, &stok) != EOF) {
        tambahBarang(head, nama, harga, kode, stok);
        jumlah_barang++;
    }

    fclose(file);
    return jumlah_barang;
}

// Fungsi untuk melakukan input data barang
void input(Barang **head, int *jumlah_barang) {
    printf("Masukkan jumlah barang: ");
    scanf("%d", jumlah_barang);
    getchar(); // Membersihkan buffer

    // Menggunakan OpenMP untuk parallel input
    #pragma omp parallel for
    for (int i = 0; i < *jumlah_barang; i++) {
        char nama[50];
        int harga;
        char kode[10];
        int stok;

        printf("Barang %d:\n", i + 1);
        printf("Nama: ");
        scanf(" %[^\n]%*c", nama);
        printf("Harga: ");
        scanf("%d", &harga);
        getchar(); // Membersihkan buffer
        printf("Kode: ");
        scanf("%s", kode);
        printf("Stok: ");
        scanf("%d", &stok);

        #pragma omp critical
        tambahBarang(head, nama, harga, kode, stok);
    }

    printf("Berhasil memasukkan %d data ke dalam database\n", *jumlah_barang);

    // Simpan data barang ke file setelah input
    simpanDataBarang(*head);
}

// Fungsi untuk mencetak data barang
void cetakDataBarang(Barang *head) {
    printf("\nData Barang:\n");
    printf("%-20s %-10s %-10s %-10s\n", "Nama", "Harga", "Kode", "Stok"); // Header tabel
    printf("-------------------------------------------------\n");
    Barang *current = head;
    while (current != NULL) {
        printf("%-20s %-10d %-10s %-10d\n", current->nama, current->harga, current->kode, current->stok);
        current = current->next;
    }
}

// Fungsi untuk mencari barang berdasarkan kode
void searchByKode(Barang *head) {
    char kodeSearch[10];
    printf("Masukkan kode yang dicari: ");
    scanf("%s", kodeSearch);

    int found = 0;
    printf("\nData Barang:\n");
    printf("%-20s %-10s %-10s %-10s\n", "Nama", "Harga", "Kode", "Stok"); // Header tabel
    printf("-------------------------------------------------\n");

    Barang *current = head;
    while (current != NULL) {
        if (strcmp(current->kode, kodeSearch) == 0) {
            printf("%-20s %-10d %-10s %-10d\n", current->nama, current->harga, current->kode, current->stok);
            found = 1;
            break;
        }
        current = current->next;
    }

    if (!found) {
        printf("Tidak ada barang dengan kode %s.\n", kodeSearch);
    }
}

// Fungsi untuk mencari barang berdasarkan nama
void searchByName(Barang *head) {
    char namaSearch[50];
    printf("Masukkan nama yang dicari: ");
    scanf(" %[^\n]%*c", namaSearch);

    int found = 0;
    printf("\nData Barang:\n");
    printf("%-20s %-10s %-10s %-10s\n", "Nama", "Harga", "Kode", "Stok"); // Header tabel
    printf("-------------------------------------------------\n");

    Barang *current = head;
    while (current != NULL) {
        if (strstr(current->nama, namaSearch) != NULL) {
            printf("%-20s %-10d %-10s %-10d\n", current->nama, current->harga, current->kode, current->stok);
            found = 1;
            break;
        }
        current = current->next;
    }

    if (!found) {
        printf("Tidak ada barang dengan nama %s.\n", namaSearch);
    }
}

// Fungsi untuk melakukan transaksi
void kasir(Barang *head) {
    char kode[10];
    int jumlah;
    int totalHarga = 0;
    char lagi;
    int transaksiID = 1;

    printf("==== MODE KASIR ====\n");

    #pragma omp barrier // Menggunakan OpenMP Barrier
    do {
        printf("\nMasukkan kode barang: ");
        scanf("%s", kode);

        Barang *current = head;
        int found = 0;

        #pragma omp parallel
        {
            #pragma omp single // Menggunakan OpenMP Single
            {
                while (current != NULL) {
                    if (strcmp(current->kode, kode) == 0) {
                        found = 1;
                        printf("Barang %s dengan harga %d ditemukan. Stok tersedia: %d\n", current->nama, current->harga, current->stok);
                        
                        do {
                            printf("Masukkan jumlah barang: ");
                            scanf("%d", &jumlah);
                            if (jumlah <= current->stok) {
                                break;
                            }
                            printf("Stok tidak cukup. Stok %s tersedia: %d, diminta: %d\n", current->nama, current->stok, jumlah);
                        } while (1);

                        totalHarga += current->harga * jumlah;

                        #pragma omp critical
                        current->stok -= jumlah;
                        printf("Stok %s tersisa: %d\n", current->nama, current->stok);

                        // Simpan transaksi ke file
                        simpanTransaksi(kode, jumlah, current->harga, transaksiID++);
                        break;
                    }
                    current = current->next;
                }
            }
        }

        if (!found) {
            printf("Barang dengan kode %s tidak ditemukan.\n", kode);
        }

        printf("Tambahkan barang lain? (y/n): ");
        scanf(" %c", &lagi);
    } while (lagi == 'y' || lagi == 'Y');

    printf("\nTotal harga pembelian: %d\n", totalHarga);
    printf("Terima kasih telah berbelanja!\n");

    // Simpan data barang ke file setelah transaksi
    simpanDataBarang(head);
}

// Fungsi untuk menyimpan transaksi ke file
void simpanTransaksi(const char *kode, int jumlah, int harga, int transaksiID) {
    FILE *file = fopen("transaksi.txt", "a"); // Membuka file dengan mode append
    if (file == NULL) {
        printf("Gagal membuka file transaksi.txt\n");
        return;
    }

    // Menulis transaksi ke file
    fprintf(file, "Transaksi ID: %d, Kode Barang: %s, Jumlah: %d, Harga Satuan: %d, Total Harga: %d\n",
            transaksiID, kode, jumlah, harga, jumlah * harga);

    fclose(file); // Menutup file
}

// Fungsi untuk melihat history transaksi
void lihatTransaksi() {
    FILE *file = fopen("transaksi.txt", "r");
    if (file == NULL) {
        printf("Gagal membuka file transaksi.txt\n");
        return;
    }

    char buffer[255];
    printf("\nHistory Transaksi:\n");
    printf("-------------------------------------------------\n");
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }
    printf("-------------------------------------------------\n");

    fclose(file); // Menutup file
}

// Fungsi untuk membebaskan memori yang digunakan oleh linked list
void freeLinkedList(Barang *head) {
    Barang *current = head;
    Barang *next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

// Fungsi untuk membaca akun dari file
void readAkun(Akun *akun, int *jumlah_akun) {
    FILE *file = fopen("akun.txt", "r");
    if (file == NULL) {
        printf("Gagal membuka file akun.txt\n");
        return;
    }

    *jumlah_akun = 0;
    while (fscanf(file, "%s %s %d", akun[*jumlah_akun].username, akun[*jumlah_akun].password, &akun[*jumlah_akun].isAdmin) != EOF) {
        (*jumlah_akun)++;
    }

    fclose(file);
}

// Fungsi untuk menambahkan akun baru
void registerAkun() {
    FILE *file = fopen("akun.txt", "a");
    if (file == NULL) {
        printf("Gagal membuka file akun.txt\n");
        return;
    }

    Akun akun;
    printf("Masukkan username: ");
    scanf("%s", akun.username);
    printf("Masukkan password: ");
    scanf("%s", akun.password);
    printf("Apakah ini akun admin? (1 = ya, 0 = tidak): ");
    scanf("%d", &akun.isAdmin);

    fprintf(file, "%s %s %d\n", akun.username, akun.password, akun.isAdmin);
    fclose(file);

    printf("Akun berhasil didaftarkan!\n");
}

// Fungsi untuk login
int login(Akun *akun, int jumlah_akun, int *isAdmin) {
    char username[50];
    char password[50];

    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    for (int i = 0; i < jumlah_akun; i++) {
        if (strcmp(akun[i].username, username) == 0 && strcmp(akun[i].password, password) == 0) {
            *isAdmin = akun[i].isAdmin;
            return 1;
        }
    }

    return 0;
}

int main() {
    Barang *head = NULL;
    int jumlah_barang = 0;
    Akun akun[100];
    int jumlah_akun = 0;

    jumlah_barang = bacaDataBarang(&head);
    readAkun(akun, &jumlah_akun);

    int isAdmin = 0;
    int loginSuccess = 0;
    do {
        printf("==== LOGIN ====\n");
        printf("1. Login\n2. Register\n3. Keluar\nPilihan: ");
        int pilihan;
        scanf("%d", &pilihan);
        getchar(); // Membersihkan buffer
        system("cls");

        switch (pilihan) {
        case 1:
            loginSuccess = login(akun, jumlah_akun, &isAdmin);
            if (!loginSuccess) {
                printf("Login gagal! Username atau password salah.\n");
                system("pause");
                system("cls");
            }
            break;
        case 2:
            registerAkun();
            system("pause");
            system("cls");
            break;
        case 3:
            printf("==== TERIMA KASIH ====\n");
            return 0;
        }
    } while (!loginSuccess);

    int pilihan;
    do {
        printf("\nSelamat Datang Di Simulator Kasir!\n\n");
        printf("==== KASIR BARANG ===\n");
        printf("1. Input Data\n2. Database\n3. Search\n4. Kasir\n5. History Transaksi\n6. Keluar\nPilihan Anda: ");
        scanf("%d", &pilihan);
        getchar(); // Membersihkan buffer
        system("cls");

        if (isAdmin || pilihan == 4 || pilihan == 2 || pilihan == 6) {
            switch (pilihan) {
            case 1: // Input data barang
                input(&head, &jumlah_barang);
                system("pause");
                system("cls");
                break;
            case 2: // Lihat database
                if (jumlah_barang > 0) {
                    cetakDataBarang(head);
                } else {
                    printf("Data barang kosong. Silakan input data terlebih dahulu.\n");
                }
                system("pause");
                system("cls");
                break;
            case 3: // Pilihan searching
                if (jumlah_barang > 0) {
                    printf("Pilih Pencarian\n1. Nama\n2. Kode\nPilihan: ");
                    int mode;
                    scanf("%d", &mode);
                    getchar(); // Membersihkan buffer

                    switch (mode) {
                    case 1: // Search by name
                        searchByName(head);
                        break;
                    case 2: // Search by kode
                        searchByKode(head);
                        break;
                    }
                } else {
                    printf("Data barang kosong. Silakan input data terlebih dahulu.\n");
                }
                system("pause");
                system("cls");
                break;
            case 4: // Opsi kasir
                if (jumlah_barang > 0) {
                    kasir(head);
                } else {
                    printf("Data barang kosong. Silakan input data terlebih dahulu.\n");
                }
                system("pause");
                system("cls");
                break;
            case 5: // Lihat history transaksi (hanya untuk admin)
                if (isAdmin) {
                    lihatTransaksi();
                } else {
                    printf("Anda tidak memiliki akses ke fitur ini. Silakan login sebagai admin.\n");
                }
                system("pause");
                system("cls");
                break;
            }
        } else {
            printf("Anda tidak memiliki akses ke fitur ini. Silakan login sebagai admin.\n");
            system("pause");
            system("cls");
        }
        
    } while (pilihan != 6);

    // Membebaskan memori yang digunakan oleh linked list
    freeLinkedList(head);

    printf("==== TERIMA KASIH ====\n");
    return 0;
}
