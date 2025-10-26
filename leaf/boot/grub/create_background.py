#!/usr/bin/env python3
# boot/grub/create_background.py
from PIL import Image, ImageDraw, ImageFont

img = Image.new('RGB', (800, 600), color=(10, 20, 40))
draw = ImageDraw.Draw(img)

# Draw gradient background
for y in range(600):
    shade = int(10 + (y / 600) * 30)
    draw.rectangle([(0, y), (800, y+1)], fill=(shade, shade*2, shade*4))

# Draw grid pattern
for x in range(0, 800, 40):
    draw.line([(x, 0), (x, 600)], fill=(30, 40, 60), width=1)
for y in range(0, 600, 40):
    draw.line([(0, y), (800, y)], fill=(30, 40, 60), width=1)

# Draw title
try:
    font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 60)
    small_font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24)
except:
    font = ImageFont.load_default()
    small_font = ImageFont.load_default()

draw.text((400, 150), "LeafOS", font=font, fill=(100, 200, 255), anchor="mm")
draw.text((400, 220), "Boot Option", font=small_font, fill=(150, 180, 220), anchor="mm")

# Draw decorative corners
draw.arc([20, 20, 120, 120], 180, 270, fill=(70, 140, 200), width=3)
draw.arc([680, 20, 780, 120], 270, 360, fill=(70, 140, 200), width=3)
draw.arc([20, 480, 120, 580], 90, 180, fill=(70, 140, 200), width=3)
draw.arc([680, 480, 780, 580], 0, 90, fill=(70, 140, 200), width=3)

import os
import sys

# Get the output path from command line or use default
output_path = sys.argv[1] if len(sys.argv) > 1 else 'background.png'

# Create directory if needed
os.makedirs(os.path.dirname(output_path) or '.', exist_ok=True)

img.save(output_path)
print(f"✓ Background image created: {output_path}")