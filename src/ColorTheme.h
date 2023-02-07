#pragma once

#include "ofMain.h"

namespace ColorThemes
{
    enum Color{
        background,
        black,
        red,
        green,
        yellow,
        blue,
        magenta,
        cyan,
        white,
        foreground,
        bright_black,
        bright_red,
        bright_green,
        bright_yellow,
        bright_blue,
        bright_magenta,
        bright_cyan,
        bright_white,
    };

    //const vector<ColorTheme> color_themes = {
    const vector<vector<ofColor>> colorThemes = {
        {   // dracula
            ofColor::fromHex(0x282a36), // background
            ofColor::fromHex(0x21222c), // black
            ofColor::fromHex(0xff5555), // red
            ofColor::fromHex(0x50fa7b), // green
            ofColor::fromHex(0xf1fa8c), // yellow
            ofColor::fromHex(0xbd93f9), // blue
            ofColor::fromHex(0xff79c6), // magenta
            ofColor::fromHex(0x8be9fd), // cyan
            ofColor::fromHex(0xf8f8f2), // white
            ofColor::fromHex(0xf8f8f2), // foreground
            ofColor::fromHex(0x6272a4), // bright black
            ofColor::fromHex(0xff6e6e), // bright red
            ofColor::fromHex(0x69ff94), // bright green
            ofColor::fromHex(0xffffa5), // bright yellow
            ofColor::fromHex(0xd6acff), // bright blue
            ofColor::fromHex(0xff92df), // bright magenta
            ofColor::fromHex(0xa4ffff), // bright cyan
            ofColor::fromHex(0xffffff), // bright white
        }, 
        {   // catpuccin-latte
            ofColor::fromHex(0x4C4F69), // background
            ofColor::fromHex(0x5C5F77), // black
            ofColor::fromHex(0xD20F39), // red
            ofColor::fromHex(0x40A02B), // green
            ofColor::fromHex(0xDF8E1D), // yellow
            ofColor::fromHex(0x1E66F5), // blue
            ofColor::fromHex(0xEA76CB), // magenta
            ofColor::fromHex(0x179299), // cyan
            ofColor::fromHex(0xACB0BE), // white
            ofColor::fromHex(0xEFF1F5), // foreground
            ofColor::fromHex(0x6C6F85), // bright black
            ofColor::fromHex(0xD20F39), // bright red
            ofColor::fromHex(0x40A02B), // bright green
            ofColor::fromHex(0xDF8E1D), // bright yellow
            ofColor::fromHex(0x1E66F5), // bright blue
            ofColor::fromHex(0xEA76CB), // bright magenta
            ofColor::fromHex(0x179299), // bright cyan
            ofColor::fromHex(0xBCC0CC), // bright white
        }
    }; 
}
