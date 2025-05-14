import os
from PIL import Image

# Change this to your target directory
directory = r"D:\CLASSSWORKKK\far5a\Far5a-Trollware\frames"

# Iterate over all files in the directory
for filename in os.listdir(directory):
    if filename.lower().endswith(".png"):
        png_path = os.path.join(directory, filename)
        bmp_filename = os.path.splitext(filename)[0] + ".bmp"
        bmp_path = os.path.join(directory, bmp_filename)

        try:
            with Image.open(png_path) as img:
                rgb_img = img.convert("RGB")  # BMP doesn't support alpha channel
                rgb_img.save(bmp_path, "BMP")
                print(f"Converted: {filename} -> {bmp_filename}")
        except Exception as e:
            print(f"Failed to convert {filename}: {e}")