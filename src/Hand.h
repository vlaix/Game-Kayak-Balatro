#pragma once
#include "Card.h"
#include <vector>
#include <iostream>

class Hand {
private:
    std::vector<Card> cards;
    int maxSize;

public:
    Hand(int maxSize = 8) : maxSize(maxSize) {}

    // Tambah kartu ke tangan
    bool addCard(const Card& card) {
        if (static_cast<int>(cards.size()) >= maxSize) {
            std::cout << "[Hand] Tangan penuh! Maksimal " << maxSize << " kartu.\n";
            return false;
        }
        cards.push_back(card);
        return true;
    }

    // Ambil kartu berdasarkan index (untuk dimainkan)
    // Mengembalikan kartu dan menghapusnya dari tangan
    Card playCard(int index) {
        if (index < 0 || index >= static_cast<int>(cards.size())) {
            throw std::out_of_range("Index kartu tidak valid!");
        }
        Card played = cards[index];
        cards.erase(cards.begin() + index);
        return played;
    }

    // Ambil semua kartu yang dipilih untuk dimainkan (berdasarkan indeks)
    std::vector<Card> playSelected(std::vector<int> indices) {
        // Sort descending supaya erase tidak geser index
        std::sort(indices.rbegin(), indices.rend());
        std::vector<Card> played;
        for (int idx : indices) {
            played.push_back(playCard(idx));
        }
        return played;
    }

    void clear() { cards.clear(); }

    int size() const { return static_cast<int>(cards.size()); }
    bool empty() const { return cards.empty(); }
    bool isFull() const { return static_cast<int>(cards.size()) >= maxSize; }

    const std::vector<Card>& getCards() const { return cards; }

    void display() const {
        if (cards.empty()) {
            std::cout << "  [Tangan kosong]\n";
            return;
        }
        for (int i = 0; i < static_cast<int>(cards.size()); ++i) {
            std::cout << "  [" << i << "] " << cards[i].toString()
                      << " (chips: " << cards[i].getBaseChips() << ")\n";
        }
    }
};