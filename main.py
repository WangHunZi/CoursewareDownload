import re,os,random
import requests
from bs4 import BeautifulSoup

# 定义一个函数，用于将特殊字符替换为下划线
def sanitize_filename(filename):
    # 使用正则表达式替换特殊字符为下划线
    return re.sub(r'[\\/:*?"<>|]', '_', filename)

url = "https://jyywiki.cn/OS/2023/"             # 目标网页的URL

jupyter_links = []                              # 获取Jupyter课件的网页链接
jupyter_filenames_path = []                     # 本地文件名
jupyter_normalized_path = []                    # 格式化的文件路径

slide_links = []                                # 获取课件中嵌入的课件的链接
slide_filenames_path = []                       # 嵌入的课件在本地的地址
slide_normalized_path = []                      # 格式化的文件路径

reveal_links = []                           # 获取课件中嵌入的课件的样式链接
reveal_filenames_path = []                  # 嵌入的课件中的样式在本地的地址
reveal_normalized_path = []                 # 格式化的文件路径


current_dir = os.getcwd() # 获取当前脚本所在的文件夹路径
courseware_path = os.path.join(current_dir, "OSCourseware") # 创建courseware文件夹
os.makedirs(courseware_path, exist_ok=True)


os_path = os.path.join(courseware_path, "OS", "2023") # 创建OS文件夹
os.makedirs(os_path, exist_ok=True)

jupyter_folder = os.path.join(os_path, "build") # 创建build文件夹
os.makedirs(jupyter_folder, exist_ok=True)  # 创建保存Jupyter课件的文件夹

slides_folder = os.path.join(os_path, "slides") # 创建slides文件夹
os.makedirs(slides_folder, exist_ok=True)   # 创建保存保存Jupyter中嵌入的课件的文件夹

static_path = os.path.join(courseware_path, "static") # 创建static文件夹
os.makedirs(static_path, exist_ok=True)

css_path = os.path.join(static_path, "css") # 创建css、katex和reveal文件夹
os.makedirs(css_path, exist_ok=True)

katex_path = os.path.join(static_path, "katex")
os.makedirs(katex_path, exist_ok=True)

reveal_path = os.path.join(static_path, "reveal")
os.makedirs(reveal_path, exist_ok=True)

simple_path = os.path.join(reveal_path, "theme") # 创建reveal/theme文件夹
os.makedirs(simple_path, exist_ok=True)

# 下载目录网页
index_file = os.path.join(os_path, "index.html")
response = requests.get(url) # 发送HTTP GET请求获取网页内容
if response.status_code == 200:
    # 获取网页内容
    web_content = response.text
    # 打开文件并写入网页内容
    with open(index_file, "w", encoding="utf-8") as file:
        file.write(web_content)
    # 打开文件并读取内容
    with open(index_file, "r", encoding="utf-8") as file:
        html_content = file.read()

    # 使用Beautiful Soup解析HTML内容
    soup = BeautifulSoup(html_content, 'html.parser')
    # 找到所有包含href属性的<a>标签
    link_tags = soup.find_all('a', href=True)
    # 提取所有的href链接
    href_links = [link.get('href') for link in link_tags]

    # 使用正则表达式筛选出满足条件的链接
    pattern = re.compile(r'^build/lect\d+\.ipynb\.html$')  # 例如，匹配 "build/lect1.ipynb.html"
    # 等待加入链接
    jupyter_filenames_path = [link for link in href_links if pattern.match(link)]
    # 添加筛选后的链接
    for path in jupyter_filenames_path:
        jupyter_links.append(url+path)
        jupyter_normalized_path.append(os.path.normpath(os.path.join(os_path, path)))

    jupyter_filenames_path[0:len(jupyter_normalized_path)] = jupyter_normalized_path

else:
    print(f"无法访问网页，状态码: {response.status_code}")

