# DroneDetection
rename.py作用为将目标文件夹内的图片加上一个文件夹名字的前缀，防止在整合数据集时重名。
renamefr.py作用为将train数据集内的文件夹按1开始编号。
randompick.py作用为在每个文件夹内随机选取50张图片并选取与之同名的txt标注文件。
renamev2.py作用是根据json文件生成每张图片对应的txt标注文件，并且与图片同名。
其中YOLOv7的代码参考自https://github.com/WongKinYiu/yolov7
，并无太大改动。
