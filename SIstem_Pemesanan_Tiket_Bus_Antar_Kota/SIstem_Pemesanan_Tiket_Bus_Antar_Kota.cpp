#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <locale>
#include <cstdlib>
#include <limits>
#include <cctype>
using namespace std;

struct JadwalBus {
    string asal;
    string tujuan;
    int harga;
    tm jadwal;
};

struct Penumpang {
    string nama;
    unsigned long long nik;
    unsigned long long nomor;
};

vector<JadwalBus> dataBus;
vector<Penumpang> dataPenumpang;
JadwalBus currentJadwal;
vector<JadwalBus> jadwalDipilih;
tm stringToTime(const string& timeStr);
string buffer, konfirmasi, kodePemesanan, buffercetak, kodeUnik, formatHarga(int harga), buat_kode_pemesanan(int length = 20);
void displaydataBus(const vector<JadwalBus>& dataBus, bool kelas, int tk), cetakHeader(string kalimat), cetakSubHeader(string kalimat), tampilkan_jadwal(), pilih_jadwal(), hitung_total(), cetak_ringkasan();
bool init(), jadwalSudahBenar(std::string g), isDigits(const string& str);
int pilihan, pilihanKelas, jumlahPenumpang, pilihJadwal();
unsigned long long totalHargaTiket;
char pilihUlangJadwal;

int main() {    
    if (!init()) return 1;    
    tampilkan_jadwal();
    pilih_jadwal();       
    hitung_total();   
    kodeUnik = buat_kode_pemesanan();
    cetak_ringkasan();
    return 0;
}

string buat_kode_pemesanan(int length) {
    const string alphanumeric = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int charSetSize = alphanumeric.size();
    string result;

    srand(static_cast<unsigned>(time(0)));

    for (int i = 0; i < length; ++i) {
        result += alphanumeric[rand() % charSetSize];
    }

    return result;
}

void cetak_ringkasan() {
    cout << "\n\n";
    cout << "+-----------------------------------------------------+\n"
         << "|                        INVOICE                      |\n"
         << "+-----------------------------------------------------+\n"
         << "|                                                     |\n";

    buffercetak = "Kode Pemesanan : " + kodeUnik;
    cout << "|" << buffercetak << string(52 - buffercetak.length(), ' ') << " |\n";

    cout << "|                                                     |\n"
         << "|                                                     |\n"
         << "|                    DATA PERJALANAN                  |\n"
         << "|                                                     |\n";

    buffercetak = "Asal Kota : " + currentJadwal.asal;
    cout << "|" << buffercetak << string(52 - buffercetak.length(), ' ') << " |\n";

    buffercetak = "Tujuan Kota : " + currentJadwal.tujuan;
    cout << "|" << buffercetak << string(52 - buffercetak.length(), ' ') << " |\n";

    buffercetak = "Jadwal : ";
    cout << "|" << buffercetak << put_time(&currentJadwal.jadwal, "%H:%M") << string(47 - buffercetak.length(), ' ') << " |\n";

    cout << "|                                                     |\n"
        << "|                                                     |\n"
        << "|                    DATA PENUMPANG                   |\n"
        << "|                                                     |\n";

    int index = 1;
    for (auto cetak : dataPenumpang) {
        std::string namaSingkat = cetak.nama.length() > 30 ? cetak.nama.substr(0, 27) + "..." : cetak.nama;
        std::string nikSingkat = to_string(cetak.nik).length() > 30 ? to_string(cetak.nik).substr(0, 27) + "..." : to_string(cetak.nik);
        std::string nomorSingkat = to_string(cetak.nomor).length() > 30 ? to_string(cetak.nomor).substr(0, 27) + "..." : to_string(cetak.nomor);
        buffercetak = "Penumpang " + to_string(index++);
        cout << "| " << buffercetak << string(52 - buffercetak.length(), ' ') << "|\n";
        cout << "|     " << namaSingkat << string(48 - namaSingkat.length(), ' ') << "|\n";
        cout << "|     " << nikSingkat << string(48 - nikSingkat.length(), ' ') << "|\n";
        cout << "|     " << nomorSingkat << string(48 - nomorSingkat.length(), ' ') << "|\n";
        cout << "|" << string(53, ' ') << "|\n";
    }
    cout << "|                                                     |\n";

    unsigned long long grandTotal;

    buffercetak = "Jumlah Penumpang : " + to_string(jumlahPenumpang);
    cout << "|" << string(52 - buffercetak.length(), ' ') << buffercetak << " |\n";

    buffercetak = "Harga Tiket per Orang : " + formatHarga(currentJadwal.harga);
    cout << "|" << string(52 - buffercetak.length(), ' ') << buffercetak << " |\n";

    buffercetak = "Total Harga Tiket : " + formatHarga(totalHargaTiket);
    cout << "|" << string(52 - buffercetak.length(), ' ') << buffercetak << " |\n";

    buffercetak = "Pajak (5%) : " + formatHarga(totalHargaTiket * 0.05);
    cout << "|" << string(52 - buffercetak.length(), ' ') << buffercetak << " |\n";

    grandTotal = totalHargaTiket * 1.05;

    if (totalHargaTiket >= 500000) {
        buffercetak = "Diskon Pembelian >500K : -" + formatHarga(totalHargaTiket * 0.1);
        cout << "|" << string(52 - buffercetak.length(), ' ') << buffercetak << " |\n";
        grandTotal -= totalHargaTiket * 0.1;
    }

    buffercetak = "Grand Total : " + formatHarga(grandTotal);
    cout << "|" << string(52, ' ') << " |\n";
    cout << "|" << string(52 - buffercetak.length(), ' ') << buffercetak << " |\n";


    cout << "+-----------------------------------------------------+\n";
}

