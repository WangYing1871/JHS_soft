#见微 禁核试TPC 离线数据分析代码
  数据解包
  基线处理
  配置文件生成
  简单分析

#依赖
  CERN-ROOT
  Boost
  c++ 标准库
  cmake（构建工具）

# 编译
  mkdir your/path/of/build
  cd your/path//of/build
  cmake path/of/this/project
    (如果需要指定库位置，可使用：cmake 选项 -DCMAKE_PREFIX_PATH 
     i.g: cmake ../ -DCMAKE_PREFIX_PATH=your/path/of/boost_1_84_0-build/lib/cmake/boost_program_options-1.84.0/)
  make -j*

# 功能
  JHS：解包、生成电子学配置文件、分析（这部分功能后被分开）
  script/draw_wave: 查看原初数据波形
  script/draw_time_dis: 按照时间分布重新划分事例
  script/draw_hitmap: 简单分析

# 使用例
  电子学采集基线文件： baseline.dat
  电子学采集数据文件： trigger.dat
  ./JHS -M 0 -f your/path/of/baseline.dat
  生成文件在当前目录calc下 可见baseline的信息

  ./JHS -M 1 -f your/path/of/trigger.dat
  （笔者机器上10G数据约用时90s）
  生成文件在your/path/of/trigger_entry.root

  [script下可执行文件未使用命令行参数解析器，请按照数量和顺序输入]
  ./script/draw_wave trigger_entry.root
  部分波形存储到wave.root中

  ./script/draw_time_dis your/path/of/trigger_entry.root ./calc/baseline_prestal.txt 5000 ./calc/mapping1029.txt
  （笔者机器上10G数据约用时140s）
    参数说明：
       ./calc/baseline_prestal.txt: 基线文件
       5000: unit: ns 表明将多宽时间窗口的波形看作一个事例
       ./calc/mapping1029.txt 电子学通道和探测器通道对应关系
     结果存储到timevsadc.root 

# 数据格式说明
  采用rootcint与TTree/TBranch 数据结构在 inc/data_strcut_cint.h中（如需要增加，请在cmake前同时修改 inc/LinkDef.h)
  struct entry_new 记录解包后数据结构
    event_id: 事例号
    fec_ids: 板号分布
    hit_channel_nos： 空，忽略
    time_stamps: 时间戳 unit: ns
    global_ids: 通道绝对id: gid=电子学板号<<8+电子学通道号
    adcs: 若干1024长度的数组的数组(12bits adcs 按照uint16_t存储)

  struct entry_tm 排序后数据结构
    det_ids：探测器通道表
    start_times：起始时间
    adcs: 同entry_new 


# 配置文件生成
    保证argv.ini 文件与可执行文件 JHS 同路径
    注意: 只负责生成配置文件，配置电子学请暂时请通过采数软件
    argv.ini文件说明：
      # 号行注释
    [config] 下：
      enable: =T表示功能使能
      filename: 基线文件
      sigma-compres: 压缩阈值sigma倍数
      L<*>C<*>S<*>=: 具体某通道压缩sigma倍数
      TT L<*>C<*>ADC<X>: =T 具体某通道触发阈值设置为X
      Rise Step Trigger Threshold: 波形上升幅度(为筛选有效波形)
      NHitChannel: 每层过阈通道数计数
      config: 配置文件生成位置

   './JHS -c' to generate config files

#常见问题、错误解决
  各类编译、链接错误：编译器版本过低、未安装依赖软件或不在环境中
  解包事例数与存储事例数不同：数据包打包错误，暂时未解决
