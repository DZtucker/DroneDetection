import os
import json
import shutil
import random

for i in range(75, 151):
    # 遍历图像文件夹
    image_folder = f'D:/Coding/Python/Objectdetection/Objectdetection/Objectdetection/train/{i}'
    target_folder = r'D:\Coding\Python\Objectdetection\image'

    image_files = [f for f in os.listdir(image_folder) if f.endswith('.jpg')]
    k = min(len(image_files), 50)
    # 随机选择图像
    selected_images = random.sample(image_files, k=k)

    # 复制图像和对应的txt文件到目标文件夹
    for filename in selected_images:
        basename = os.path.splitext(filename)[0]
        # 检查对应的txt文件是否存在
        txt_file = os.path.join(image_folder, basename + '.txt')
        if os.path.exists(txt_file):
            # 复制图像
            image_src = os.path.join(image_folder, filename)
            image_dst = os.path.join(target_folder, filename)
            shutil.copyfile(image_src, image_dst)
            # 复制txt文件
            txt_src = os.path.join(image_folder, basename + '.txt')
            txt_dst = os.path.join(target_folder, basename + '.txt')
            shutil.copyfile(txt_src, txt_dst)
