# 说明
这是一个[jyy老师](https://jyywiki.cn/)网站的本地镜像，方便离线浏览课件和代码示例。

# 步骤
可以通过下面的步骤在网页中唤起nvim/cLion/vscode等等文本编辑器或者IDE（和老师视频中演示的那样）

已经测试过：`nvim`，`clion`，`vscode`

如果只是浏览课件，并不需要唤醒IDE，那么只需要在打开Courseware文件夹下的index.html即可，下面的步骤可以忽略。

1. 环境变量

Windows下要把IDE或者文本编辑器路径放置在环境变量中，并修改`server.py`中的
```python
""" 调用 文本编辑器或IDE 打开文件夹 """
IDE = "code"
subprocess.run([f"{IDE}", full], check=True)
```
中的`code`为对应你要在网页中点击唤起的IDE或文本编辑器（code对应vscode）

2. 环境搭建
```pip
pip install -r requirements.txt
```

3. 执行

windows或者WSL下执行`python3 server.py`

windows下执行脚本，在浏览器访问下面网址时，点击会唤起windows下的IDE

WSL下执行脚本，也可以在windows浏览器访问下面网址，但点击会唤起WSL下的IDE

4. 浏览器输入`http://127.0.0.1:8001/index.html`


# TODO
- [ ] 收集缺失的文件

```markdown
https://jyywiki.cn/pages/OS/img/GET-square.jpg (from https://jyywiki.cn/OI/linear-algebra.slides.html)
https://jyywiki.cn/OS/2024/slides/img/visicalc.webp (from https://jyywiki.cn/OS/2024/slides/1.3.html)
https://jyywiki.cn/OS/2024/slides/img/just-for-fun.webp (from https://jyywiki.cn/OS/2024/slides/1.4.html)
https://jyywiki.cn/OS/2024/slides/img/sc.webp (from https://jyywiki.cn/OS/2024/slides/5.4.html)
https://jyywiki.cn/OS/2024/slides/img/wmo.webp (from https://jyywiki.cn/OS/2024/slides/5.4.html)
https://jyywiki.cn/OS/2024/slides/img/the-world.webp (from https://jyywiki.cn/OS/2024/slides/6.1.html)
https://jyywiki.cn/OS/2024/slides/img/homura.webp (from https://jyywiki.cn/OS/2024/slides/6.1.html)
https://jyywiki.cn/OS/2024/slides/img/cyber-human.webp (from https://jyywiki.cn/OS/2024/slides/6.2.html)
https://jyywiki.cn/OS/2024/slides/img/first-bug.webp (from https://jyywiki.cn/OS/2024/slides/8.1.html)
https://jyywiki.cn/OS/2024/slides/img/orchestra.webp (from https://jyywiki.cn/OS/2024/slides/9.1.html)
https://jyywiki.cn/OS/2024/slides/img/locker.webp (from https://jyywiki.cn/OS/2024/slides/10.1.html)
https://jyywiki.cn/OS/2024/slides/img/locker-2.webp (from https://jyywiki.cn/OS/2024/slides/10.1.html)
https://jyywiki.cn/OS/img/dgx-1.webp (from https://jyywiki.cn/OS/2024/slides/11.4.html)
https://jyywiki.cn/OS/2024/slides/img/get.webp (from https://jyywiki.cn/OS/2024/slides/13.4.html)
https://jyywiki.cn/OS/2024/slides/img/linux-onion.webp (from https://jyywiki.cn/OS/2024/slides/17.1.html)
https://jyywiki.cn/OS/2024/slides/img/linker.webp (from https://jyywiki.cn/OS/2024/slides/19.2.html)
https://jyywiki.cn/OS/2024/slides/img/PATHCOV.webp (from https://jyywiki.cn/OS/2024/slides/23.3.html)
https://jyywiki.cn/OS/2024/slides/img/8088-mb.webp (from https://jyywiki.cn/OS/2024/slides/26.1.html)
https://jyywiki.cn/OS/img/FAT-number.webp (from https://jyywiki.cn/OS/2024/slides/28.3.html)
https://jyywiki.cn/OS/img/FAT-dent.webp (from https://jyywiki.cn/OS/2024/slides/28.3.html)
https://jyywiki.cn/OS/2024/slides/img/rosetta-stone.webp (from https://jyywiki.cn/OS/2024/slides/29.1.html)
https://jyywiki.cn/OS/2024/slides/img/silica-talk.webp (from https://jyywiki.cn/OS/2024/slides/29.1.html)
https://jyywiki.cn/OS/2024/slides/img/dian-nao.webp (from https://jyywiki.cn/OS/2024/slides/29.1.html)
https://jyywiki.cn/pages/OS/img/logo-text-white.svg (from https://jyywiki.cn/pages/OS/img/sel4-logo.svg)
https://jyywiki.cn/static/Virgil.woff2 (from https://jyywiki.cn/pages/ISER/2021/slides/img/static-analysis.svg)
```
