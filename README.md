# Composing pictures

The program is free-to-use for all kinds of developers.

***
## What does the project do
The program does [alpha compositing](https://en.wikipedia.org/wiki/Alpha_compositing) of two or more pictures.

***
## Why is the project useful
Alpha compositing is a task of a frequent occurence, therefore it makes sense to try to speed it up
using different approaches.

***
## Functionality
Define directive at the beginning of [main](main.cpp) switches modes of the program:

1. __PICTURE_SHOWING__: showing picture which is a result of alpha compositing of pictures
2. __FPS_CHECKING__: just doing calculations connected with alpha compositing, in this case FPS counter is being shown at the top left corner 
 
Showing the result in a window is implemented via [SFML library](https://www.sfml-dev.org/).

***
## How can I help
I explicitly welcome any outside contributors. Use my mail: _ilichev.kv@phystech.edu_.

__A strong request__:
- Do not break the codestyle
- Do not destroy previous functionality with additions

***
## Where to ask questions
Use the mail mentioned earlier.
