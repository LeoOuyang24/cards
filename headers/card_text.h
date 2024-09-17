#ifndef CARD_TEXT_H_INCLUDED
#define CARD_TEXT_H_INCLUDED

#include "FreeTypeHelper.h"

#include "deck.h"

//character texture for each character in card text
//has all the fields of Character is more akin to a Sprite
//because it can have multiple colors
class CardTextChar : public Character
{
public:
    CardTextChar(char c, FT_Face& face);
    CardTextChar(char c, std::string spritePath);
};

class CardTextFont : public Font
{
public:
    static constexpr GLchar coinChar = '\x80'; //character to represent coin icons. Any occurences of this character in card text will be replaced by the coin icon
    static constexpr GLchar damageChar = coinChar + 1;
    static constexpr GLchar foodChar = damageChar + 1;
    //returns the card description for a card with resources
    static std::string getCardResourceString(const ResourceStats& stats);

    CardTextFont();


};


#endif // CARD_TEXT_H_INCLUDED
