#pragma once
#include "Deck.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

// ─── State loop ───────────────────────────────────────────
enum class GameState {
    PLAYING,   // lagi di dalam round, mainin kartu
    SHOP,      // antara round, di toko
    GAME_OVER  // run selesai (menang/kalah)
};

// ─── RunSession ───────────────────────────────────────────
class RunSession {
private:
    // --- Data game ---
    Deck deck;
    std::vector<Card> hand;       // kartu di tangan sekarang
    GameState state;

    int currentRound;
    int maxRounds;

    int handsLeft;                // berapa kali boleh main tangan per round
    int discardsLeft;             // berapa kali boleh discard per round

    int roundScore;               // score yang terkumpul di round ini
    int targetScore;              // target yang harus dicapai di round ini
    int totalScore;               // score keseluruhan run

    int gold;                     // uang untuk beli di shop
    int handSize;                 // berapa kartu di tangan (default 8)

    // --- Konstanta per round ---
    static const int HANDS_PER_ROUND    = 4;
    static const int DISCARDS_PER_ROUND = 3;
    static const int BASE_TARGET        = 300;
    static const int TARGET_SCALE       = 200; // tambahan target per round

public:
    RunSession()
        : state(GameState::PLAYING),
          currentRound(1), maxRounds(8),
          handsLeft(HANDS_PER_ROUND), discardsLeft(DISCARDS_PER_ROUND),
          roundScore(0), targetScore(BASE_TARGET),
          totalScore(0), gold(4), handSize(8)
    {
        deck.shuffle();
        dealHand();
    }

    // ─── Entry point ───────────────────────────────────────
    void run() {
        printWelcome();
        while (state != GameState::GAME_OVER) {
            switch (state) {
                case GameState::PLAYING:  playingLoop(); break;
                case GameState::SHOP:     shopLoop();    break;
                default: break;
            }
        }
        printGameOver();
    }

private:
    // ══════════════════════════════════════════════════════
    //  PLAYING STATE
    // ══════════════════════════════════════════════════════
    void playingLoop() {
        while (state == GameState::PLAYING) {
            printRoundHeader();
            printHand();
            printPlayingMenu();

            std::string input;
            std::getline(std::cin, input);

            if      (input == "p") doPlayHand();
            else if (input == "d") doDiscard();
            else if (input == "q") { state = GameState::GAME_OVER; }
            else    std::cout << "  [!] Perintah tidak dikenal.\n";
        }
    }

    void doPlayHand() {
        if (hand.empty()) { std::cout << "\n  Tangan kosong!\n"; return; }
        if (handsLeft <= 0) { std::cout << "\n  Tidak ada sisa tangan!\n"; return; }

        auto indices = pickCards("Pilih 1-5 kartu untuk dimainkan");
        if (indices.empty()) return;
        if (indices.size() > 5) {
            std::cout << "  [!] Maksimal 5 kartu per tangan!\n"; return;
        }

        // Ambil kartu yang dipilih
        std::vector<Card> played = extractCards(indices);

        // Tampilkan kartu yang dimainkan
        std::cout << "\n  Kartu dimainkan: ";
        for (auto& c : played) std::cout << c.toString() << " ";
        std::cout << "\n";

        // Placeholder score (nanti diganti ScoringSystem)
        int score = 100; // dummy score
        roundScore += score;
        handsLeft--;

        std::cout << "  Score ronde ini: " << roundScore
                  << " / " << targetScore << "\n";

        // Buang kartu yang dimainkan ke discard
        for (auto& c : played) deck.discard(c);

        // Refill tangan
        refillHand();

        // Cek apakah target tercapai
        if (roundScore >= targetScore) {
            std::cout << "\n  *** TARGET TERCAPAI! Lanjut ke shop... ***\n";
            gold += 5; // dapat gold tiap round menang
            enterShop();
            return;
        }

        // Cek apakah tangan habis (kalah)
        if (handsLeft <= 0) {
            std::cout << "\n  !! Tangan habis. Score tidak cukup. Game Over! !!\n";
            state = GameState::GAME_OVER;
        }
    }

    void doDiscard() {
        if (discardsLeft <= 0) { std::cout << "\n  Tidak ada sisa discard!\n"; return; }
        if (hand.empty())      { std::cout << "\n  Tangan kosong!\n"; return; }

        auto indices = pickCards("Pilih kartu yang mau di-discard");
        if (indices.empty()) return;

        std::vector<Card> discarded = extractCards(indices);
        std::cout << "\n  Di-discard: ";
        for (auto& c : discarded) std::cout << c.toString() << " ";
        std::cout << "\n";

        for (auto& c : discarded) deck.discard(c);
        discardsLeft--;

        // Refill tangan
        refillHand();
    }

    // ══════════════════════════════════════════════════════
    //  SHOP STATE
    // ══════════════════════════════════════════════════════
    void shopLoop() {
        while (state == GameState::SHOP) {
            printShop();
            printShopMenu();

            std::string input;
            std::getline(std::cin, input);

            if (input == "n") {
                nextRound();
            } else if (input == "q") {
                state = GameState::GAME_OVER;
            } else {
                std::cout << "  [!] Perintah tidak dikenal.\n";
            }
        }
    }

    void enterShop() {
        totalScore += roundScore;
        state = GameState::SHOP;
    }

