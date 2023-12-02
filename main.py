import os
import sys
import requests
import re
import html
import time
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
name_time_count = {}
fail_download = []
sources_url_path_pairs = {}

def file_time():
    print(f"{'NAME':<20}{'TIME':>12}{'COUNT':>11}")
    for name, item in name_time_count.items():
        print(f"{name:<20} {'{:.2f}'.format(item[0]):>11} {str(item[1]):>10}")


def timecalc(func):
    def wrapper(*arg, **kwargs):
        global time_calc
        start = time.time()
        result = func(*arg, **kwargs)
        total = time.time() - start
        func_name = func.__name__.ljust(20)
        if func_name in name_time_count:
            item_list = name_time_count[func_name]
            item_list[0] += total
            item_list[1] += 1
        else:
            name_time_count[func_name] = [total, 1]
        return result
    return wrapper


# 通过某一文件路径以及文件中出现的相对链接，拼接成文件在本地的存储地址
@timecalc
def get_full_path(path, link):
    pathdir = os.path.dirname(path).replace("/", os.sep)    # 用os.sep替换文件中提取出来的路径（文件中的路径都是`/`)
    if link.startswith("/"):
        if link.find("../") != -1:  # 滤过`/path/to/file/../path/to/file`
            return None
        return os.path.join(BASE_DIR, link[1:].replace("/", os.sep))    # 舍弃第一个字符`/`
    elif link.startswith("../"):
        urlsplit = link.split("/")
        pathsplit = pathdir.split(os.sep)
        count = sum([-1 for item in urlsplit if item == ".."])  # 多个`../`这样的路径的处理
        return os.path.join(os.sep.join(pathsplit[:count]), os.sep.join(urlsplit[-count:]))
    else:
        return os.path.join(pathdir, link.replace("/", os.sep))


@timecalc
def get_full_url(path):
    return path.replace(BASE_DIR, BASE_URL).replace("\\", "/")  # url都是`/`，替换path中所有的`\`为`/`


@timecalc
def download(url, path):
    if not os.path.exists(os.path.dirname(path)):
        os.makedirs(os.path.dirname(path))
    if os.path.exists(path):
        return True
    response = requests.get(url)
    if response.status_code == 200:
        if response.headers['Content-Type'].startswith('text'):
            with open(path, 'w', encoding="utf-8") as file:
                file.write(response.text)
        else:
            with open(path, 'wb') as file:  # 非文本文件
                file.write(response.content)
        print(f"\033[32m已下载 \033[0m文件链接 {url}, 文件路径 {path}")
        return True
    else:
        fail_download.append([response.status_code, url, path])
        return False


@timecalc
def file_download_option():
    global sources_url_path_pairs
    global WITHOUT_DOWNLOAD
    KEY_YEAR = {'A': "2021", 'B': "2022", 'C': "2023", 'D': "ALL", '': "2023"}

    def build_courseware_url_path(year):
        url_ = f'{BASE_URL}/OS/{year}/index.html'
        path_ = os.path.join(BASE_DIR, "OS", year, "index.html")
        return {url_: path_}

    year = input("无法下载的文件会提示。下载成功后提示“下载成功”\n" +
                            "通过选项下载对应年份课件，回车默认下载2023年课件，输入其他符号则退出\n" +
                            "\033[32mA\033[0m:2021 \033[32mB\033[0m:2022 "
                            "\033[32mC\033[0m:2023 \033[32mD\033[0m:All\n")
    year = KEY_YEAR.get(year, "Invalid")  # 将输入转化为年份

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


@timecalc
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
    if fail_download:
        print("\033[91m无法下载如下文件：\033[0m")
        for code, url, path in fail_download:     # 输出失败的文件下载以及应该在本地存储的位置
            print(f"\033[91m状态码：\033[0m{code} \033[0m{url}\033[91m 应存放：f{path}")


# 提取每个文件中的链接
@timecalc
def file_analyse(filepath):
    global sources_url_path_pairs
    global WITHOUT_DOWNLOAD
    # 对非HTML文件不做分析
    if filepath.endswith(".html"):
        with open(filepath, 'r', encoding='utf-8') as file:
            soup = BeautifulSoup(file.read(), 'html.parser')
    else:
        return

    # 提取文件中的相对链接
    links_tags = soup.find_all(href=True) + soup.find_all(src=True) + soup.find_all(data=True)
    links_attr = []
    for link in links_tags:
        links_attr.extend([link.get("href"), link.get("src"), link.get("data")])
    links_attr = list(set(links_attr))  # 去除重复的元素

    # 以filepath指定的文件为参照补全文件中的网址以及在本地存储的地址
    for link in links_attr:
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

@timecalc
def file_fix():
    filedir = os.path.join(BASE_DIR, "OS", "2023", "build")
    if os.path.exists(filedir) and os.path.isdir(filedir):
        filename = os.listdir(filedir)
        for item in filename:
            filepath = os.path.join(BASE_DIR, "OS", "2023", "build", item)
            with open(filepath, 'r', encoding='utf-8') as file:
                change = re.sub(r'/OS/2023/slides/', '../slides/', file.read())

            with open(filepath, 'w', encoding='utf-8') as file:
                file.write(change)

@timecalc
def file_decode():
    for root, dirs, files in os.walk(BASE_DIR):
        for item in files:
            if item.endswith(".html"):
                with open(os.path.join(root, item), 'r', encoding='utf-8') as file:
                    change = html.unescape(file.read())

                with open(os.path.join(root, item), 'w', encoding='utf-8') as file:
                    file.write(change)


if __name__ == "__main__":
    file_download_option()
    file_download()
    file_fix()
    file_decode()
    file_time()
