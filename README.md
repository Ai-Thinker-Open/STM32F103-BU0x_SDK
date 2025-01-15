# 简介

BU03-Kit 是由深圳市安信可科技有限公司开发的一款 UWB 开发板。该开发板是基
于 BU03 收发模组，搭载一颗 ST 主控设计而成的一款测试评估板。其上的 BU03 模组
集成了板载天线，RF 电路，电源管理。 BU03-Kit 可以用于双向测距或 TDOA 定位系
统中，定位精度可达到 10 厘米，并支持高达 6.8 Mbps 的数据速率。可广泛应用于物
联网(IoT)、移动设备、可穿戴电子设备、智能家居等领域。如需开放所有代码，请联系安信可官方：<https://www.ai-thinker.com/>

## **温馨提示：** STM32F103-BU0x_SDK 只适用于安信可BU03或BU04系列模组或开发板

### 更多教程请参考：[UWB 使用指南](https://fcniufr8ibx1.feishu.cn/wiki/space/7454451041846034460?ccm_open_type=lark_wiki_spaceLink&open_tab_from=wiki_home)

# 芯片架构

![alt text](doc/img/chip.png)

# 目录结构

| 文件夹| 描述 | 备注 |
| :-----------: | :---: | :---: |
| components         |  组件             | /|
| components/app     | 应用程序          | /   |
| doc                | 直到文档和图片     | /    |
| components/hal     | 驱动库            | / |
| components/main    | 入口函数          | /   |
| components/Examples | 示例例程 | / |
| projects           | 工程文档          | / |

# 环境搭建

使用Windows平台来搭建开发环境。

## keil mdk 下载

Keil MDK（Microcontroller Development Kit）是一个用于开发基于 ARM Cortex-M 系列微控制器的综合集成开发环境（IDE）。它包含代码编辑、编译、调试等工具，主要用于嵌入式系统开发。建议前去官方网站下载最新版本的mdk-arm:**<https://www.keil.com/download/product/>**

![alt text](doc/img/keilmdk.png)

## keil mdk 安装完成

Keil MDK 安装完成后，用户将获得一个功能齐全的集成开发环境（IDE），用于开发基于 ARM Cortex-M 微控制器的嵌入式应用程序。

![](doc/img/keil_v5.PNG)

## stm32f1 keil扩展包 下载

STM32F1 Keil 扩展包是用于支持 STM32F1 系列微控制器的 Keil MDK 软件包，帮助开发人员在 Keil 环境中轻松开发基于 STM32F1 的嵌入式应用。扩展包包含设备相关的启动代码、驱动程序、示例代码以及其他开发资源。建议前去官方网站下载最新版本的扩展包:**<https://www.keil.arm.com/packs/stm32f1xx_dfp-keil/boards/>**

![alt text](doc/img/keilpack.png)

## stm32f1 keil扩展包 安装

STM32F1的Keil扩展包安装是STM32微控制器开发过程中的一个重要步骤，它允许开发者在Keil MDK-ARM这一集成开发环境（IDE）中方便地开发、编译和调试基于STM32F1系列微控制器的应用程序。

## SDK 克隆

### Github

```
git clone https://github.com/Ai-Thinker-Open/STM32F103-BU0x_SDK.git
```

### Gitee

```
git clone https://gitee.com/Ai-Thinker-Open/STM32F103-BU0x_SDK.git
```

## SDK 打开

找到拉下来的工程，在\Projects\USER目录下面找到Project.uvprojx文件双击打开工程

![alt text](doc/img/keilopen.png)

## SDK 编译

点击下图中的 **第2个**按钮 `Rebuil` 重新编译即可。

![!\[\](doc/img/projectbuild.png)](doc/img/keilbuild.png)

### 编译成功示例

```
... other log
compiling stm32f10x_tim.c...
compiling stm32f10x_wwdg.c...
compiling system_stm32f10x.c...
linking...
Program Size: Code=65980 RO-data=4304 RW-data=5196 ZI-data=7596  
FromELF: creating hex file...
".\Output\Project.axf" - 0 Error(s), 0 Warning(s).
```

### 编译报错 ArmClang: error: unsupported option '--c99'

错误原因是当前软件版本不支持C99编译器。

解决办法：Keil主界面点击魔术棒对话框中选择Target,右侧ARM Compiler发现当前版本是6，将版本更改为5，再次编译就不会出现C99错误了
![alt text](doc/img/keildebug-0.png)

#### 【自行添加AC5(ARMCC)编译器】
解压ARMCC（即MDK5.36）到Keil_v5\ARM目录下

（MDK5.36难以下载 已上传到本仓库）
![alt text](doc/img/keildebug-3.png)
点击Manage Project Items
![alt text](doc/img/keildebug-1.png)
在Folders/Extensions标签下，点击右侧省略号添加之前复制的ARMCC文件夹，这样子以来我们的MDK5.37就有Arm Compiler 5了
![alt text](doc/img/keildebug-2.png)

#### 【Keil MDK 540版本安装出现unknown compiler编译错误解决办法】
原因：arm在这个CMSIS上故意加上了版本号的限制，直接导致v5.06无法使用。
解决办法：卸载掉CMSIS的v6.1.0版本，再编译工程即可
![alt text](doc/img/keildebug-4.png)

## 程序烧录

    程序烧录有多种方式，比如：keil烧录、ST link烧录、j-flash烧录等等等。这里选用j-flash烧录介绍。

### j-flash 下载

    JFlash 是 Segger 公司开发的一款用于编程和调试嵌入式系统闪存的工具软件，常用于对各种微控制器和存储器进行固件编程。
    建议前去官方网站下载对应版本的烧录软件:**<https://www.segger.com/downloads/jlink/>**

![alt text](doc/img/jflash.png)

### j-flash 安装

    对下载的jflash进行安装，安装完成之后的j-flash界面。

![alt text](doc/img/jflashopen.png)

### 打开j-flash lite软件

    打开j-flash lite软件

![alt text](doc/img/jflashlite.png)

### j-flash lite 选择STM32F103T8芯片

    选择stm32f103t8芯片

![alt text](doc/img/jflashstm32f103t8.png)

### j-flash lite 加载烧录文件

    加载烧录文件

![alt text](doc/img/jflashloadprogram.png)

### j-flash lite 烧录

    烧录

![alt text](doc/img/jflashprogram.png)

# 常见问题

## 编译失败

可能会出现编译失败问题，目前主要原因为芯片型号选择错误，请选择正确的芯片型号，固定型号为：STM32F103CB
