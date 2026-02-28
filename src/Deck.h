#pragma once
#include "Card.h"
#include <vector>
#include <algorithm>
#include <random>
#include <stdexcept>

class Deck {
private:
    std::vector<Card> cards;
    std::vector<Card> discardPile;
    std::mt19937 rng;

public:
    Deck() : rng(std::random_device{}()) { reset(); }

    void reset() {
        cards.clear();
        discardPile.clear();
        for (auto suit : {Suit::HEARTS, Suit::DIAMONDS, Suit::CLUBS, Suit::SPADES})
            for (int r = 2; r <= 14; ++r)
                cards.emplace_back(suit, static_cast<Rank>(r));
    }

    void shuffle() {
        std::shuffle(cards.begin(), cards.end(), rng);
    }

    Card draw() {
        if (cards.empty()) reshuffleDiscard();
        if (cards.empty()) throw std::runtime_error("Deck kosong!");
        Card c = cards.back();
        cards.pop_back();
        return c;
    }

    std::vector<Card> drawMultiple(int count) {
        std::vector<Card> result;
        for (int i = 0; i < count && !cards.empty(); ++i)
            result.push_back(draw());
        return result;
    }

    void discard(const Card& c) { discardPile.push_back(c); }

    void discardAll(const std::vector<Card>& hand) {
        for (auto& c : hand) discardPile.push_back(c);
    }

    void reshuffleDiscard() {
        cards.insert(cards.end(), discardPile.begin(), discardPile.end());
        discardPile.clear();
        shuffle();
    }

    int remaining() const { return (int)cards.size(); }
    int discarded() const { return (int)discardPile.size(); }
};