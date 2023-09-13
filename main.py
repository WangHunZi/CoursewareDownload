import os
import sys
import requests
from bs4 import BeautifulSoup
from urllib.parse import urljoin


def download(url_, path_):
    if not os.path.exists(os.path.dirname(path_)):
        os.makedirs(os.path.dirname(path_))
    if os.path.exists(path_):
        return
    response = requests.get(url_)
    if response.status_code == 200:
        if response.headers['Content-Type'].startswith('text'):
            with open(path_, 'w', encoding="utf-8") as file:
                file.write(response.text)
        else:
            with open(path_, 'wb') as file:
                file.write(response.content)
        print(f"\033[32m已下载 \033[0m{path_}")
    else:
        print(f"\033[91m无法下载文件链接：\033[0m{url_}, \033[91m状态码：\033[0m{response.status_code}")


class OSCourseware:
    BASE_URL = "https://jyywiki.cn"
    BASE_DIR = ""
    COURSEWARE_DIR = "Courseware"
    WITHOUT_DOWNLOAD = [
        "https://jyywiki.cn/pages/OS/2022/Labs/lab-makefile.png",  # 404
        "https://jyywiki.cn/OS/2021/slides/Slides_Author",         # 404
        "https://jyywiki.cn/index.html"                            # unnecessary
    ]
    KEY_YEAR = {'A': "2021", 'B': "2022", 'C': "2023", 'D': "ALL", '': "2023"}
    year_input = ''
    sources_url_path_pairs = {}

    def __init__(self):
        self.file_download_option()
        self.file_download()
        print("\033[32m下载完成")

    def file_download_option(self):
        def build_courseware_url_path(year_):
            url_ = f'{self.BASE_URL}/OS/{year_}'
            path_ = f'{os.path.join(self.BASE_DIR, self.COURSEWARE_DIR)}\\OS\\{year_}\\index.html'
            return {url_: path_}

        self.BASE_DIR = os.getcwd()
        self.year_input = input("无法下载的文件会提示。下载成功后提示“下载成功”\n" +
                                "通过选项下载对应年份课件，回车默认下载2023年课件，输入其他符号则退出\n" +
                                "\033[32mA\033[0m:2021 \033[32mB\033[0m:2022 "
                                "\033[32mC\033[0m:2023 \033[32mD\033[0m:All\n")
        self.year_input = self.KEY_YEAR.get(self.year_input, "Invalid")  # 将按键转化为年份

        if self.year_input == "ALL":
            for year in ['2021', '2022', '2023']:
                self.sources_url_path_pairs.update(build_courseware_url_path(year))
        elif self.year_input != "Invalid":
            self.sources_url_path_pairs.update(build_courseware_url_path(self.year_input))
            if self.year_input != "2023":
                self.WITHOUT_DOWNLOAD.append(f'{self.BASE_URL}/OS/2023/index.html')
        else:
            print("\033[91m输入非法，程序退出")
            sys.exit()

    def file_download(self):
        _url_path_pairs = {}
        while self.sources_url_path_pairs:
            _url_path_pairs.update(self.sources_url_path_pairs)
            self.sources_url_path_pairs.clear()
            for _url, _path in _url_path_pairs.items():
                download(_url, _path)
                self.file_analyse(_path)

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
            _links_attr.extend([link.get("href"), link.get("src"), link.get("data")])
        _links_attr = list(set(_links_attr))  # 去除重复的元素

        # 补全完整的文件地址和链接
        for link in _links_attr:
            if link is not None and not link.startswith(("http", "data")):  # data是ipynb.html文件资源
                # 以filepath指定的文件为参照补全文件中的网址以及在本地存储的地址
                path = os.path.normpath(os.path.join(os.path.dirname(filepath), link.replace("/", "\\")))
                try:
                    relative_path = path.split(self.BASE_DIR + os.sep + self.COURSEWARE_DIR)[1]
                    url = urljoin(self.BASE_URL, relative_path.replace("\\", "/"))
                    if url not in self.WITHOUT_DOWNLOAD:
                        self.sources_url_path_pairs.update({url: path})
                        self.WITHOUT_DOWNLOAD.append(url)
                except IndexError:
                    continue


courseware = OSCourseware()
