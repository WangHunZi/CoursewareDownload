import os
import subprocess
import markdown
from flask import Flask, request, jsonify, send_from_directory

app = Flask(__name__, static_folder='Courseware', static_url_path='')

# 路径映射规则
PATH_MAPPING = {
    "demo": "os-demos",  # 将demo映射为os-demos
    "intro": "introduction",
}

# 基础路径（相对于 server.py 的位置）
BASE_PATH = f'{os.getcwd()}/Courseware/os-demos/'

@app.route('/<path:filename>')
def serve_static(filename):
    return send_from_directory('Courseware', filename)

@app.route('/api/localhost/', methods=['GET'])
def handle_api():
    action = request.args.get('action')
    path = request.args.get('path')
    for old, new in PATH_MAPPING.items():
        path = path.replace(old, new)
    full = os.path.normpath(os.path.join(BASE_PATH, path))

    if action == 'demo':
        try:
            if os.path.isdir(full):
                readme = os.path.normpath(os.path.join(full, 'README.md'))
                with open(readme, 'r', encoding='utf-8') as f:
                    content = f.read()
                return jsonify(markdown.markdown(content))
            else:
                return jsonify({"error": "Path not found"}), 404
        except Exception as e:
            return jsonify({"error": str(e)}), 500
    elif action == 'launch':
        try:
            """ 调用 文本编辑器或IDE 打开文件夹 """
            IDE = "code"
            subprocess.run([f"{IDE}", full], check=True)
            return jsonify({"status": "success", "message": f"Opened {full} in {IDE}"})
        except subprocess.CalledProcessError as e:
            return jsonify({"status": "error", "message": str(e)}), 500
        except Exception as e:
            return jsonify({"status": "error", "message": str(e)}), 500
    else:
        return jsonify({"error": "Invalid action"}), 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8001)
