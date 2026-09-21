#!/usr/bin/env python3
"""Render the firmware's existing RGB565 assets for visual verification (Pillow)."""
from pathlib import Path
from PIL import Image, ImageDraw
from build_assets import source_assets


def render(style, scale):
    assets = source_assets()
    diameter, width, height, canvas = (80, 256, 64, 200) if style == 'human' else (128, 256, 32, 160)
    iris = assets[style+'_iris'][0]
    polar = assets[style+'_polar'][0]
    sclera = assets['human_sclera'][0]
    upper = assets['upper_lid'][0]
    lower = assets['lower_lid'][0]
    offset = (canvas - 128)//2
    sx = canvas//2 - offset//2
    sy = canvas//2 - (offset + diameter//4)
    upper_t = (upper[sy*128+sx] + upper[sy*128+127-sx])//2
    lower_t = 254 - upper_t
    threshold = (128*(1023-scale)+512)//1024
    iris_scale = height*65536//threshold
    image = Image.new('RGB', (128,128))
    for y in range(128):
        iy = y + offset - (canvas-diameter)//2
        for x in range(128):
            ix = x + offset - (canvas-diameter)//2
            value = sclera[(y+offset)*200+x+offset] if style == 'human' else 0
            if upper[y*128+x] <= upper_t or lower[y*128+x] <= lower_t:
                value = 0
            elif 0 <= ix < diameter and 0 <= iy < diameter:
                position = polar[iy*diameter+ix]
                distance = position & 127
                if distance < threshold:
                    row = distance*iris_scale//65536
                    column = width*(position >> 7)//512
                    value = iris[row*width+column]
            image.putpixel((x,y), ((value>>11)*255//31, ((value>>5)&63)*255//63, (value&31)*255//31))
    return image.rotate(180)


if __name__ == '__main__':
    result = Image.new('RGB', (3*272, 3*292), '#171b24')
    draw = ImageDraw.Draw(result)
    for column, (style, minimum, maximum) in enumerate([('human',120,720),('lizard',80,400),('goat',80,320)]):
        for row, scale in enumerate([minimum, (minimum+maximum)//2, maximum]):
            x, y = column*272+8, row*292+8
            label = ['minimum pupil','medium pupil','maximum pupil'][row]
            draw.text((x,y), f'{style.title()} - {label}', fill='white')
            result.paste(render(style,scale).resize((256,256), Image.Resampling.NEAREST),(x,y+20))
    destination = Path(__file__).resolve().parents[1]/'docs/eye-styles.png'
    result.save(destination)
    print(destination)
