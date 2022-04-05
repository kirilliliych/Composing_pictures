#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

const int FPS_STR_MAX_SIZE      = 16;
const int FPS_STR_FPS_VALUE_POS = 5;
const int MAX_FILE_NAME_SIZE    = 256;
const int COLOR_PIXELS          = 3;

struct picture
{
    sf::Texture texture; 
    sf::Sprite  sprite;
};

struct FPS
{
    sf::Font font;
    sf::Text text;

    clock_t cur_time  = clock();
    clock_t prev_time = 0;

    clock_t FPS_delay = CLOCKS_PER_SEC / 100;

    char fps_str[FPS_STR_MAX_SIZE] = "FPS: 000.0";
};

const unsigned BACKGROUND_WIDTH           = 800;
const unsigned BACKGROUND_HEIGHT          = 600;
const unsigned KITTY_UWU_WIDTH            = 235;
const unsigned KITTY_UWU_HEIGHT           = 126;

const unsigned KITTY_UWU_X_POSITION       = 300;
const unsigned KITTY_UWU_Y_POSITION       = 220;


unsigned Reverse(unsigned x)
{
    x = (x & 0x00FF00FF) <<  8 | (x & 0xFF00FF00) >>  8;
    x = (x & 0x0000FFFF) << 16 | (x & 0xFFFF0000) >> 16;

    return x;
}

void RenewFPS(FPS *fps_struct)
{
    assert(fps_struct != nullptr);

    fps_struct->cur_time = clock();
    if (fps_struct->cur_time - fps_struct->prev_time > fps_struct->FPS_delay)
    {
        sprintf(fps_struct->fps_str + FPS_STR_FPS_VALUE_POS, "%.1lf\n", 
                ((float) CLOCKS_PER_SEC / (fps_struct->cur_time - fps_struct->prev_time)));
        
        fps_struct->text.setString(fps_struct->fps_str);
    }
    fps_struct->prev_time = fps_struct->cur_time;
}

void DoComposedPicture(unsigned char *background_pixels, unsigned char *kitty_uwu_pixels, unsigned char *result_picture_pixels)
{
    assert(background_pixels     != nullptr);
    assert(kitty_uwu_pixels      != nullptr);
    assert(result_picture_pixels != nullptr);

    for (int y_pos = 0; y_pos < KITTY_UWU_HEIGHT; ++y_pos)
    {
        for (int x_pos = 0; x_pos < KITTY_UWU_WIDTH; ++x_pos)
        {
            unsigned char alpha = kitty_uwu_pixels[(y_pos * KITTY_UWU_WIDTH + x_pos) * sizeof(unsigned) + COLOR_PIXELS];
            
            for (int color_index = 0; color_index < COLOR_PIXELS; ++color_index)
            {
                result_picture_pixels[((y_pos + KITTY_UWU_Y_POSITION)    * BACKGROUND_WIDTH + 
                                        x_pos + KITTY_UWU_X_POSITION)    * sizeof(unsigned) + color_index] = 
                   (background_pixels[((y_pos + KITTY_UWU_Y_POSITION)    * BACKGROUND_WIDTH + 
                                        x_pos + KITTY_UWU_X_POSITION)    * sizeof(unsigned) + color_index] * (255 - alpha) + 
                     kitty_uwu_pixels[ (y_pos * KITTY_UWU_WIDTH + x_pos) * sizeof(unsigned) + color_index] * alpha) / 255;
            }
        }
    }    
}

void FillPixelsArray(sf::Image *image, unsigned *pixels, const unsigned width, const unsigned height)
{
    assert(image  != nullptr);
    assert(pixels != nullptr);

    for (unsigned y_pos = 0; y_pos < height; ++y_pos)
    {
        for (unsigned x_pos = 0; x_pos < width; ++x_pos)
        {
            pixels[y_pos * width + x_pos] = Reverse(image->getPixel(x_pos, y_pos).toInteger());
        }
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(BACKGROUND_WIDTH, BACKGROUND_HEIGHT), "Composing_pictures", sf::Style::Default);     

    FPS fps;
    if (!fps.font.loadFromFile("CamingoMono-Regular.ttf"))
    {
        printf("Error: can't load font\n");
        
        return 1;
    }
    fps.text.setFont(fps.font);
    fps.text.setCharacterSize(30);
    fps.text.setFillColor(sf::Color::Green);

    sf::Image background;
    if (!background.loadFromFile("Table.bmp"))
    {
        printf("Error, can't load Table.bmp\n");
        
        return 1;
    }   

    unsigned *background_pixels     = (unsigned *) calloc(BACKGROUND_WIDTH * BACKGROUND_HEIGHT,  sizeof(unsigned));
    FillPixelsArray(&background, background_pixels, BACKGROUND_WIDTH, BACKGROUND_HEIGHT);

    sf::Image kitty_uwu;
    if (!kitty_uwu.loadFromFile("AskhatCat.bmp"))
    {
        printf("Error, can't load AskhatCat.bmp\n");

        return 1;
    }
    unsigned *kitty_uwu_pixels      = (unsigned *) calloc(KITTY_UWU_WIDTH  * KITTY_UWU_HEIGHT,   sizeof(unsigned));
    FillPixelsArray(&kitty_uwu, kitty_uwu_pixels, KITTY_UWU_WIDTH, KITTY_UWU_HEIGHT);
    
    picture result_picture;
    result_picture.texture.create(BACKGROUND_WIDTH, BACKGROUND_HEIGHT);
    result_picture.sprite.setTexture(result_picture.texture);

    unsigned *result_picture_pixels = (unsigned *) calloc(BACKGROUND_WIDTH  * BACKGROUND_HEIGHT, sizeof(unsigned));
    FillPixelsArray(&background, result_picture_pixels, BACKGROUND_WIDTH, BACKGROUND_HEIGHT);

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        DoComposedPicture((unsigned char *) background_pixels, (unsigned char *) kitty_uwu_pixels, (unsigned char *) result_picture_pixels);
        result_picture.texture.update((const uint8_t *) result_picture_pixels);
        
        RenewFPS(&fps);
        
        window.clear();
        
        window.draw(result_picture.sprite);
        window.draw(fps.text);

        window.display();
    }

    free(background_pixels);
    free(kitty_uwu_pixels);
    free(result_picture_pixels);

    return 0;
}