# 1️⃣如何用Template配置一个属于自己的新项目？
如果您认为重新创建工程过于麻烦，可以选择从原有的工程中做一些改变,变成您自己的空工程，如改成xxx工程。
- 文件夹名的更改：
  -'Template' -> 'xxx'
  -'Template.ioc' -> 'xxx.ioc'
  -'MDK-ARM\Template.uvprojx' -> 'MDK-ARM\xxx.uvprojx'
  -MDK-ARM文件夹中删除 **除** 'xxx.uvprojx'以外所有文件
  -用记事本打开'xxx.uvprojx'将所有的Template替换为xxx
  
- 'Template.ioc'重新生成工程

  
  
  
  

# 2️⃣clone已有的工程时拉取SRML的代码

 -  ```bash
    $ git submodule update --init --recursive

