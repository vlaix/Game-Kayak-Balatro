#pragma once
#include <string>

enum class Suit { HEARTS, DIAMONDS, CLUBS, SPADES };
enum class Rank { TWO=2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE };

class Card {
public:
    Suit suit;
    Rank rank;

    Card(Suit s, Rank r) : suit(s), rank(r) {}

    std::string getSuitName() const {
        switch(suit) {
            case Suit::HEARTS:   return "H";
            case Suit::DIAMONDS: return "D";
            case Suit::CLUBS:    return "C";
            case Suit::SPADES:   return "S";
        }
        return "?";
    }

    std::string getRankName() const {
        switch(rank) {
            case Rank::TWO:   return "2";
            case Rank::THREE: return "3";
            case Rank::FOUR:  return "4";
            case Rank::FIVE:  return "5";
            case Rank::SIX:   return "6";
            case Rank::SEVEN: return "7";
            case Rank::EIGHT: return "8";
            case Rank::NINE:  return "9";
            case Rank::TEN:   return "10";
            case Rank::JACK:  return "J";
            case Rank::QUEEN: return "Q";
            case Rank::KING:  return "K";
            case Rank::ACE:   return "A";
        }
        return "?";
    }

    std::string toString() const {
        return "[" + getRankName() + getSuitName() + "]";
    }
};