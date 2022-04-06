#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
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
const unsigned PATTINSON_WIDTH            = 104;
const unsigned PATTINSON_HEIGHT           = 298;

const unsigned DUDE_X_POSITION            = 0;
const unsigned DUDE_Y_POSITION            = 0;
const unsigned PATTINSON_X_POSITION       = 350;
const unsigned PATTINSON_Y_POSITION       = 100;


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

void DoComposedPicture(const unsigned char *add_pixels, unsigned char *result_picture_pixels, 
                       const unsigned width, const unsigned height, const unsigned x_position, 
                       const unsigned y_position)
{
    assert(add_pixels            != nullptr);
    assert(result_picture_pixels != nullptr);

    for (int y_pos = 0; y_pos < height; ++y_pos)
    {
        for (int x_pos = 0; x_pos < width; ++x_pos)
        {
            unsigned char alpha = add_pixels[(y_pos * width + x_pos) * sizeof(unsigned) + COLOR_PIXELS];
            
            for (int color_index = 0; color_index < COLOR_PIXELS; ++color_index)
            {
                result_picture_pixels[((y_pos + y_position)    * BACKGROUND_WIDTH + 
                                        x_pos + x_position)    * sizeof(unsigned) + color_index] = 
               (result_picture_pixels[((y_pos + y_position)    * BACKGROUND_WIDTH + 
                                        x_pos + x_position)    * sizeof(unsigned) + color_index] * (255 - alpha) + 
                      add_pixels[      (y_pos * width + x_pos) * sizeof(unsigned) + color_index] * alpha) / 255;
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

int main()
{   
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
    const unsigned char *background_pixels = InitImage(&background, "background.jpg");

    sf::Image dude;
    const unsigned char *dude_pixels       = InitImage(&dude,       "dude.png");

    sf::Image pattinson;
    const unsigned char *pattinson_pixels  = InitImage(&pattinson,  "pattinson.png");

    picture result_picture;
    result_picture.texture.create(BACKGROUND_WIDTH, BACKGROUND_HEIGHT);
    result_picture.sprite.setTexture(result_picture.texture);

    unsigned *result_picture_pixels = (unsigned *) calloc(BACKGROUND_WIDTH * BACKGROUND_HEIGHT,  sizeof(unsigned));
    memcpy(result_picture_pixels, background_pixels,      BACKGROUND_WIDTH * BACKGROUND_HEIGHT * sizeof(unsigned));

    for (int i = 0; i < 100000; ++i)
    {
        DoComposedPicture(dude_pixels,      (unsigned char *) result_picture_pixels,
                          DUDE_WIDTH, DUDE_HEIGHT, DUDE_X_POSITION, DUDE_Y_POSITION);
        DoComposedPicture(pattinson_pixels, (unsigned char *) result_picture_pixels,
                          PATTINSON_WIDTH, PATTINSON_HEIGHT, PATTINSON_X_POSITION,
                          PATTINSON_Y_POSITION);
        RenewFPS(&fps);
    }

    /*
    sf::RenderWindow window(sf::VideoMode(BACKGROUND_WIDTH, BACKGROUND_HEIGHT), "Composing_pictures", sf::Style::Default);  

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        DoComposedPicture(dude_pixels,      (unsigned char *) result_picture_pixels,
                          DUDE_WIDTH, DUDE_HEIGHT, DUDE_X_POSITION, DUDE_Y_POSITION);
        DoComposedPicture(pattinson_pixels, (unsigned char *) result_picture_pixels,
                          PATTINSON_WIDTH, PATTINSON_HEIGHT, PATTINSON_X_POSITION,
                          PATTINSON_Y_POSITION);
                          
        result_picture.texture.update((const uint8_t *) result_picture_pixels);
        
        RenewFPS(&fps);
        
        window.clear();
        
        window.draw(result_picture.sprite);
        window.draw(fps.text);

        window.display();
    }*/

    free(result_picture_pixels);

    return 0;
}