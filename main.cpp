#include "Composing_pictures.hpp"

int main()
{
    FPS fps;
    sf::Image background;
    sf::Image dude;
    sf::Image pattinson;
    picture result_picture;
    const unsigned char *background_pixels = nullptr;
    const unsigned char *dude_pixels       = nullptr;
    const unsigned char *pattinson_pixels  = nullptr;
    unsigned *result_picture_pixels = (unsigned *) calloc(BACKGROUND_WIDTH * BACKGROUND_HEIGHT, 
                                                          sizeof(unsigned));
                                                                 
    InitPictures(&fps, &background, &dude, &pattinson, &result_picture, &background_pixels, 
                 &dude_pixels, &pattinson_pixels, result_picture_pixels);

    for (int iteration = 0; iteration < 1000000; ++iteration)
    {
        DoComposedPicture((const unsigned *) dude_pixels, result_picture_pixels,
                           DUDE_WIDTH, DUDE_HEIGHT, DUDE_X_POSITION, DUDE_Y_POSITION);
        DoComposedPicture((const unsigned *) pattinson_pixels, result_picture_pixels,
                           PATTINSON_WIDTH, PATTINSON_HEIGHT, PATTINSON_X_POSITION,
                           PATTINSON_Y_POSITION);
        RenewFPS(&fps);
    }

    /*sf::RenderWindow window(sf::VideoMode(BACKGROUND_WIDTH, BACKGROUND_HEIGHT), 
                            "Composing_pictures", sf::Style::Default);

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        DoComposedPicture((const unsigned *) dude_pixels, result_picture_pixels,
                           DUDE_WIDTH, DUDE_HEIGHT, DUDE_X_POSITION, DUDE_Y_POSITION);
        DoComposedPicture((const unsigned *) pattinson_pixels, result_picture_pixels,
                           PATTINSON_WIDTH, PATTINSON_HEIGHT, PATTINSON_X_POSITION,
                           PATTINSON_Y_POSITION);
        result_picture.texture.update((const unsigned char *) result_picture_pixels);
        
        RenewFPS(&fps);
        
        window.clear();
        
        window.draw(result_picture.sprite);
        window.draw(fps.text);

        window.display();
    }*/

    free(result_picture_pixels);

    return 0;
}