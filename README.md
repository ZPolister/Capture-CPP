## Dian-Capture （点截）

点~~（巅）~~截，一个简单的截图软件。

![dian](docs/0.jpg)

### 功能特性

#### 基础截图

![image-20241013232644724](docs/1.png)

#### 图片基础修改

可添加以下内容（包括自定义颜色）：

- 线段
- 矩形
- 圆
- 文字

![](docs/2.png)

#### 图片Pin

使用**鼠标中键**即可将图片钉住

![](docs/3.png)

#### OCR识别*

提供了OCR识别接口，可对图像文本提供本地识别服务。

![image-20241013233843472](docs/4.png)

*：本服务为插件式实现服务，需要运行项目中python代码，可见OCR服务配置指南

#### PicGo系列上传接口接入*

支持[PicGo](https://picgo.github.io/PicGo-Doc/)中图片上传，可以直接上传截图并直接复制url，同时支持md格式url快速复制（在设置中开启）

*：本服务需要安装配置PicGo，详见[PicGo下载安装](https://picgo.github.io/PicGo-Doc/zh/guide/#下载安装)

#### 快捷键支持

提供了完整的快捷键支持，可见以下：

- F1：全局唤醒
- Enter：复制截图到剪切板
- S：保存截图
- P：绘制矩形
- L：绘制直线
- M：绘制椭圆
- T：添加文字
- O：OCR识别
- U：截图上传
- ESC/鼠标右键：退出

#### 更多

- 兼容多显示器截图
- i18n不完整支持
- 支持打开文件直接钉住
- more...

### OCR服务运行指南

#### 环境需求

- python 3.11（其他版本不保证兼容）
- AnaConda（可选，推荐）

#### 安装方法

1. clone代码

   ```bash
   git clone https://github.com/ZPolister/Capture-CPP
   ```

2. 新建并进入Conda环境（不使用Conda可忽略）

   ```bash
   conda create -n ocr-service python=3.11
   conda activate ocr-service
   ```

3. 切换到ocr目录下，加载依赖

   ```bash
   cd Capture-CPP
   pip install -r requirements.txt
   ```

4. 运行项目

   ```bash
   python main.py
   ```

5. That's all.

### 开发环境

- C++11
- Qt 5.15
- MingW 8.1 / MSVC2017

### Thanks

- [Qt | 软件开发全周期的各阶段工具](https://www.qt.io/zh-cn/)
- [Molunerfinn/PicGo](https://github.com/Molunerfinn/PicGo)
- [RapidAI/RapidOCR](https://github.com/RapidAI/RapidOCR)
- **And You**

### ISSUE

反馈请直接提issue即可，欢迎fork & mr。

~~看到这里的话，这只是个小课设，没什么含金量，浪费大家时间了（逃~~