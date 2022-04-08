#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

//#define PICTURE_SHOWING
#define FPS_CHECKING

const int FPS_STR_MAX_SIZE      = 16;
const int FPS_STR_FPS_VALUE_POS = 5;
const int COLOR_PIXELS          = 3;
const int BYTE_MAX              = 255;

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

    double FPS_sum    = 0;
    int frames_saved  = 0;

    char fps_str[FPS_STR_MAX_SIZE] = "FPS: 0000.00\n";
};

const unsigned BACKGROUND_WIDTH           = 604;
const unsigned BACKGROUND_HEIGHT          = 453;
const unsigned DUDE_WIDTH                 = 124;
const unsigned DUDE_HEIGHT                = 360;
const unsigned PATTINSON_WIDTH            = 104;
const unsigned PATTINSON_HEIGHT           = 298;

const unsigned DUDE_X_POSITION            = 0;
const unsigned DUDE_Y_POSITION            = 0;
const unsigned PATTINSON_X_POSITION       = 350;
const unsigned PATTINSON_Y_POSITION       = 100;


void RenewFPS(FPS *fps_struct)
{
    assert(fps_struct != nullptr);
    
    fps_struct->delay_time   = fps_struct->delay_clock.getElapsedTime();
    fps_struct->cur_time     = fps_struct->clock.getElapsedTime();
    fps_struct->FPS_sum     += 1 / fps_struct->cur_time.asSeconds();
    ++fps_struct->frames_saved;

    if (fps_struct->delay_time.asSeconds() > fps_struct->FPS_delay)
    {
        sprintf(fps_struct->fps_str + FPS_STR_FPS_VALUE_POS, "%.2lf\n", 
                fps_struct->FPS_sum / fps_struct->frames_saved);
        
        fps_struct->text.setString(fps_struct->fps_str);
        
        fps_struct->delay_clock.restart();

        fps_struct->FPS_sum = 0;
        fps_struct->frames_saved = 0;

#ifdef FPS_CHECKING
        fprintf(stderr, "%s", fps_struct->fps_str); 
#endif
    }

    fps_struct->clock.restart();
}

void DoComposedPicture(const unsigned char *pixels_1, const unsigned char *pixels_2, 
                       unsigned char *result_picture_pixels, const unsigned width, 
                       const unsigned height, const unsigned x_position,
                       const unsigned y_position, const unsigned pixels_1_width)
{
    assert(pixels_1              != nullptr);
    assert(pixels_2              != nullptr);
    assert(result_picture_pixels != nullptr);

    for (int y_pos = 0; y_pos < height; ++y_pos)
    {
        for (int x_pos = 0; x_pos < width; ++x_pos)
        {
            unsigned char alpha = pixels_2[(y_pos * width + x_pos) * sizeof(unsigned) + COLOR_PIXELS];
            
            for (int color_index = 0; color_index < COLOR_PIXELS; ++color_index)
            {
                result_picture_pixels[((y_pos + y_position)    * pixels_1_width + 
                                        x_pos + x_position)    * sizeof(unsigned) + color_index] = 
                            (pixels_1[((y_pos + y_position)    * pixels_1_width + 
                                        x_pos + x_position)    * sizeof(unsigned) + color_index] * (BYTE_MAX - alpha) + 
                             pixels_2[ (y_pos * width + x_pos) * sizeof(unsigned) + color_index] * alpha) / BYTE_MAX;
            }
        }
    }    
}

const unsigned char *InitImage(sf::Image *image, const char *file_name)
{
    assert(image     != nullptr);
    assert(file_name != nullptr);

    if (!image->loadFromFile(file_name))
    {
        printf("Error, can't load %s\n", file_name);
    }

    return image->getPixelsPtr(); 
}

int InitPictures(FPS *fps, sf::Image *background, sf::Image *dude, sf::Image *pattinson,
                 picture *result_picture,
                 const unsigned char **background_pixels, 
                 const unsigned char **dude_pixels,
                 const unsigned char **pattinson_pixels,
                 unsigned *result_picture_pixels)
{
    assert(fps                   != nullptr);
    assert(background            != nullptr);
    assert(dude                  != nullptr);
    assert(pattinson             != nullptr);
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
    fps->text.setPosition(350.f, 0.f);
    
    *background_pixels = InitImage(background, "background.jpg");
    *dude_pixels       = InitImage(dude,       "dude.png");
    *pattinson_pixels  = InitImage(pattinson,  "pattinson.png");

    result_picture->texture.create(BACKGROUND_WIDTH, BACKGROUND_HEIGHT);
    result_picture->sprite.setTexture(result_picture->texture);

    memcpy(result_picture_pixels, *background_pixels, BACKGROUND_WIDTH * BACKGROUND_HEIGHT * 
           sizeof(unsigned));

    return 0;
}


int main()
{   
    FPS fps;
    sf::Image background;
    sf::Image dude;
    sf::Image pattinson;
    const unsigned char *background_pixels = nullptr;
    const unsigned char *dude_pixels       = nullptr;
    const unsigned char *pattinson_pixels  = nullptr;
    picture result_picture;
    unsigned *result_picture_pixels = (unsigned *) calloc(BACKGROUND_WIDTH * BACKGROUND_HEIGHT,  
                                                          sizeof(unsigned));

    InitPictures(&fps, &background, &dude, &pattinson, &result_picture, &background_pixels,
                 &dude_pixels, &pattinson_pixels, result_picture_pixels);

#ifdef FPS_CHECKING
    for (int i = 0; i < 100000; ++i)
    {
        DoComposedPicture((unsigned char *) result_picture_pixels, dude_pixels, 
                          (unsigned char *) result_picture_pixels, DUDE_WIDTH, DUDE_HEIGHT, 
                           DUDE_X_POSITION, DUDE_Y_POSITION, BACKGROUND_WIDTH);
        DoComposedPicture((unsigned char *) result_picture_pixels, pattinson_pixels,
                          (unsigned char *) result_picture_pixels, PATTINSON_WIDTH, PATTINSON_HEIGHT, 
                          PATTINSON_X_POSITION, PATTINSON_Y_POSITION, BACKGROUND_WIDTH);
        RenewFPS(&fps);
    }
#endif

#ifdef PICTURE_SHOWING
    sf::RenderWindow window(sf::VideoMode(BACKGROUND_WIDTH, BACKGROUND_HEIGHT), 
                            "Composing_pictures", sf::Style::Default);  

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        DoComposedPicture((unsigned char *) result_picture_pixels, dude_pixels, 
                          (unsigned char *) result_picture_pixels, DUDE_WIDTH, DUDE_HEIGHT, 
                          DUDE_X_POSITION, DUDE_Y_POSITION, BACKGROUND_WIDTH);
        DoComposedPicture((unsigned char *) result_picture_pixels, pattinson_pixels,
                          (unsigned char *) result_picture_pixels, PATTINSON_WIDTH, PATTINSON_HEIGHT, 
                          PATTINSON_X_POSITION, PATTINSON_Y_POSITION, BACKGROUND_WIDTH);
                          
        result_picture.texture.update((const unsigned char *) result_picture_pixels);
        
        RenewFPS(&fps);
        
        window.clear();
        
        window.draw(result_picture.sprite);
        window.draw(fps.text);

        window.display();
    }
#endif

    free(result_picture_pixels);

    return 0;
}