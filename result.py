import os
import json

# 输入文件夹路径，包含未处理的图片
input_folder = r'D:\Coding\Python\Objectdetection\Objectdetection\Objectdetection\test\20190925_133630_1_1'
# 处理后的图片文件夹路径
processed_folder = r'D:\Coding\Python\Objectdetection\Objectdetection\Objectdetection\result\20190925_133630_1_1'

with open(os.path.join(processed_folder, '1.txt'), 'r') as p:
    # 读取处理后的图片的结果
    result = []
    for line in p:
        # 逐行读取文件，将每行的 JSON 数据处理后添加到结果列表中
        try:
            data = json.loads(line.strip())
            result.extend(data['res'])
        except:
            print(f'Error reading line: {line.strip()}')

print(len(result))

# 创建txt文件，保存结果
txt_file_name = os.path.basename(input_folder) + '.txt'
with open(txt_file_name, 'w') as f:
    f.write('{"res":[')

    # 遍历文件夹内的所有图片文件
    i = 0
    j = 0
    for root, dirs, files in os.walk(input_folder):
        for file_name in files:
            # 检查是否存在与遍历到的文件名相同的文件
            if file_name.endswith('.jpg'):
                processed_path = os.path.join(processed_folder, file_name)

                if os.path.exists(processed_path):

                    # 将结果写入 txt 文件
                    f.write(str(result[i]) + ',')
                    i += 1
                else:
                    # 如果处理后的图片文件夹中不存在相应的文件，则将空列表写入 txt 文件
                    f.write('[],')
                    j += 1

    # 去除最后一个逗号
    f.seek(f.tell() - 1, os.SEEK_SET)
    f.truncate()

    f.write(']}')
    print(i)
    print(j)
    print(i+j)
