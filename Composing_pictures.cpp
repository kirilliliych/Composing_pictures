#include "Composing_pictures.hpp"

void RenewFPS(FPS *fps_struct)
{
    assert(fps_struct != nullptr);
    
    fps_struct->delay_time = fps_struct->delay_clock.getElapsedTime();

    if (fps_struct->delay_time.asSeconds() > fps_struct->FPS_delay)
    {
        fps_struct->cur_time = fps_struct->clock.getElapsedTime();

        sprintf(fps_struct->fps_str + FPS_STR_FPS_VALUE_POS, "%.2lf\n", 
                (1 / fps_struct->cur_time.asSeconds()));
        
        fps_struct->text.setString(fps_struct->fps_str);
        
        fps_struct->delay_clock.restart();

        fprintf(stderr, fps_struct->fps_str);
    }

    fps_struct->clock.restart(); 
}

int InitPictures(FPS *fps, sf::Image *background, sf::Image *dude,
                 picture *result_picture,
                 const unsigned char **background_pixels, 
                 const unsigned char **dude_pixels,
                 unsigned *result_picture_pixels)
{
    assert(fps                   != nullptr);
    assert(background            != nullptr);
    assert(dude                  != nullptr);
    assert(result_picture        != nullptr);
    assert(result_picture_pixels != nullptr);

    if (!fps->font.loadFromFile("CamingoMono-Regular.ttf"))
    {
        printf("Error: can't load CamingoMono-Regular.ttf\n");
        
        return 1;
    }
    fps->text.setFont(fps->font);
    fps->text.setCharacterSize(30);
    fps->text.setFillColor(sf::Color::Green);
    
    if (!background->loadFromFile("background.jpg"))
    {
        printf("Error, can't load background.jpg\n");
        
        return 1;
    }   
    *background_pixels = background->getPixelsPtr();
    
    if (!dude->loadFromFile("dude.png"))
    {
        printf("Error, can't load dude.png\n");

        return 1;
    }
    *dude_pixels = dude->getPixelsPtr();

    result_picture->texture.create(BACKGROUND_WIDTH, BACKGROUND_HEIGHT);
    result_picture->sprite.setTexture(result_picture->texture);

    memcpy(result_picture_pixels, *background_pixels, BACKGROUND_WIDTH * BACKGROUND_HEIGHT * 
           sizeof(unsigned));

    return 0;
}

void DoComposedPicture(const unsigned *background_pixels, const unsigned *dude_pixels, 
                       unsigned *result_picture_pixels)
{
    assert(background_pixels     != nullptr);
    assert(dude_pixels           != nullptr);
    assert(result_picture_pixels != nullptr);

    for (int y_pos = 0; y_pos < DUDE_HEIGHT; ++y_pos)
    {
        for (int x_pos = 0; x_pos < DUDE_WIDTH; x_pos += PIXELS_AT_ONCE)
        {
            __m128i bg_lo = _mm_loadu_si128((__m128i *) &background_pixels[(y_pos + DUDE_Y_POSITION) * 
                                                                            BACKGROUND_WIDTH +
                                                                            x_pos + DUDE_X_POSITION]);
            __m128i fg_lo = _mm_loadu_si128((__m128i *) &dude_pixels[     y_pos * DUDE_WIDTH + x_pos]);

            __m128i bg_hi = (__m128i) _mm_movehl_ps((__m128) ZEROS, (__m128) bg_lo);
            __m128i fg_hi = (__m128i) _mm_movehl_ps((__m128) ZEROS, (__m128) fg_lo);

            bg_lo = _mm_cvtepu8_epi16(bg_lo);
            bg_hi = _mm_cvtepu8_epi16(bg_hi);

            fg_lo = _mm_cvtepu8_epi16(fg_lo);
            fg_hi = _mm_cvtepu8_epi16(fg_hi);

            __m128i alpha_placing_mask = _mm_set_epi8(MSB, 0x0Eu, MSB, 0x0Eu, MSB, 0x0Eu, MSB, 0x0Eu,
                                                      MSB, 0x06u, MSB, 0x06u, MSB, 0x06u, MSB, 0x06u);

            __m128i alpha_lo = _mm_shuffle_epi8(fg_lo, alpha_placing_mask);
            __m128i alpha_hi = _mm_shuffle_epi8(fg_hi, alpha_placing_mask);

            fg_lo = _mm_mullo_epi16(fg_lo, alpha_lo);
            fg_hi = _mm_mullo_epi16(fg_hi, alpha_hi);

            bg_lo = _mm_mullo_epi16(bg_lo, _mm_sub_epi8(MAX_BYTE_VALUES, alpha_lo));
            bg_hi = _mm_mullo_epi16(bg_hi, _mm_sub_epi8(MAX_BYTE_VALUES, alpha_hi));

            __m128i sum_lo = _mm_add_epi16(bg_lo, fg_lo);
            __m128i sum_hi = _mm_add_epi16(bg_hi, fg_hi);

            __m128i sum_shr_mask = _mm_set_epi8(MSB,   MSB,   MSB,   MSB,   MSB,   MSB,   MSB,   MSB,
                                                0x0Fu, 0x0Du, 0x0Bu, 0x09u, 0x07u, 0x05u, 0x03u, 0x01u);

            sum_lo = _mm_shuffle_epi8(sum_lo, sum_shr_mask);
            sum_hi = _mm_shuffle_epi8(sum_hi, sum_shr_mask);

            __m128i final_pixels = (__m128i) _mm_movelh_ps((__m128) sum_lo, (__m128) sum_hi);

            _mm_storeu_si128((__m128i *) &result_picture_pixels[(y_pos + DUDE_Y_POSITION) * BACKGROUND_WIDTH + 
                                                                 x_pos + DUDE_X_POSITION], final_pixels);
        }
    }
}