import os

# 作用为将train数据集内的文件夹按1开始编号

# 文件夹路径
folder_path = r'D:\Coding\Python\Objectdetection\Objectdetection\Objectdetection\train'

# 遍历文件夹
for idx, folder_name in enumerate(os.listdir(folder_path)):
    # 构造新的文件夹路径
    new_folder_name = str(idx + 1)
    new_folder_path = os.path.join(folder_path, new_folder_name)
    old_folder_path = os.path.join(folder_path, folder_name)
    # 重命名文件夹
    os.rename(old_folder_path, new_folder_path)
