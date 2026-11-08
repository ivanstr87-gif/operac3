#define NOMINMAX
#undef UNICODE
#undef _UNICODE

#include <iostream>
#include <windows.h>
#include <string>

using namespace std;

struct Receipt {
    char brand[50];
    char date[15];
    int status;
};

void WriteStringToFile(HANDLE hFile, const string& str) {
    DWORD bytesWritten;
    WriteFile(hFile, str.c_str(), (DWORD)str.length(), &bytesWritten, NULL);
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    cout << "========================================================" << endl;
    cout << "LAB 3. VARIANT 13 - Radioatelier" << endl;
    cout << "========================================================" << endl;

    char choice;
    cout << "\nVvedite 'q' chtoby prosto prochitat' binarniy fayl," << endl;
    cout << "ili lyuboy drugoy simvol dlya vvoda novyh dannyh: ";
    cin >> choice;

    if (choice == 'q' || choice == 'Q') {
        cout << "\n[REZHIM CHTENIYA] Otkryvaem receipts_data.bin..." << endl;

        HANDLE hReadFile = CreateFileA(
            "receipts_data.bin",
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (hReadFile == INVALID_HANDLE_VALUE) {
            cout << " Oshibka otkrytiya fayla: " << GetLastError() << endl;
            cout << " Vozmozhno, fayl eshche ne sozdan (net dannyh)." << endl;
            system("pause");
            return 1;
        }

        Receipt tempReceipt;
        DWORD bytesRead;
        int count = 0;

        cout << "\n--- Soderzhimoe binarnogo fayla ---" << endl;

        while (ReadFile(hReadFile, &tempReceipt, sizeof(Receipt), &bytesRead, NULL) && bytesRead == sizeof(Receipt)) {
            count++;
            cout << "Zapis #" << count << ":" << endl;
            cout << "  Marka:  " << tempReceipt.brand << endl;
            cout << "  Data:   " << tempReceipt.date << endl;
            cout << "  Status: " << (tempReceipt.status == 1 ? "vypolnen" : "ne vypolnen") << endl;
            cout << "-------------------------" << endl;
        }

        if (count == 0) {
            cout << "Fayl pust ili ne soderzhit zapisey." << endl;
        }
        else {
            cout << "\nVsego prochitano zapisey: " << count << endl;
        }

        CloseHandle(hReadFile);
    }
    else {
        int n;
        cout << "\nVvedite kolichestvo kvitanciy: ";
        cin >> n;
        cin.ignore(1000, '\n');

        Receipt* receipts = new Receipt[n];

        for (int i = 0; i < n; i++) {
            cout << "\n--- Kvitanciya #" << i + 1 << " ---" << endl;

            cout << "Marka televizora: ";
            cin.getline(receipts[i].brand, 50);

            cout << "Data priyomki (DD.MM.GGGG): ";
            cin.getline(receipts[i].date, 15);

            cout << "Sostoyanie (0 - ne vypolnen, 1 - vypolnen): ";
            cin >> receipts[i].status;
            cin.ignore(1000, '\n');
        }

        cout << "\n Zapisi v fayl..." << endl;

        HANDLE hBinFile = CreateFileA(
            "receipts_data.bin",
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (hBinFile == INVALID_HANDLE_VALUE) {
            cout << " CreateFile failed: " << GetLastError() << endl;
            delete[] receipts;
            system("pause");
            return 1;
        }

        DWORD bytesWritten;
        WriteFile(hBinFile, receipts, n * sizeof(Receipt), &bytesWritten, NULL);
        FlushFileBuffers(hBinFile);
        CloseHandle(hBinFile);
        delete[] receipts;

        cout << " Zapisano bayt: " << bytesWritten << endl;

        HANDLE hReadFile = CreateFileA(
            "receipts_data.bin",
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        HANDLE hTxtFile = CreateFileA(
            "result_report.txt",
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        string header = "=== Nevypolnennye zakazy ===\r\n";
        cout << "\n" << header;
        WriteStringToFile(hTxtFile, header);

        Receipt tempReceipt;
        DWORD bytesRead;
        bool found = false;

        while (ReadFile(hReadFile, &tempReceipt, sizeof(Receipt), &bytesRead, NULL) && bytesRead > 0) {
            cout << " Prochitano: " << tempReceipt.brand
                << " | " << tempReceipt.date
                << " | status=" << tempReceipt.status << endl;

            if (tempReceipt.status == 0) {
                found = true;
                string outStr = string("Marka: ") + tempReceipt.brand +
                    " | Data: " + tempReceipt.date +
                    " | Status: NE VYPOLNEN\r\n";
                cout << "  -> " << outStr;
                WriteStringToFile(hTxtFile, outStr);
            }
        }

        if (!found) {
            string msg = "Nevypolnennyh zakazov net.\r\n";
            cout << msg;
            WriteStringToFile(hTxtFile, msg);
        }

        CloseHandle(hReadFile);
        CloseHandle(hTxtFile);

        cout << "\n[SUCCESS] Otchet sohranen v result_report.txt" << endl;
    }

    cout << "\n[SUCCESS] Rabota zavershena." << endl;
    system("pause");
    return 0;
}