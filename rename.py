import os

path = r"D:\Coding\Python\Objectdetection\Objectdetection\Objectdetection\train"  # 文件夹路径

for folder_name in os.listdir(path):
    if os.path.isdir(os.path.join(path, folder_name)):
        for file_name in os.listdir(os.path.join(path, folder_name)):
            if file_name.endswith(".jpg") or file_name.endswith(".txt"):
                new_file_name = folder_name + "_" + file_name
                os.rename(os.path.join(path, folder_name, file_name), os.path.join(path, folder_name, new_file_name))