void hitung_total() {
    totalHargaTiket = currentJadwal.harga * jumlahPenumpang;
}

void tampilkan_jadwal() {
    cetakHeader("Sistem Pemesanan Tiket Bus Antar-Kota");
}

void pilih_jadwal() {
    do {
        displaydataBus(dataBus, false, 1);
        pilihan = pilihJadwal();
        cout << "\n";
        jadwalDipilih = { dataBus[pilihan - 1] };
        currentJadwal = dataBus[pilihan - 1];
        displaydataBus(jadwalDipilih, false, 1);
        do {
            cout << "\nApakah Jadwal Sudah Benar (Y/n) ? : ";
            getline(cin, konfirmasi);
            pilihUlangJadwal = konfirmasi[0];
            if (!jadwalSudahBenar(konfirmasi)) {
                cout << "Masukkan Y atau N\n";
            }
        } while (!jadwalSudahBenar(konfirmasi));
        cout << "\n\n";

    } while (toupper(pilihUlangJadwal) == 'N');
    
    do {
        cetakHeader("Pemilihan Kelas Bus");
        displaydataBus(jadwalDipilih, true, 0);
        int tmpharga = currentJadwal.harga;
        while (true) {
            cout << "\nPilih Kelas (1-3) : ";
            getline(cin, buffer);

            try {
                pilihan = stoi(buffer);

                if (pilihan >= 1 && pilihan <= 3) {
                    pilihanKelas = pilihan;
                    break;
                }
                else {
                    cout << "Input tidak valid! Masukkan angka antara 1 dan 3." << endl;
                }
            }
            catch (const invalid_argument& e) {
                cout << "Input tidak valid! Masukkan angka yang benar." << endl;
            }
            catch (const out_of_range& e) {
                cout << "Input tidak valid! Masukkan angka yang lebih kecil dari 3." << endl;
            }
        }
        switch (pilihanKelas) {
        case 1:
            currentJadwal.harga *= 1;
            break;
        case 2:
            currentJadwal.harga *= 1.2;
            break;
        case 3:
            currentJadwal.harga *= 1.5;
            break;
        }
        vector<JadwalBus> tmp = { currentJadwal };
        cout << '\n';
        displaydataBus(tmp, false, 1);
        do {
            cout << "\nApakah Kelas Sudah Benar (Y/n) ? : ";
            getline(cin, konfirmasi);
            pilihUlangJadwal = konfirmasi[0];
            if (!jadwalSudahBenar(konfirmasi)) {
                cout << "Masukkan Y atau N\n";
            }
        } while (!jadwalSudahBenar(konfirmasi));
        if (toupper(pilihUlangJadwal) == 'N') {
            currentJadwal.harga = tmpharga;
        }
        cout << '\n';
    } while (toupper(pilihUlangJadwal) == 'N');

    cetakHeader("Deskripsi Penumpang");    
    do {
        cout << "\nJumlah Penumpang\t: ";
        getline(cin, buffer);
        if (isDigits(buffer)) {
            try {
                jumlahPenumpang = stoi(buffer);
                if (jumlahPenumpang <= 0) cout << "Jumlah Penumpang harus > 0\n";
            }
            catch (...) {
                cout << "Jumlah Penumpang Harus Berupa Angka\n";
            }
        }
        else {
            cout << "Jumlah Penumpang Harus Berupa Angka\n";
        }
    } while (!isDigits(buffer) || jumlahPenumpang <= 0);

    vector<string> question = {
        "Nama        : ",
        "NIK         : ",
        "No. Telp    : "
    };


    for (int i = 0; i < jumlahPenumpang; i++) {
        cetakSubHeader("Penumpang " + to_string(i + 1));
        string na;
        unsigned long long ni, no;
        cout << question[0];
        cin.ignore();
        getline(cin, na);
        cout << question[1];
        cin.ignore();
        cin >> ni;
        cout << question[2];
        cin.ignore();
        cin >> no;
        dataPenumpang.insert(dataPenumpang.begin(), { na, ni, no });
    }
}

bool isDigits(const string& str) {
    for (char c : str) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return !str.empty();
}

void cetakSubHeader(string kalimat) {
    int width = (30 - kalimat.length()) / 2;
    cout << '\n'
        << string(30, '=')
        << "\n" << string(width, ' ') << kalimat << "\n"
        << string(30, '=') << '\n';
}

