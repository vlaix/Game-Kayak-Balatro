#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "Card.h"
#include "Deck.h"
#include "Hand.h"

void printDivider(const std::string& title = "") {
    std::cout << "\n========================================\n";
    if (!title.empty()) std::cout << "  " << title << "\n========================================\n";
}

void printStatus(const Deck& deck, int score) {
    std::cout << "\n  Score   : " << score << " chips"
              << "\n  Deck    : " << deck.remainingCards() << " kartu tersisa"
              << "\n  Discard : " << deck.discardCount() << " kartu\n";
}

// Parse input seperti "0 2 4" jadi vector {0, 2, 4}
std::vector<int> parseIndices(const std::string& input) {
    std::vector<int> result;
    std::istringstream ss(input);
    int n;
    while (ss >> n) result.push_back(n);
    return result;
}

int main() {
    printDivider("Balatro Copy  -  Terminal Edition");

    Deck deck;
    deck.shuffle();
    Hand hand(8);
    int totalScore = 0;
    int round = 1;

    // Draw hand awal
    auto initial = deck.drawHand(5);
    for (auto& c : initial) hand.addCard(c);

    while (true) {
        printDivider("ROUND " + std::to_string(round));
        printStatus(deck, totalScore);

        std::cout << "\nKartu di tangan kamu:\n";
        hand.display();

        std::cout << "\nPilih aksi:\n"
                  << "  [p] Main kartu\n"
                  << "  [d] Draw kartu baru\n"
                  << "  [q] Keluar\n"
                  << "\n> ";

        std::string aksi;
        std::getline(std::cin, aksi);

        // ── MAIN KARTU ──────────────────────────────────────
        if (aksi == "p") {
            if (hand.empty()) {
                std::cout << "\n  Tangan kosong! Draw dulu.\n";
                continue;
            }

            std::cout << "Pilih nomor kartu yang mau dimainkan (contoh: 0 2 3): ";
            std::string pilihanStr;
            std::getline(std::cin, pilihanStr);
            auto pilihan = parseIndices(pilihanStr);

            // Validasi index
            bool valid = true;
            for (int idx : pilihan) {
                if (idx < 0 || idx >= hand.size()) {
                    std::cout << "  [!] Index " << idx << " tidak valid!\n";
                    valid = false;
                    break;
                }
            }
            if (!valid || pilihan.empty()) continue;

            // Main kartu yang dipilih
            auto played = hand.playSelected(pilihan);
            int roundChips = 0;

            std::cout << "\nKartu yang dimainkan:\n";
            for (auto& c : played) {
                std::cout << "  -> " << c.toString()
                          << "  (+" << c.getBaseChips() << " chips)\n";
                roundChips += c.getBaseChips();
                deck.discard(c);
            }

            totalScore += roundChips;
            std::cout << "\n  +++ Dapat " << roundChips << " chips! "
                      << "Total: " << totalScore << " chips +++\n";
            round++;
        }

        // ── DRAW KARTU ──────────────────────────────────────
        else if (aksi == "d") {
            if (hand.isFull()) {
                std::cout << "\n  Tangan sudah penuh (maks 8 kartu)!\n";
                continue;
            }
            if (deck.isEmpty() && deck.discardCount() == 0) {
                std::cout << "\n  Tidak ada kartu tersisa!\n";
                continue;
            }

            std::cout << "Mau draw berapa kartu? (1-"
                      << (8 - hand.size()) << "): ";
            std::string drawStr;
            std::getline(std::cin, drawStr);

            int jumlah = 0;
            try { jumlah = std::stoi(drawStr); } catch (...) { jumlah = 1; }

            int bisa = std::min(jumlah, 8 - hand.size());
            std::cout << "\nDraw " << bisa << " kartu:\n";
            for (int i = 0; i < bisa; ++i) {
                Card c = deck.draw();
                hand.addCard(c);
                std::cout << "  << " << c.toString() << "\n";
            }
        }

        // ── KELUAR ──────────────────────────────────────────
        else if (aksi == "q") {
            printDivider("GAME OVER");
            std::cout << "\n  Total score akhir kamu: " << totalScore << " chips\n";
            std::cout << "  Dimainkan dalam " << (round - 1) << " ronde.\n\n";
            break;
        }

        else {
            std::cout << "  [!] Perintah tidak dikenal. Ketik p, d, atau q.\n";
        }
    }

    return 0;
}