import os
import sys
import requests
from bs4 import BeautifulSoup
from urllib.parse import urljoin


def download(url_, path_):
    folder_path = os.path.dirname(path_)  # 检查路径，对不存在的路径进行创建
    if not os.path.exists(folder_path):
        os.makedirs(folder_path)
    if not os.path.exists(path_):  # 检查文件是否存在，对本地不存在的文件进行下载
        response = requests.get(url_)
        if response.status_code == 200:
            if path_.endswith((".css", ".js", ".html", ".c", "cpp", ".py", ".sh", ".S")):
                with open(path_, 'w', encoding="utf-8") as file:
                    file.write(response.text)
            else:
                # 非文本文件的下载
                with open(path_, 'wb') as file:
                    file.write(response.content)
            print(f"\033[32m已下载 \033[0m{path_}")
        else:
            print(f"\033[91m无法下载文件链接：\033[0m{url_}, \033[91m状态码：\033[0m{response.status_code}")


class OSCourseware:
    BASE_URL = "https://jyywiki.cn"
    SOURCE_FILE_TYPE = (
        ".png", ".jpg", ".gif", ".webp", "jpeg", ".js", ".css", ".html", ".c", ".cpp", ".py", ".sh", ".S"
    )
    COURSEWARE_DIR = "Courseware"
    WITHOUT_DOWNLOAD = [
        "https://jyywiki.cn/pages/OS/2022/Labs/lab-makefile.png",  # 404
        "https://jyywiki.cn/index.html"  # unnecessary
    ]
    KEY_YEAR = {'A': "2021", 'B': "2022", 'C': "2023", 'D': "ALL", '': "2023"}
    year = []
    year_input = ''
    current_dir = ''
    index_url_path_pairs = {}
    slides_url_path_pairs = {}
    sources_url_path_pairs = {}  # 字典不用去重

    def __init__(self):
        self.current_dir = os.path.join(os.getcwd(), self.COURSEWARE_DIR)
        self.file_download_option()  # 等待添加新的功能：自定义文件位置、更新
        self.file_download()
        print("下载成功")

    def file_download_option(self):
        def build_courseware_url_path(year_):
            url_ = f'{self.BASE_URL}/OS/{year_}'
            path_ = f'{self.current_dir}\\OS\\{year_}\\index.html'
            self.year.append(year_)  # 更新year的内容，指定下载对应年的课件
            return {url_: path_}

        self.year_input = input("无法下载的文件会提示。下载成功后提示“下载成功”\n" +
                                "通过选项下载对应年份课件，回车默认下载2023年课件，输入其他符号则退出\n" +
                                "\033[32mA\033[0m:2021 \033[32mB\033[0m:2022 "
                                "\033[32mC\033[0m:2023 \033[32mD\033[0m:All\n")
        self.year_input = self.KEY_YEAR.get(self.year_input, "Invalid")  # 将按键转化为年份

        if self.year_input == "ALL":
            for year in ['2021', '2022', '2023']:
                self.index_url_path_pairs.update(build_courseware_url_path(year))
        elif self.year_input != "Invalid":
            self.index_url_path_pairs.update(build_courseware_url_path(self.year_input))
            if self.year_input != "2023":
                self.WITHOUT_DOWNLOAD.append(f'{self.BASE_URL}/OS/2023/index.html')
        else:
            print("输入非法，程序退出")
            sys.exit()

    def file_download(self):
        # 按年下载、分析index.html
        def _download(_url_path_pairs):
            for _url, _path in _url_path_pairs.items():
                download(_url, _path)

        def _analyse(_url_path_pairs):
            for _url, _path in _url_path_pairs.items():
                self.file_analyse(_path)

        def _analyse_download(_url_path_pairs):
            _analyse(_url_path_pairs)
            _download(self.sources_url_path_pairs)

        # 下载index.html文件，分析index.html后下载课件
        _download(self.index_url_path_pairs)
        _analyse_download(self.index_url_path_pairs)

        # 分析课件后下载课件中的其他文件
        self.slides_url_path_pairs.update(self.sources_url_path_pairs)
        _analyse_download(self.slides_url_path_pairs)

        self.slides_url_path_pairs.update(self.sources_url_path_pairs)
        _analyse_download(self.slides_url_path_pairs)

    # 提取每个文件中的链接
    def file_analyse(self, filepath):
        # 对非HTML文件不做分析
        if filepath.endswith(".html"):
            with open(filepath, 'r', encoding='utf-8') as file:
                content = file.read()
                soup = BeautifulSoup(content, 'html.parser')
        else:
            return

        # 提取文件中的相对链接
        _links_tags = soup.find_all(href=True) + soup.find_all(src=True)
        _links_attr = []
        for link in _links_tags:
            _links_attr.extend([link.get("href"), link.get("src")])
        _links_attr = list(set(_links_attr))  # 去除重复的元素

        # 补全完整的文件地址和链接
        for link in _links_attr:
            if link is None or link.startswith(("http", "data")):  # data是ipynb.html文件资源
                continue
            if link.endswith(self.SOURCE_FILE_TYPE):
                # 以filepath指定的文件为参照补全文件中的网址以及在本地存储的地址
                path = os.path.normpath(os.path.join(os.path.dirname(filepath), link.replace("/", "\\")))
                relative_path = path.split(os.getcwd() + os.sep + self.COURSEWARE_DIR)[1]
                url = urljoin(self.BASE_URL, relative_path.replace("\\", "/"))
                if url in self.WITHOUT_DOWNLOAD:
                    continue
                self.sources_url_path_pairs.update({url: path})


courseware = OSCourseware()
