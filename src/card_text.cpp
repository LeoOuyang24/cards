#include "resourcesMaster.h"

#include "../headers/card_text.h"

CardTextChar::CardTextChar(char c, FT_Face& face) : Character(c,face)
{
    //shamelessly stolen from https://www.khronos.org/opengl/wiki/Texture#Swizzle_mask
    //this causes the Red channel to
    GLint swizzleMask[] = {GL_ZERO, GL_ZERO, GL_ZERO, GL_RED};
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);

}

CardTextChar::CardTextChar(char c, std::string spritePath) : Character(c)
{
    Sprite::load(spritePath);

     size =     glm::ivec2(width/5, height/5);
    bearing =  {10,size.y};
    advance =  width*10;

}


std::string CardTextFont::getCardResourceString(const ResourceStats& stats)
{
    return (stats.coins > 0 ? (std::to_string(stats.coins) + coinChar) : "") +
            (stats.damage > 0 ? ((stats.coins > 0 ? "\n" : "") + std::to_string(stats.damage) + damageChar) : "") +
            (stats.food > 0 ? ((stats.coins > 0 || stats.damage > 0 ? "\n" : "") + std::to_string(stats.food) + foodChar) : "");
}

CardTextFont::CardTextFont() : Font()
{
    Font::init<CardTextChar>("fonts/Cryptik.ttf");
    characters[coinChar] = std::unique_ptr<Character>(new CardTextChar(coinChar,"sprites/icons/coin.png"));
    characters[damageChar] = std::unique_ptr<Character>(new CardTextChar(damageChar,"sprites/icons/damage.png"));
    characters[foodChar] = std::unique_ptr<Character>(new CardTextChar(damageChar,"sprites/icons/food.png"));


}
