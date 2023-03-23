import json
import os

# 定义要读取的文件夹路径和要保存结果的文件夹路径
input_folder = r"C:\Users\86188\Desktop\结果"
output_folder = r"C:\Users\86188\Desktop\取整结果"



# 遍历输入文件夹内所有txt文件
for filename in os.listdir(input_folder):
    if filename.endswith(".txt"):
        with open(os.path.join(input_folder, filename), "r") as f:
            data = json.load(f)
            # 将每个框的坐标四舍五入
            rounded_data = [[round(x) for x in box] for box in data["res"]]

        # 写入输出文件夹中同名的txt文件
        with open(os.path.join(output_folder, filename), "w") as f:
            f.write(json.dumps({"res": rounded_data}))
