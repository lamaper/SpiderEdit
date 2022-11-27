# SpiderEdit

A fantastic tools for teachers who can analyse scores easily

迄今为止最好用的学生错题管理系统

author：(lamaper)[https://github.com/lamaper]、(LittFlower)[https://github.com/LittFlower]、 (illustager)[https://github.com/illustager]

team：E-butx

## 一、项目结构

程序基于Qt5.12.12开发，结合了python3和MySQL，在实现过程中，Qt主要用于窗口开发、基础结构搭建，python负责通信C++Qt和MySQL。

程序分为SpiderEdit和welcomepage，其中SpiderEdit是主程序，welcomepage是欢迎页面。

我们使用的编译器是MinGW64

## 二、功能

程序实现了分班管理学生成绩和错题。使用前用户需要创建班级并导入班级名单，之后添加错题集，需要注意的是，前面所说的错题集需要囊括可预见的所有错题，在后期使用过程中，用户只需要给相应学生添加错题集中的错题就可以完成登记。

依靠这样的功能可以实现查询学生在某些考试中错题类型的汇总。

目前本项目实现的功能有： 

新建班级、学生；

新建知识点；

数据库相关操作，包括导出已有数据库；

 一位用户（老师）同时管理多个班级； 

导入学科所有知识点及其对应编号；

导入学生错误的知识点（支持 Excel 快捷导入） 

查看学生考点错误频次。

## 三、使用

我们提供了编译好的版本，在release中。由于才疏学浅，我们并没有掌握Qt静态编译的方式，加上不缺保每个用户电脑中都安装有python，因而我们的安装包和最终程序大小有一定偏大，我们会在后期解决这个问题。

## 四、TODO

增加查询排序功能。用户可以对查询后的错题编号、数量进行排序。

增加考试增删改系统。

增加条形图饼状图成绩分析。

增加一键生成学习报告功能。

增加用户模块。

改进UI界面。

重新设计使用逻辑。

重构数据库代码，弃用python。

调整编译选项，缩小程序体积。

## 五、联系我们

目前作者们均为高中生，时间有限，如遇到任何问题可以通过lamaper@qq.com联系，或直接issue。

## 六、鸣谢

西安铁一中滨河高级中学-张德志老师（提供了idea）

西安铁一中滨河高级中学高2023届L2班全体师生（测试与其他帮助）

E-Butx（我们的团队）

