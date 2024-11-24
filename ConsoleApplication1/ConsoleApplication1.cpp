#include <iostream>
#include <vector>
#include <windows.h>
#include <string>
#include <sstream>
#include <cstdlib>
#include <memory>

// Functie pentru a verifica daca un numar este prim
bool estePrim(int numar) {
    if (numar <= 1) return false;
    for (int i = 2; i * i <= numar; i++) {
        if (numar % i == 0) return false;
    }
    return true;
}

// Functie care va rula in procesele fiu si va cauta numere prime in intervalul specificat
void cautaPrimeInInterval(int start, int end, HANDLE writePipe) {
    std::vector<int> prime;
    for (int i = start; i <= end; ++i) {
        if (estePrim(i)) {
            prime.push_back(i);
        }
    }

    // Scrie rezultatele in pipe
    for (int p : prime) {
        std::string primStr = std::to_string(p) + " ";
        WriteFile(writePipe, primStr.c_str(), primStr.size(), nullptr, nullptr);
    }
}

int main() {
    const int numereMaxim = 10000;
    const int intervale = 10;
    const int intervalSize = numereMaxim / intervale;

    // Creaza pipe-uri
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    HANDLE pipe[2];
    if (!CreatePipe(&pipe[0], &pipe[1], &sa, 0)) {
        std::cerr << "Eroare la crearea pipe-ului!" << std::endl;
        return 1;
    }

    // Creaza procesele fiu
    for (int i = 0; i < intervale; ++i) {
        int start = i * intervalSize + 1;
        int end = (i + 1) * intervalSize;

        // Construieste comanda pentru a lansa procesul fiu
        std::ostringstream cmd;
        cmd << "start /B ";
        cmd << "cmd /C \"echo searching prime numbers in range " << start << " to " << end << " && ";
        cmd << "C:\\Users\\stanc\\source\\repos\\ConsoleApplication1\\ConsoleApplication1\\ConsoleApplication1.cpp"  << start << " " << end << "\"";

        // Lanseaza procesul fiu
        system(cmd.str().c_str());
    }

    // Citeste rezultatele de la pipe
    char buffer[128];
    while (true) {
        DWORD bytesRead;
        if (ReadFile(pipe[0], buffer, sizeof(buffer) - 1, &bytesRead, nullptr) == FALSE) {
            break;
        }

        if (bytesRead == 0) {
            break;
        }

        buffer[bytesRead] = '\0';
        std::cout << buffer; // Afiseaza numerele prime
    }

    // Inchide pipe-ul
    CloseHandle(pipe[0]);
    CloseHandle(pipe[1]);

    return 0;
}
