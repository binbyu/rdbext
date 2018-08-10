# rdbext
It is used to extract tencent PC app RDB resource file

本程序用于提取腾讯PC端应用的RDB类型资源文件。如：QQ2018（仅供学习参考）
以命令行方式运行，命令如下：
rdbext.exe res.rdb
运行后会在当前目录创建文件夹，并把提取的内容保存在该文件夹下面。文件夹的名字与输入的rdb文件名字保持一致

解析算法：
1. 前36个字节为数据头，我抽象为如下结构体：
typedef struct  
{
    s8      flag[16];         // 固定值: 531E98204F8542F0
    s32     item_count;       // rdb里面包含多少个文件
    s64     header_size;      // 本数据头的大小，固定：36个字节
    s64     item_name_size;   // “2.中间文件名”数据的大小
} rdb_header_t; // 36 bytes

2. 中间是文件名数据，我抽象为如下结构体：
typedef struct
{
    //s8*     item_name;            // 最前面是名字，后面才是数据，名字以wchar_t 0结尾。 干嘛不把header放前面？不知道为啥要这么设计
    s64     item_content_offset;    // 文件数据的偏移位置
    s64     item_content_size;      // 文件数据的大小
} item_name_header_t;

3. 后面是文件数据



其他：
本程序没有测试，只是简单运行了一次
算法参考：https://www.52pojie.cn/thread-708173-1-1.html
