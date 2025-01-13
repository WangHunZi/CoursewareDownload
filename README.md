# 说明
这是一个[jyy老师](https://jyywiki.cn/)网站的本地镜像，方便离线浏览课件和代码示例。

# 步骤
可以通过下面的步骤在WSL中唤起nvim/cLion/vscode等等代码编辑器或者IDE（和老师视频中演示的那样）
如果只是浏览课件，并不需要唤醒IDE，那么只需要在打开Courseware文件夹下的index.html即可，下面的步骤可以忽略

1. 环境搭建
```pip
pip install -r requirements.txt
```

2. 执行
windows或者WSL下执行`python3 server.py`

3. 浏览器输入`http://127.0.0.1:8001/index.html`

4. 环境变量
Windows下要把IDE路径放置在环境变量中，并修改`server.py`中的
```python
""" 调用 文本编辑器或IDE 打开文件夹 """
IDE = "code"
subprocess.run([f"{IDE}", full], check=True)
```
中的`code`为对应你要在网页中点击唤起的IDE或文本编辑器（code对应vscode）
已经测试过：`nvim`，`clion`，`vscode`

# TODO
- [ ] 收集缺失的文件
