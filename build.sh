#!/bin/sh
cc asteroids.c -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib` -o asteroids
./asteroids