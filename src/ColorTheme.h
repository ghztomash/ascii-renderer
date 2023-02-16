#pragma once

#include "ofMain.h"

namespace ColorThemes
{
    enum Color{
        background,
        red,
        green,
        yellow,
        blue,
        magenta,
        cyan,
        foreground,
        bright_red,
        bright_green,
        bright_yellow,
        bright_blue,
        bright_magenta,
        bright_cyan,
    };

    //const vector<ColorTheme> color_themes = {
    const vector<vector<ofColor>> colorThemes = {
        {   // dracula https://draculatheme.com/
            ofColor::fromHex(0x282a36), // background
            ofColor::fromHex(0xff5555), // red
            ofColor::fromHex(0x50fa7b), // green
            ofColor::fromHex(0xf1fa8c), // yellow
            ofColor::fromHex(0xbd93f9), // blue
            ofColor::fromHex(0xff79c6), // magenta
            ofColor::fromHex(0x8be9fd), // cyan
            ofColor::fromHex(0xf8f8f2), // foreground
            ofColor::fromHex(0xff6e6e), // bright red
            ofColor::fromHex(0x69ff94), // bright green
            ofColor::fromHex(0xffffa5), // bright yellow
            ofColor::fromHex(0xd6acff), // bright blue
            ofColor::fromHex(0xff92df), // bright magenta
            ofColor::fromHex(0xa4ffff), // bright cyan
        }, 
        /*
        {   // catpuccin-latte https://github.com/catppuccin/catppuccin
            ofColor::fromHex(0xEFF1F5), // background
            ofColor::fromHex(0xD20F39), // red
            ofColor::fromHex(0x40A02B), // green
            ofColor::fromHex(0xDF8E1D), // yellow
            ofColor::fromHex(0x1E66F5), // blue
            ofColor::fromHex(0xEA76CB), // magenta
            ofColor::fromHex(0x179299), // cyan
            ofColor::fromHex(0x4C4F69), // foreground
            ofColor::fromHex(0xD20F39), // bright red
            ofColor::fromHex(0x40A02B), // bright green
            ofColor::fromHex(0xDF8E1D), // bright yellow
            ofColor::fromHex(0x1E66F5), // bright blue
            ofColor::fromHex(0xEA76CB), // bright magenta
            ofColor::fromHex(0x179299), // bright cyan
        }, 
        */
        {   // catpuccin-frape https://github.com/catppuccin/catppuccin
            ofColor::fromHex(0x303446), // background
            ofColor::fromHex(0xe78284), // red
            ofColor::fromHex(0xA6D189), // green
            ofColor::fromHex(0xE5C890), // yellow
            ofColor::fromHex(0x8CAAEE), // blue
            ofColor::fromHex(0xF4B8E4), // magenta
            ofColor::fromHex(0x81C8BE), // cyan
            ofColor::fromHex(0xC6D0F5), // foreground
            ofColor::fromHex(0xE78284), // bright red
            ofColor::fromHex(0x81c8be), // bright green
            ofColor::fromHex(0xef9f76), // bright yellow
            ofColor::fromHex(0x99d1db), // bright blue
            ofColor::fromHex(0xca9ee6), // bright magenta
            ofColor::fromHex(0x85c1dc), // bright cyan
        }, 
        {   // catpuccin-macchiato https://github.com/catppuccin/catppuccin
            ofColor::fromHex(0x24273A), // background
            ofColor::fromHex(0xED8796), // red
            ofColor::fromHex(0xA6DA95), // green
            ofColor::fromHex(0xEED49F), // yellow
            ofColor::fromHex(0x8AADF4), // blue
            ofColor::fromHex(0xF5BDE6), // magenta
            ofColor::fromHex(0x8BD5CA), // cyan
            ofColor::fromHex(0xCAD3F5), // foreground
            ofColor::fromHex(0xee99a0), // bright red
            ofColor::fromHex(0xA6DA95), // bright green
            ofColor::fromHex(0xf5a97f), // bright yellow
            ofColor::fromHex(0x91d7e3), // bright blue
            ofColor::fromHex(0xc6a0f6), // bright magenta
            ofColor::fromHex(0x7dc4e4), // bright cyan
        }, 
        {   // catpuccin-mocha https://github.com/catppuccin/catppuccin
            ofColor::fromHex(0x1E1E2E), // background
            ofColor::fromHex(0xF38BA8), // red
            ofColor::fromHex(0xA6E3A1), // green
            ofColor::fromHex(0xF9E2AF), // yellow
            ofColor::fromHex(0x89B4FA), // blue
            ofColor::fromHex(0xF5C2E7), // magenta
            ofColor::fromHex(0x94E2D5), // cyan
            ofColor::fromHex(0xCDD6F4), // foreground
            ofColor::fromHex(0xeba0ac), // bright red
            ofColor::fromHex(0x74c7ec), // bright green
            ofColor::fromHex(0xfab387), // bright yellow
            ofColor::fromHex(0xb4befe), // bright blue
            ofColor::fromHex(0xF5C2E7), // bright magenta
            ofColor::fromHex(0x89dceb), // bright cyan
        }, 
        {   // cyberpunk-neon https://github.com/Roboron3042/Cyberpunk-Neon
            ofColor::fromHex(0x000b1e), // background
            ofColor::fromHex(0xff0000), // red
            ofColor::fromHex(0xd300c4), // green
            ofColor::fromHex(0xf57800), // yellow
            ofColor::fromHex(0x123e7c), // blue
            ofColor::fromHex(0x711c91), // magenta
            ofColor::fromHex(0x00F2FF), // cyan
            ofColor::fromHex(0x0abdc6), // foreground
            ofColor::fromHex(0x66D437), // bright red
            ofColor::fromHex(0xD437C9), // bright green
            ofColor::fromHex(0xF2B57C), // bright yellow
            ofColor::fromHex(0x00ff00), // bright blue
            ofColor::fromHex(0xFF19F0), // bright magenta
            ofColor::fromHex(0x00F2FF), // bright cyan
        }, 
        {   // tokyo-night https://github.com/zatchheems/tokyo-night-alacritty-theme
            ofColor::fromHex(0x1a1b26), // background
            ofColor::fromHex(0xf7768e), // red
            ofColor::fromHex(0x9ece6a), // green
            ofColor::fromHex(0xe0af68), // yellow
            ofColor::fromHex(0x7aa2f7), // blue
            ofColor::fromHex(0xad8ee6), // magenta
            ofColor::fromHex(0x449dab), // cyan
            ofColor::fromHex(0xa9b1d6), // foreground
            ofColor::fromHex(0xff7a93), // bright red
            ofColor::fromHex(0xb9f27c), // bright green
            ofColor::fromHex(0xff9e64), // bright yellow
            ofColor::fromHex(0x7da6ff), // bright blue
            ofColor::fromHex(0xbb9af7), // bright magenta
            ofColor::fromHex(0x0db9d7), // bright cyan
        }, 
        /*
        {   // tokyo-night-storm https://github.com/zatchheems/tokyo-night-alacritty-theme
            ofColor::fromHex(0x24283b), // background
            ofColor::fromHex(0xf7768e), // red
            ofColor::fromHex(0x9ece6a), // green
            ofColor::fromHex(0xe0af68), // yellow
            ofColor::fromHex(0x7aa2f7), // blue
            ofColor::fromHex(0xad8ee6), // magenta
            ofColor::fromHex(0x449dab), // cyan
            ofColor::fromHex(0xa9b1d6), // foreground
            ofColor::fromHex(0xff7a93), // bright red
            ofColor::fromHex(0xb9f27c), // bright green
            ofColor::fromHex(0xff9e64), // bright yellow
            ofColor::fromHex(0x7da6ff), // bright blue
            ofColor::fromHex(0xbb9af7), // bright magenta
            ofColor::fromHex(0x0db9d7), // bright cyan
        }, 
        */
        {   // https://www.markusweimar.de/static/contrasty-darkness-alacritty.txt 
            ofColor::fromHex(0x000000), // background
            ofColor::fromHex(0xff7c4d), // red
            ofColor::fromHex(0x22ff00), // green
            ofColor::fromHex(0xffcc00), // yellow
            ofColor::fromHex(0x1a66ff), // blue
            ofColor::fromHex(0xff61df), // magenta
            ofColor::fromHex(0x00ffff), // cyan
            ofColor::fromHex(0xffffff), // foreground
            ofColor::fromHex(0xFF4400), // bright red
            ofColor::fromHex(0xACFF00), // bright green
            ofColor::fromHex(0xFFD600), // bright yellow
            ofColor::fromHex(0x003BB3), // bright blue
            ofColor::fromHex(0xff61df), // bright magenta
            ofColor::fromHex(0x68FF00), // bright cyan
        }, 
        {   // https://github.com/alacritty/alacritty-theme/blob/master/themes/ayu_dark.yaml
            ofColor::fromHex(0x0A0E14), // background
            ofColor::fromHex(0xEA6C73), // red
            ofColor::fromHex(0x91B362), // green
            ofColor::fromHex(0xF9AF4F), // yellow
            ofColor::fromHex(0x53BDFA), // blue
            ofColor::fromHex(0xFAE994), // magenta
            ofColor::fromHex(0x90E1C6), // cyan
            ofColor::fromHex(0xB3B1AD), // foreground
            ofColor::fromHex(0xF07178), // bright red
            ofColor::fromHex(0xC2D94C), // bright green
            ofColor::fromHex(0xFFB454), // bright yellow
            ofColor::fromHex(0x59C2FF), // bright blue
            ofColor::fromHex(0xFFEE99), // bright magenta
            ofColor::fromHex(0x95E6CB), // bright cyan
        }, 
        {   // https://github.com/alacritty/alacritty-theme/blob/master/themes/tomorrow_night_bright.yaml
            ofColor::fromHex(0x000000), // background
            ofColor::fromHex(0xd54e53), // red
            ofColor::fromHex(0xb9ca4a), // green
            ofColor::fromHex(0xe6c547), // yellow
            ofColor::fromHex(0x7aa6da), // blue
            ofColor::fromHex(0xc397d8), // magenta
            ofColor::fromHex(0x70c0ba), // cyan
            ofColor::fromHex(0xeaeaea), // foreground
            ofColor::fromHex(0xff3334), // bright red
            ofColor::fromHex(0x9ec400), // bright green
            ofColor::fromHex(0xe7c547), // bright yellow
            ofColor::fromHex(0x7aa6da), // bright blue
            ofColor::fromHex(0xb77ee0), // bright magenta
            ofColor::fromHex(0x54ced6), // bright cyan
        }, 
        {   // https://github.com/alacritty/alacritty-theme/blob/master/themes/tomorrow_night.yaml
            ofColor::fromHex(0x1d1f21), // background
            ofColor::fromHex(0xcc6666), // red
            ofColor::fromHex(0xb5bd68), // green
            ofColor::fromHex(0xe6c547), // yellow
            ofColor::fromHex(0x81a2be), // blue
            ofColor::fromHex(0xb294bb), // magenta
            ofColor::fromHex(0x70c0ba), // cyan
            ofColor::fromHex(0xc5c8c6), // foreground
            ofColor::fromHex(0xff3334), // bright red
            ofColor::fromHex(0x9ec400), // bright green
            ofColor::fromHex(0xf0c674), // bright yellow
            ofColor::fromHex(0x81a2be), // bright blue
            ofColor::fromHex(0xb77ee0), // bright magenta
            ofColor::fromHex(0x54ced6), // bright cyan
        }, 
        /*
        {   // 
            ofColor::fromHex(0x), // background
            ofColor::fromHex(0x), // red
            ofColor::fromHex(0x), // green
            ofColor::fromHex(0x), // yellow
            ofColor::fromHex(0x), // blue
            ofColor::fromHex(0x), // magenta
            ofColor::fromHex(0x), // cyan
            ofColor::fromHex(0x), // foreground
            ofColor::fromHex(0x), // bright red
            ofColor::fromHex(0x), // bright green
            ofColor::fromHex(0x), // bright yellow
            ofColor::fromHex(0x), // bright blue
            ofColor::fromHex(0x), // bright magenta
            ofColor::fromHex(0x), // bright cyan
        }
        */
    }; 
}
