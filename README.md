# learn_opengl

## 介绍
这是一个用来学习 **OpenGL** 的仓库，本仓库对应的[视频教程](http://https://www.bilibili.com/video/BV1Ni4y1o7Au)来自著名油管博主 [**The Cherno**](http://https://www.youtube.com/@TheCherno)

## 想要查看某节课对应的代码？
该仓库提交历史有对应介绍里的视频教程某一节的代码

所以在 **git 提交历史**里你可以查看某节课的代码

基本上都吻合，有几节课是分多次提交

## 如何构建？
该项目使用流行的 **cmake** 构建，**lib** 目录下是相关依赖的二进制文件

目前仅支持 **Windows** 平台

下面是为不熟悉 **cmake** 的朋友准备的从零开始的简单构建教程：

1. 下载 **C++ 编译工具链**（推荐 MSVC 或 LLVM）

2. 下载 [**cmake**](http://https://cmake.org/download/)

3. 下载 [**VSCode**](http://https://code.visualstudio.com/download)

4. 在扩展里分别搜索 **cmake tools、Chinese** 并安装

![](/assets/1.png) ![](/assets/2.png)

5. 点击左侧新出现的 **cmake** 图标

6. 点击最上面的 **项目状态**

7. 点击配置下第一个选择工具包右边的**铅笔图标**

![](/assets/3.png)

8. 选好你的编译器

9. 点调试或启动并等待项目编译完成

## 更简单的构建方式
1. 下载 [**xmake**](http://https://xmake.io/#/zh-cn/guide/installation)
2. 在项目根目录打开一个命令行
3. 输入 xmake 并回车

