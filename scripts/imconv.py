#!/usr/bin/python3

from PIL import Image
from sys import argv

def bmp_4bit_indexed_to_c(img: Image, params: list):
    if len(params) > 0:
        name = params[0]
    else:
        name = "image"

    palette = img.getpalette()
    palette_str = "const uint16_t %sPalette[16] = {" % name
    pixels_str = "const uint8_t %sData[] = {" % name

    if palette is not None:
        ix = 0
        for color in range(0, 16):
            r = palette[ix]
            ix+=1
            g = palette[ix]
            ix+=1
            b = palette[ix]
            ix+=1

            r5 = r >> 3
            g6 = g >> 2
            b5 = b >> 3

            rgb565 = ((r5 << 11) | (g6 << 5) | b5)
            if color % 8 == 0:
                palette_str += "\n    "
            palette_str += "0x%04x, " % rgb565


        palette_str = palette_str[:-2] + "\n};"

    pixels_str += "\n    %d, %d, 1, 0, 0, 0, (uint8_t)ColorMode::index, 0," % (img.width, img.height)

    index = 0;
    for y in range (0, img.height):
        for x in range(0, img.width, 2):

            px1 = img.getpixel( (x, y) )
            px2 = img.getpixel( (x+1, y) )

            if index % 16 == 0:
                pixels_str += "\n    "
            pixels_str += "0x%02x, " % (px1 << 4 | px2)
            
            index += 1
    
    pixels_str = pixels_str[:-2] + "\n};"

    return palette_str + "\n" + pixels_str

def gbm_screenshot_palette_fixup(img: Image, params: list):
    if len(params) < 1:
        print("Gamebuino Meta screenshot palette fixup requires 2 or 3 parameters: ")
        print(" - Result file name")
        print(" - Tileset BMP file")
        print(" - Extra information (optional)")
        return None

    assert(img.width == 160)
    assert(img.height % 128 == 0)

    tileset_image = Image.open(params[1])
    main_palette = tileset_image.getpalette()
    tileset_image.close()

    altered_palettes = [None] * 128

    if len(params) > 2:
        if params[2] == "spaceshoot-gameplay":
            for ix in range(0, 8):
                alt_pal = [
                    0, 0, (8 - ix) * 24,
                    (7 - ix) * 32, (8 - ix) * 31, 255,
                    255, (8 - ix) * 31, 0,
                    (7 - ix) * 32, (4 - ix/2) * 63, 160
                ] + ([0] * 12 * 3)
                altered_palettes[ix] = alt_pal
                altered_palettes[127 - ix] = alt_pal
        else:
            print("Unsupported specialization: %s" % params[3])

    assert(main_palette is not None)
    n_frames = int(img.height / 128)

    result_image = Image.new(mode="RGB", size=(img.width, img.height))

    for frame_no in range(0, n_frames):
        for pal_y in range(0, 128):
            y = frame_no * 128 + pal_y
            for x in range(0, img.width):
                color_index = img.getpixel(xy=(x, y))
                assert(isinstance(color_index, int))
                    
                if altered_palettes[pal_y] is not None:
                    palette = altered_palettes[pal_y]
                else:
                    palette = main_palette

                r = int(palette[color_index * 3 + 0])
                g = int(palette[color_index * 3 + 1])
                b = int(palette[color_index * 3 + 2])

                r = r & 0xF8
                g = g & 0xFC
                b = b & 0xF8

                result_image.putpixel( (x, y), (r, g, b) )

    result_image.save(params[0], "BMP")

    return params[0]


if __name__ == "__main__":
    if len(argv) < 3:
        print("USAGE: %s <filename> <mode> [params...]")
        exit(1)

    img = Image.open(argv[1])
    mode = argv[2]

    MODES = {
        "bmp4-c": (
            "indexed 16-color (4 bpp) BMP to C code",
            bmp_4bit_indexed_to_c),
        "gbmss-colorcells1-deploy": (
            "Palette fixup for Gamebuino Meta screenshot (or recording)",
            gbm_screenshot_palette_fixup)
    }

    if mode in MODES:
        descr, func = MODES[mode]
        print(func(img, argv[3:]))
    else:
        print("Unknown mode: %s. Supported modes available:" % mode)
        for key, mode_descr in MODES.items():
            print("%-30s: %s" % (key, mode_descr[0]))
        


    img.close()
