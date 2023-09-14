from PIL import Image

img = Image.open('assets/backgrounds/warehouse_chests.png', 'r')
px = img.load()

chest = px[3, 32]
for y in range(96):
    for x in range(64):
        if (px[x, y]) != chest:
            continue
        print(hex(y * 64 + x), end=',')
