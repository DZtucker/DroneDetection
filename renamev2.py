import os
import json

# 读取标注文件
import cv2

for i in range(19, 151):
    image_folder = f'D:/Coding/Python/Objectdetection/Objectdetection/Objectdetection/train/{i}'
    with open(os.path.join(image_folder, 'IR_label.json'), 'r') as f:
        data = json.load(f)

    # 遍历图像文件夹
    count = -1
    for filename in os.listdir(image_folder):
        if filename.endswith('.jpg'):
            basename = os.path.splitext(filename)[0]
            # 确定是否需要处理该图像
            count += 1
            idx = count
            if data['exist'][idx] == 0:
                os.remove(os.path.join(image_folder, filename))
            else:
                # 获取当前图像的宽和高
                img = cv2.imread(os.path.join(image_folder, filename))
                h, w, _ = img.shape
                # 创建txt文件并写入内容
                with open(os.path.join(image_folder, basename + '.txt'), 'w') as f:
                    rect = data['gt_rect'][idx]
                    x, y, width, height = rect[0], rect[1], rect[2], rect[3]
                    x_center = (x + width / 2) / float(w)
                    y_center = (y + height / 2) / float(h)
                    width_ratio = width / float(w)
                    height_ratio = height / float(h)
                    f.write('0 {:.6f} {:.6f} {:.6f} {:.6f}'.format(x_center, y_center, width_ratio, height_ratio))
