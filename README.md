LCUI-2DFightGame
================

一个和《热血格斗》大致一样的格斗游戏。

源代码基于 GNU通用公共许可协议第二版(GPLv2) 发布，源文件中的版权声明会在以后修改时添加。

## 构建

在 GNU/Linux 平台上，请先确认你已经安装了 LCUI 库，然后在游戏源码根目录中使用命令：

```shell
	./configure
	make
```

生成的游戏主程序会在src里，名为game，请将它移动至bin目录下，然后运行。

在 windows 平台上，你需要准备VisualStudio，2010或2012版都行，进入build目录中的相应
目录，打开sln文件，然后用VisualStudio生成即可，生成的可执行文件在bin目录下。

## 效果图
![screenshort 1](http://lcui.org/files/images/game/2013-11-15-21-54-36.png)
![screenshort 2](http://lcui.org/files/images/game/2013-11-15-22-02-07.png)
