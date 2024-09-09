#ifndef CARD_TEXT_H_INCLUDED
#define CARD_TEXT_H_INCLUDED

#include "FreeTypeHelper.h"

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
    static constexpr GLchar coinChar = 128; //character to represent coin icons. Any occurences of this character in card text will be replaced by the coin icon
    CardTextFont();
};


#endif // CARD_TEXT_H_INCLUDED
