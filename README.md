# 从零开始写rtos

# 一. build

在build目录下执行以下命令, 即可完成代码编译:

```
cmake .. && make
```

其中`cmake ..`是为了生产makefile, make是为了编译;

生成文件在`build/bin`目录下


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
