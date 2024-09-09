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
    bearing =  {0,size.y};
    advance =  width*10;

}

CardTextFont::CardTextFont() : Font()
{
    Font::init<CardTextChar>(ResourcesConfig::config[ResourcesConfig::RESOURCES_DIR] + "/tnr.ttf");
    characters[coinChar] = std::unique_ptr<Character>(new CardTextChar(coinChar,"sprites/icons/coin.png"));

}
