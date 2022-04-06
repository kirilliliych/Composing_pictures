#ifndef COMPOSING_PICTURES_HPP
#define COMPOSING_PICTURES_HPP

#include <assert.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include <stdio.h>
#include <string.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

const int FPS_STR_MAX_SIZE                = 16;
const int FPS_STR_FPS_VALUE_POS           = 5;
const int PIXELS_AT_ONCE                  = 4;

struct picture
{
    sf::Texture texture; 
    sf::Sprite  sprite;
};

struct FPS
{
    sf::Font font;
    sf::Text text;

    sf::Clock clock;
    sf::Time  cur_time   = clock.getElapsedTime();

    sf::Clock delay_clock;
    sf::Time  delay_time = delay_clock.getElapsedTime();

    float FPS_delay = 0.2f;

    char fps_str[FPS_STR_MAX_SIZE] = "FPS: 0000.00\n";
};

const unsigned BACKGROUND_WIDTH           = 604;
const unsigned BACKGROUND_HEIGHT          = 453;
const unsigned DUDE_WIDTH                 = 124;
const unsigned DUDE_HEIGHT                = 360;

const unsigned DUDE_X_POSITION            = 350;
const unsigned DUDE_Y_POSITION            = 80;

const unsigned char MAX                   = 255u;
const unsigned char MSB                   = 0x80u;

const __m128i ZEROS           = _mm_set_epi8(0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0);
const __m128i MAX_BYTE_VALUES = _mm_cvtepu8_epi16(_mm_set_epi8(MAX, MAX, MAX, MAX,  MAX, MAX, MAX, MAX,  
                                                               MAX, MAX, MAX, MAX,  MAX, MAX, MAX, MAX)); 

void RenewFPS(FPS *fps_struct);

int InitPictures(FPS *fps, sf::Image *background, sf::Image *dude,
                 picture *result_picture,
                 const unsigned char **background_pixels, 
                 const unsigned char **dude_pixels,
                 unsigned *result_picture_pixels);

void DoComposedPicture(const unsigned *background_pixels, const unsigned *dude_pixels, 
                       unsigned *result_picture_pixels);

#endif