# 访问并Jupyter课件网页链接并下载Jupyter课件
for link in jupyter_links:
    # 仅下载缺失的文件
    file_path = os.path.normpath(os.path.join(os_path, link[len(url):]))
    if not os.path.exists(file_path):
        response = requests.get(link)
        # 检查响应状态码
        if response.status_code == 200:
            # 保存文件
            with open(file_path, "wb") as file:
                file.write(response.content)
            print(f"已下载文件: {file_path}")
        else:
            print(f"无法下载文件: {file_path}, 状态码: {response.status_code}")

# 打开本地已经下载好的Jupyter课件，获取嵌入Jupyter课件中的幻灯片链接供下载使用
for file in os.listdir(jupyter_folder):
    file_path = os.path.join(jupyter_folder, file)
    # 打开文件并解析其内容
    with open(file_path, 'r', encoding='utf-8') as file:
        content = file.read()
        soup = BeautifulSoup(content, 'html.parser')
        # 查找所有包含嵌入链接的<iframe>标签
        iframe_tags = soup.find_all('iframe', class_='slideshow')
        # 遍历嵌入链接并处理
        for iframe in iframe_tags:
            src = iframe.get('src')
            # 构建完整链接
            if src.startswith('../'):
                slide_links.append(url + src[3:])
            # 构建保存文件的路径
            slide_filenames_path.append(os.path.join(slides_folder, src.split("/")[-1]))

# 下载嵌入Jupyter课件中的幻灯片
for link in slide_links:
    file_path = os.path.normpath(os.path.join(os_path, link[len(url):]))
    if not os.path.exists(file_path):
        response_slide = requests.get(link)
        # 检查响应状态码
        if response_slide.status_code == 200:
            # 保存文件
            with open(file_path, "wb") as slide_file:
                slide_file.write(response_slide.content)
            print(f"已下载文件: {file_path}")
        else:
            print(f"无法下载文件: {file_path}, 状态码: {response.status_code}")

# slides文件夹中某个的存在的课件文件，并读取其内容下载js和css样式文件
file_list = os.listdir(slides_folder)
random.shuffle(file_list)                                       # 随机排序文件列表
file_path = os.path.join(slides_folder, file_list[0])   # 获取随机文件的文件名

# 打开文件并解析其内容
with (open(file_path, 'r', encoding='utf-8') as file):
    content = file.read()
    soup = BeautifulSoup(content, 'html.parser')

    # 查找所有的<script>标签和<link>标签
    script_tags = soup.find_all('script', src=True)
    link_tags = soup.find_all('link', rel='stylesheet', href=True)

    # 遍历嵌入的JavaScript脚本链接并处理
    for script in script_tags:
        src = script.get('src')
        # 构建完整链接
        if src.startswith('../../../static/'):
            src = "https://jyywiki.cn" + src[2:]
        reveal_links.append(src)

    # 遍历嵌入的CSS样式链接并处理
    for link in link_tags:
        href = link.get('href')
        # 构建完整链接
        if href.startswith('../../../static/'):
            href = "https://jyywiki.cn" + href[2:]
        reveal_links.append(href)

    for link in reveal_links:
        # 滤过其他文件
        if link.startswith("https://jyywiki.cn/../../static"):
            # 提取相对路径部分
            relative_path = link.replace("https://jyywiki.cn/../../static/", "")
            # 构建目标文件夹路径
            reveal_filenames_path.append(os.path.normpath(os.path.join(static_path, relative_path)))
            for file_path in reveal_filenames_path:
                compare_path = file_path.replace(static_path+'\\', "")
                compare_path = compare_path.replace('\\', "/")
                if not os.path.exists(file_path) and compare_path == relative_path:
                    # 发送HTTP GET请求获取JavaScript脚本或css样式文件内容
                    response_script = requests.get(link)
                    if response_script.status_code == 200:
                        # 保存文件
                        with open(file_path, "wb") as file:
                            file.write(response_script.content)
                        print(f"已下载文件: {file}")
                    else:
                        print(f"无法下载文件: {file}, 状态码: {response_script.status_code}")