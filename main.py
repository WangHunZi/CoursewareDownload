import os
import sys
import requests
import re
from bs4 import BeautifulSoup
from urllib.parse import urlparse

BASE_URL = "https://jyywiki.cn"
BASE_DIR = os.path.join(os.getcwd(), "Courseware")
WITHOUT_DOWNLOAD = [
    "https://jyywiki.cn/pages/OS/2022/Labs/lab-makefile.png",  # 404
    "https://jyywiki.cn/OS/2022/Labs/L1.html",                 # 404
    "https://jyywiki.cn/OS/2023/Labs/L1.html",                 # 404
    "https://jyywiki.cn/OS/2021/slides/Slides_Author",         # 404
    "https://jyywiki.cn/OS/2022/slides/Slides_Author",         # 404
    "https://jyywiki.cn/OS/OS_References",                     # 404
    "https://jyywiki.cn/ISER/2020/slides/Slides_Author",       # 404
    "https://jyywiki.cn/ICS/2020/slides/Slides_Author",        # 404
    "https://jyywiki.cn/ICS/2021/slides/Slides_Author",        # 404
    "https://jyywiki.cn/ICS/2021/Slides_Author",               # 404
    "https://jyywiki.cn/OI/slides/Slides_Author",              # 404
    "https://jyywiki.cn/OI/Slides_Author",                     # 404
    "https://jyywiki.cn/ISER2023/1-intro/",                    # unnecessary
    "https://jyywiki.cn/index.html",                           # unnecessary
]
KEY_YEAR = {'A': "2021", 'B': "2022", 'C': "2023", 'D': "ALL", '': "2023"}

sources_url_path_pairs = {}


def get_full_path(path, link):
    pathdir = os.path.dirname(path).replace("/", os.sep)
    filepath = ""
    if link.startswith("/"):
        if link.find("../") != -1:
            return None
        filepath = os.path.join(BASE_DIR, link[1:].replace("/", os.sep))
    elif link.startswith("../"):
        urlsplit = link.split("/")
        pathsplit = pathdir.split(os.sep)
        count = sum([-1 for item in urlsplit if item == ".."])
        filepath = os.path.join(os.sep.join(pathsplit[:count]), os.sep.join(urlsplit[-count:]))
    else:
        filepath = os.path.join(pathdir, link.replace("/", os.sep))

    return filepath


def get_full_url(path):
    return path.replace(BASE_DIR, BASE_URL).replace("\\", "/")


def download(url_, path_):
    if not os.path.exists(os.path.dirname(path_)):
        os.makedirs(os.path.dirname(path_))
    if os.path.exists(path_):
        return True
    response = requests.get(url_)
    if response.status_code == 200:
        if response.headers['Content-Type'].startswith('text'):
            with open(path_, 'w', encoding="utf-8") as file:
                file.write(response.text)
        else:
            with open(path_, 'wb') as file:
                file.write(response.content)
        print(f"\033[32m已下载 \033[0m文件链接 {url_}, 文件路径 {path_}")
        return True
    else:
        print(f"\033[91m无法下载文件链接：状态码：\033[0m{response.status_code} \033[0m{url_}\033[91m")
        return False


def file_download_option():
    global sources_url_path_pairs
    global WITHOUT_DOWNLOAD
    def build_courseware_url_path(year):
        url_ = f'{BASE_URL}/OS/{year}/index.html'
        path_ = os.path.join(BASE_DIR, "OS", year, "index.html")
        return {url_: path_}

    year = input("无法下载的文件会提示。下载成功后提示“下载成功”\n" +
                            "通过选项下载对应年份课件，回车默认下载2023年课件，输入其他符号则退出\n" +
                            "\033[32mA\033[0m:2021 \033[32mB\033[0m:2022 "
                            "\033[32mC\033[0m:2023 \033[32mD\033[0m:All\n")
    year = KEY_YEAR.get(year, "Invalid")  # 将按键转化为年份

    if year == "ALL":
        for item in ['2021', '2022', '2023']:
            sources_url_path_pairs.update(build_courseware_url_path(item))
    elif year != "Invalid":
        sources_url_path_pairs.update(build_courseware_url_path(year))
        if year != "2023":
            WITHOUT_DOWNLOAD.append(f'{BASE_URL}/OS/2023/index.html')  # 避免在其他文件中误下载2023/index.html
    else:
        print("\033[91m输入非法，程序退出")
        sys.exit()


def file_download():
    global sources_url_path_pairs
    global WITHOUT_DOWNLOAD
    _url_path_pairs = {}
    while sources_url_path_pairs:
        _url_path_pairs.update(sources_url_path_pairs)
        sources_url_path_pairs.clear()
        for _url, _path in _url_path_pairs.items():
            if download(_url, _path) == True:
                file_analyse(_path)
            else:
                WITHOUT_DOWNLOAD.append(_url)
                

# 提取每个文件中的链接
def file_analyse(filepath):
    global sources_url_path_pairs
    global WITHOUT_DOWNLOAD
    # 对非HTML文件不做分析
    if filepath.endswith(".html"):
        with open(filepath, 'r', encoding='utf-8') as file:
            content = file.read()
            soup = BeautifulSoup(content, 'html.parser')
    else:
        return

    # 提取文件中的相对链接
    _links_tags = soup.find_all(href=True) + soup.find_all(src=True) + soup.find_all(data=True)
    _links_attr = []
    for link in _links_tags:
        _links_attr.extend([link.get("href"), link.get("src"), link.get("data")])
    _links_attr = list(set(_links_attr))  # 去除重复的元素

    # 以filepath指定的文件为参照补全文件中的网址以及在本地存储的地址
    for link in _links_attr:
        if link is None or link.startswith(("http", "data")):  # data是ipynb.html文件资源
            continue
        link = urlparse(link).path  # 清除锚点
        absolute_path = get_full_path(filepath, link)
        if absolute_path is None:
            continue
        url =  get_full_url(absolute_path)  # 链接中只会有'/'而不会有'\'
        if url not in WITHOUT_DOWNLOAD:
            sources_url_path_pairs.update({url: absolute_path})
            WITHOUT_DOWNLOAD.append(url)


def file_fix():
    filedir = os.path.join(BASE_DIR, "OS", "2023", "build")
    if os.path.exists(filedir) and os.path.isdir(filedir):
        filename = os.listdir(filedir)
        for item in filename:
            filepath = os.path.join(BASE_DIR, "OS", "2023", "build", item)
            with open(filepath, 'r', encoding='utf-8') as file:
                content = file.read()
            change = re.sub(r'/OS/2023/slides/', '../slides/', content)

            with open(filepath, 'w', encoding='utf-8') as file:
                file.write(change)


if __name__ == "__main__":
    file_download_option()
    file_download()
    file_fix()