    void nextRound() {
        currentRound++;
        if (currentRound > maxRounds) {
            std::cout << "\n  *** SELAMAT! Kamu menyelesaikan semua round! ***\n";
            state = GameState::GAME_OVER;
            return;
        }

        // Reset untuk round baru
        roundScore    = 0;
        handsLeft     = HANDS_PER_ROUND;
        discardsLeft  = DISCARDS_PER_ROUND;
        targetScore   = BASE_TARGET + (currentRound - 1) * TARGET_SCALE;

        // Reset deck dan deal ulang
        deck.reset();
        deck.shuffle();
        hand.clear();
        dealHand();

        state = GameState::PLAYING;
        std::cout << "\n  === Memulai Round " << currentRound << " ===\n";
        std::cout << "  Target: " << targetScore << " chips\n\n";
    }

    // ══════════════════════════════════════════════════════
    //  HELPERS
    // ══════════════════════════════════════════════════════
    void dealHand() {
        auto drawn = deck.drawMultiple(handSize);
        hand.insert(hand.end(), drawn.begin(), drawn.end());
    }

    void refillHand() {
        int need = handSize - (int)hand.size();
        if (need <= 0) return;
        auto drawn = deck.drawMultiple(need);
        hand.insert(hand.end(), drawn.begin(), drawn.end());
        if (!drawn.empty()) {
            std::cout << "  Draw " << drawn.size() << " kartu baru: ";
            for (auto& c : drawn) std::cout << c.toString() << " ";
            std::cout << "\n";
        }
    }

    // Minta user pilih kartu berdasarkan index, return sorted descending
    std::vector<int> pickCards(const std::string& prompt) {
        std::cout << "  " << prompt << " (contoh: 0 1 3): ";
        std::string line;
        std::getline(std::cin, line);

        std::istringstream ss(line);
        std::vector<int> indices;
        int n;
        while (ss >> n) {
            if (n >= 0 && n < (int)hand.size())
                indices.push_back(n);
            else
                std::cout << "  [!] Index " << n << " diabaikan.\n";
        }

        // Hapus duplikat
        std::sort(indices.begin(), indices.end());
        indices.erase(std::unique(indices.begin(), indices.end()), indices.end());

        return indices;
    }

    // Cabut kartu dari tangan berdasarkan indices (descending agar index tidak geser)
    std::vector<Card> extractCards(std::vector<int> indices) {
        std::sort(indices.rbegin(), indices.rend());
        std::vector<Card> result;
        for (int idx : indices) {
            result.push_back(hand[idx]);
            hand.erase(hand.begin() + idx);
        }
        // Balik urutan biar sesuai urutan pilihan user
        std::reverse(result.begin(), result.end());
        return result;
    }

    // ══════════════════════════════════════════════════════
    //  DISPLAY
    // ══════════════════════════════════════════════════════
    void printWelcome() {
        std::cout << "\n";
        std::cout << "  ╔══════════════════════════════════╗\n";
        std::cout << "  ║       CARD-RUN GAME v0.1         ║\n";
        std::cout << "  ║   Poker Hand Edition - Terminal  ║\n";
        std::cout << "  ╚══════════════════════════════════╝\n";
        std::cout << "\n  Total " << maxRounds << " round. Capai target tiap round!\n";
        std::cout << "  Round 1 target: " << targetScore << " chips\n\n";
    }

    void printRoundHeader() {
        std::cout << "\n  ┌─────────────────────────────────────┐\n";
        std::cout << "  │  Round " << currentRound << " / " << maxRounds;
        std::cout << "   |  Target: " << targetScore << " chips\n";
        std::cout << "  │  Score : " << roundScore << " / " << targetScore;
        std::cout << "   |  Gold: " << gold << "G\n";
        std::cout << "  │  Tangan: " << handsLeft << "   Discard: " << discardsLeft;
        std::cout << "   |  Deck: " << deck.remaining() << " kartu\n";
        std::cout << "  └─────────────────────────────────────┘\n";
    }

    void printHand() {
        std::cout << "\n  Kartu di tangan:\n  ";
        for (int i = 0; i < (int)hand.size(); ++i) {
            std::cout << "[" << i << "]" << hand[i].toString() << " ";
        }
        std::cout << "\n";
    }

    void printPlayingMenu() {
        std::cout << "\n  [p] Main kartu   [d] Discard   [q] Quit\n  > ";
    }

    void printShop() {
        std::cout << "\n  ╔══════════════════════════════════╗\n";
        std::cout << "  ║              S H O P             ║\n";
        std::cout << "  ╚══════════════════════════════════╝\n";
        std::cout << "  Gold kamu: " << gold << "G\n";
        std::cout << "  Total score: " << totalScore << " chips\n";
        std::cout << "\n  (Shop item akan tersedia di update berikutnya)\n";
    }

    void printShopMenu() {
        std::cout << "\n  [n] Lanjut ke round berikutnya   [q] Quit\n  > ";
    }

    void printGameOver() {
        std::cout << "\n  ╔══════════════════════════════════╗\n";
        std::cout << "  ║           G A M E  O V E R       ║\n";
        std::cout << "  ╚══════════════════════════════════╝\n";
        std::cout << "  Round dicapai : " << currentRound << " / " << maxRounds << "\n";
        std::cout << "  Total score   : " << totalScore << " chips\n\n";
    }
};