#pragma once
#include "Card.h"
#include <vector>
#include <algorithm>
#include <iostream>

class Hand {
private:
    std::vector<Card> cards;
    int maxSize;

public:
    Hand(int maxSize = 8) : maxSize(maxSize) {}

    bool addCard(const Card& c) {
        if ((int)cards.size() >= maxSize) return false;
        cards.push_back(c);
        return true;
    }

    Card playCard(int index) {
        if (index < 0 || index >= (int)cards.size())
            throw std::out_of_range("Index tidak valid");
        Card c = cards[index];
        cards.erase(cards.begin() + index);
        return c;
    }

    std::vector<Card> playSelected(std::vector<int> indices) {
        std::sort(indices.rbegin(), indices.rend());
        std::vector<Card> result;
        for (int idx : indices) result.push_back(playCard(idx));
        std::reverse(result.begin(), result.end());
        return result;
    }

    void clear()      { cards.clear(); }
    int  size()  const { return (int)cards.size(); }
    bool empty() const { return cards.empty(); }
    bool isFull()const { return (int)cards.size() >= maxSize; }

    const std::vector<Card>& getCards() const { return cards; }

    void display() const {
        if (cards.empty()) { std::cout << "  [Tangan kosong]\n"; return; }
        for (int i = 0; i < (int)cards.size(); ++i)
            std::cout << "  [" << i << "] " << cards[i].toString() << "\n";
    }
};