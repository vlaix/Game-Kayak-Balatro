#pragma once
#include "Card.h"
#include <vector>
#include <algorithm>
#include <random>
#include <stdexcept>

class Deck {
private:
    std::vector<Card> cards;      // kartu yang belum di-draw
    std::vector<Card> discardPile; // kartu yang sudah dibuang
    std::mt19937 rng;

public:
    Deck() : rng(std::random_device{}()) {
        buildStandardDeck();
    }

    // Bangun deck 52 kartu standar
    void buildStandardDeck() {
        cards.clear();
        discardPile.clear();
        for (auto suit : {Suit::HEARTS, Suit::DIAMONDS, Suit::CLUBS, Suit::SPADES}) {
            for (int r = 2; r <= 14; ++r) {
                cards.emplace_back(suit, static_cast<Rank>(r));
            }
        }
    }

    // Shuffle deck (Fisher-Yates via std::shuffle)
    void shuffle() {
        std::shuffle(cards.begin(), cards.end(), rng);
    }

    // Draw 1 kartu dari atas deck
    Card draw() {
        if (cards.empty()) {
            reshuffleDiscard();
        }
        if (cards.empty()) {
            throw std::runtime_error("Deck kosong, tidak ada kartu untuk di-draw!");
        }
        Card drawn = cards.back();
        cards.pop_back();
        return drawn;
    }

    // Draw sejumlah kartu sekaligus (untuk hand awal)
    std::vector<Card> drawHand(int count = 5) {
        std::vector<Card> hand;
        for (int i = 0; i < count; ++i) {
            hand.push_back(draw());
        }
        return hand;
    }

    // Buang kartu ke discard pile
    void discard(const Card& card) {
        discardPile.push_back(card);
    }

    // Kocok ulang discard pile ke deck jika deck habis
    void reshuffleDiscard() {
        if (discardPile.empty()) return;
        cards.insert(cards.end(), discardPile.begin(), discardPile.end());
        discardPile.clear();
        shuffle();
    }

    int remainingCards() const { return static_cast<int>(cards.size()); }
    int discardCount()   const { return static_cast<int>(discardPile.size()); }
    bool isEmpty()       const { return cards.empty(); }
};