void cetakHeader(string kalimat) {
    int width = (76 - kalimat.length()) / 2;
    cout << string(76, '=')
        << "\n\n" << string(width, ' ') << kalimat << "\n\n"
        << string(76, '=') << "\n\n";
}

bool jadwalSudahBenar(std::string g) {
    if (g.length() != 1) return false;
    char ch = toupper(g[0]);
    return ch == 'Y' || ch == 'N';
}

int pilihJadwal() {
    string input;
    int pilihan;

    while (true) {
        cetakSubHeader("Pemilihan Jadwal");
        cout << "Pilih Jadwal (1-100) : ";
        getline(cin, input);

        try {
            pilihan = stoi(input);

            if (pilihan >= 1 && pilihan <= 100) {
                return pilihan;
            }
            else {
                cout << "Input tidak valid! Masukkan angka antara 1 dan 100." << endl;
            }
        }
        catch (const invalid_argument& e) {
            cout << "Input tidak valid! Masukkan angka yang benar." << endl;
        }
        catch (const out_of_range& e) {
            cout << "Input tidak valid! Masukkan angka yang lebih kecil dari 100." << endl;
        }
    }
}

tm stringToTime(const string& timeStr) {
    tm t = {};
    stringstream ss(timeStr);
    ss >> get_time(&t, "%H:%M");
    return t;
}

string formatHarga(int harga) {
    stringstream ss;
    ss.imbue(locale("id_ID.UTF-8"));
    ss << "Rp." << harga;
    return ss.str();
}

void displaydataBus(const vector<JadwalBus>& dataBus, bool kelas, int tk) {
  
    const int width_index = 5;
    const int width_asal = 20;
    const int width_tujuan = 20;
    const int width_harga = 15;
    const int width_jadwal = 10;
    const int width_kelas = 10;

    int totalKelas;
    if (kelas) totalKelas = 3;
    else {
        totalKelas = tk;
    }

    cout << "+" << string(width_index, '-') << "+"
        << string(width_asal, '-') << "+"
        << string(width_tujuan, '-') << "+"
        << string(width_harga, '-') << "+"
        << string(width_jadwal, '-') << "+";
    if (kelas) cout << string(width_kelas, '-') << "+";
    cout << endl;

    cout << "|" << left << setw(width_index) << "No"
        << "|" << left << setw(width_asal) << "Asal"
        << "|" << left << setw(width_tujuan) << "Tujuan"
        << "|" << left << setw(width_harga) << "Harga"
        << "|" << "Jadwal" << "    |";
    if (kelas) cout << "" << "Kelas " << "    |";
    cout << endl;

    cout << "+" << string(width_index, '-') << "+"
        << string(width_asal, '-') << "+"
        << string(width_tujuan, '-') << "+"
        << string(width_harga, '-') << "+"
        << string(width_jadwal, '-') << "+";
    if (kelas) cout << string(width_kelas, '-') << "+";
    cout << endl;    

    vector<string> listkelas = { "Ekonomi", "Bisnis", "VIP" };

    int index = 1;

    for (int i = 0; i < totalKelas; i++) {        
        for (const auto& schedule : dataBus) {
            cout << "|" << left << setw(width_index) << index++
                << "|" << left << setw(width_asal) << schedule.asal
                << "|" << left << setw(width_tujuan) << schedule.tujuan;
            float mul = 1;
                switch (i) {
                case 0:
                    mul = 1;
                    break;
                case 1:
                    mul = 1.2;
                    break;
                case 2:
                    mul = 1.5;
                    break;
                }        
                cout << "|" << left << setw(width_harga) << formatHarga(schedule.harga * mul);                
                cout << "|" << put_time(&schedule.jadwal, "%H:%M") << "     |";

            if (kelas) cout << listkelas[i] << string((10 - listkelas[i].length()), ' ') << "|";
            cout << endl;
        }
    }

    cout << "+" << string(width_index, '-') << "+"
        << string(width_asal, '-') << "+"
        << string(width_tujuan, '-') << "+"
        << string(width_harga, '-') << "+"
        << string(width_jadwal, '-') << "+";
    if (kelas) cout << string(width_kelas, '-') << "+";
    cout << endl;    
}

bool init() {
    string fileName = "Data_Jadwal_Bus_Indonesia.csv";

    ifstream file(fileName);

    if (!file.is_open()) {
        cerr << "Gagal Membuka File! \nHarap download file \'Data_Jadwal_Bus_Indonesia.csv\' yang telah tercantum" << endl;
        return false;
    }

    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string asal, tujuan, jadwalStr;
        int harga;

        getline(ss, asal, ',');
        getline(ss, tujuan, ',');
        ss >> harga;
        ss.ignore();
        getline(ss, jadwalStr);

        tm jadwal = stringToTime(jadwalStr);

        dataBus.push_back({ asal, tujuan, harga, jadwal });
    }

    file.close();
    return true;
}