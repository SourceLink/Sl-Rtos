# 从零开始写rtos

# 一. build

在build目录下执行以下命令, 即可完成代码编译:

```
cmake .. -DBOARD_FAMILY=NUCLEO_STM32F746ZG && make
```

其中`cmake ..`是为了生产makefile, 其中`-DBOARD_FAMILY=NUCLEO_STM32F746ZG`是用来定义板级包名称, make是为了编译;

生成文件在`build/bin`目录下; 

修改顶层的CMakeLists.txt文件选择你想编译的`demo`： 


```
add_subdirectory(demo/proc_manager)
```


# 二. download

在linux平台下, 使用st-link进行程序的下载, st-link的linux环境搭建[视频请点这](https://www.bilibili.com/video/av76683533), [文档搭建请点这](https://sourcelink.top/2019/11/22/stlink-driver-install/);


- 方式1

使用下列指令进行`bin`文件烧写:

```
st-flash write bin/xxx.bin 0x8000000
```

- 方式2

因为cmake支持客制化命令, 在书写cmake的时候已经将方式1当做cmd写入, 所以可以使用如下命令烧写: 

```
make flash
```



## 视频教程

- [【从零开始写RTOS】01-起因](https://www.bilibili.com/video/av74074508)
- [【视频】02-必备技能](https://www.bilibili.com/video/av74081139)
- [【视频】03-操作系统原理](https://www.bilibili.com/video/av75258795)
- [【视频】04-stlink环境搭建](https://www.bilibili.com/video/av76683533)
- [【视频】05-汇编语言](https://www.bilibili.com/video/av77364549)
- [【视频】06-内核调度上](https://www.bilibili.com/video/av78234770)
- [【视频】07-内核调度下](https://www.bilibili.com/video/av78389762)
- [【视频】08-vscode开发stm32](https://www.bilibili.com/video/av78709442)
- [【视频】09-内核双向链表](https://www.bilibili.com/video/av79545686)

## 文档

- [【从零开始写RTOS】01-起因](https://sourcelink.top/)
- [【文档】02-必备技能](https://sourcelink.top/2019/10/31/rtos-need-condition/)
- [【文档】03-操作系统原理](https://sourcelink.top/2019/11/10/os-principle/)
- [【文档】04-stlink环境搭建](https://sourcelink.top/2019/11/22/stlink-driver-install/)
- [【文档】05-汇编语言](https://sourcelink.top/2019/11/27/rtos-assembly/)
- [【文档】06-内核调度](https://sourcelink.top/2019/12/05/rtos-kernel-managmer/)
- [【文档】07-vscode开发stm32](https://sourcelink.top/2019/12/08/vscode-rtos/)
- [【文档】08-内核双向链表](https://sourcelink.top/2019/12/16/kernel-list/)







