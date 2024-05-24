# Explorer

资源的结构，状态机表述

1. 资源层级性
2. 资源关联性   
2.1 组合关联  
2.2 映射变换关联   
2.3 介质关联，例如不同资源间消息介质，同资源缓存介质等  
3. 事件触发性
4. 状态以及状态机层级性

产品化 framework/engine
1. 为应用提供一套framework来更友好的使用kernel，包括SM framework，CS framework以及DDS等
2. 为kernel提供一套framework来更友好的使用BSP，包括SM framework等，其实kernel本身就是一种framework，所以为什么重新造轮子呢？
关键点在于，Kernel的种类太多，复杂内核配置太多，使用kernel framework可以配置合理的参数或是直接取代旧的内核。
linux好比是豪宅，freertos好比是凉亭，我们的设计是房子框架，由用户决定是豪宅还是凉亭。。。。
3. 其他的类似游戏引擎或是包管理引擎等
4. User`Guide，用户是应用开发者或内核开发者

产品化 硬件抽象层以及虚拟环境env
1. 硬件抽象层即BSP，为内核开发者提供一种使用硬件的方式
2. 虚拟环境，即env，包括hypervisor，kernel, 容器等，为framework提供相应的接口来使用硬件

除了上述的产品化方向外，还有货架产品，例如整机，用户是普通人
以及配套的硬件和软件解决方案

目前仅仅0-1还远远不够，在1-10的过程中产品化和产品力才是用户所需的。

Agent，输入用户需求，用于输出事务接口。

Agent: Input user requirements and output transaction interfaces.

所谓的事务接口，同传统OS的系统调用接口类似，区别在于该接口不是用来管理资源，而是告知AIOS去做什么。
例如，数据采集Agent，用户需求为"采集传感器数据，通过CAN总线，输出到文件中"，输出事务接口如下:
1. 任务<优先级, 响应时间，运行时间，截至时间>，例如，数据采集任务
2. 外设<标识(寄存器), 配置(设备特定), 数据地址, 源, 目的>，例如CAN，磁盘等


事务接口可以标准化，类似POSIX标准。事务接口标准是归纳用户需求，转换为较为通用的机器理解的事务集合的结果。

Transaction interfaces can be standardized, similar to POSIX standards. Transaction interface standards are the result of summarizing user requirements and translating them into a more general machine-understood transaction set.


张量资源管理，建立统一模型来描述和管理调度时间，内存地址空间，中断触发概率分布，网络带宽，磁盘分区/带宽，大量外设，通信（IPC/event）阻塞分布区域以及通信缓存带宽，同步阻塞分布区域以及事务性内存分布区域等

Tensor resource management, building a unified model to describe and manage scheduling time, memory address space, interrupt trigger probability distribution, network bandwidth, disk partition/bandwidth, a large number of peripherals, communication (IPC/event) blocking distribution and communication cache bandwidth, synchronization blocking distribution and transactional memory distribution

事务接口参数转换为上述"统一模型"的输入，"统一模型"会按照输入需求给出当前系统的资源分配输出。

Transaction interface parameters are converted into inputs to the above Unified model, which gives the resource allocation output of the current system based on input requirements.

机器学习调优，输入不同维度的张量数据（以上描述的），根据业务/用户约束，输出理论最优/近似最优的系统级决策。例如，在输入各类资源向量，且保证实时约束的前提下，输出任务调度策略，内存分配/释放/置换策略，中断分布策略，网络流量管控策略，磁盘IO管控策略，外设分配策略，通信分布策略/带宽管控策略，同步分布策略等

Machine learning tuning, input tensor data of different dimensions (described above), output theoretically optimal/near-optimal system-level decisions based on business/user constraints. For example, under the premise of input various resource vectors and ensuring real-time constraints, output task scheduling policies, memory allocation/free/replacement policies, interrupt distribution policies, network traffic management policies, disk I/O management policies, peripheral allocation policies, communication distribution policies/bandwidth management policies, synchronization distribution policies, etc

AIOS，机器学习算法驱动的设备资源管理器，做它的原因有，内核代码指数级增长的担忧，减少内核漏洞的人力成本的剧增（即使在seL4开发中，形式化验证脚本的成本远超内核设计），复杂的用例需求对内核的畸形改动，内核的多维度启发式参数以“小作坊”的方式进行优化等；而以上所有的担忧，通过机器学习的经验增长来解决

AIOS, a machine learning algorithm-driven device resource manager, is built for the exponential growth of kernel code, the dramatic increase in the human cost of reducing kernel vulnerabilities (even in seL4 development, the cost of formalizing validation scripts far exceeds the cost of kernel design), the need for complex use cases to make abnormal changes to the kernel, the multi-dimensional heuristic parameters of the kernel are optimized in a "small workshop" way. All of these concerns are being addressed by the growing experience of machine learning

# 'Nested + Combined' Model
嵌套，即可以将某个资源一层层分解下去，亦可以一层层封装上去；组合，即可以将某些资源以任意的输入请求进行组合。该模型在调度时间和内存地址空间等资源的抽象上表现良好。
例如，->作为分解符号，那么可以将系统的调度时间抽象为：CPU集群 -> SMP/AMP CPU -> 1时间元 -> 2时间元 -> 3时间元 -> 4时间元 -> 5时间元等。
++ 作为组合符号，那么可以将某一事务的调度时间抽象为：事务 = 某几个1时间元 ++ 某几个2时间元 ++ 某几个CPU时间元 ++ ...

内存也是如此，分解过程：NUMA节点 -> UMA节点 -> 区域 -> 空闲链表 -> 页 -> 对象，组合过程：事务 = 某几个内存对象 ++ 某些页 ++ 某几个区域 ++ 某几个节点 ++ ...

|对照/统一术语|时间|空间|...|
|----|----|----|----|
|集-set|CPU集群|NUMA节点|..|
|节点-node|CPU/VCPU(Hypervisor/Supervisor)vcpu_struct|UMA节点/VUMA     mm_struct|..|
|组-group|1st|区域|..|
|单元-cell|2st|空闲链表|..|
|项-item|3st|页/vma|..其他item细粒度|
|切片-slice|4st|对象|..|

执行实体：进程组->进程->子进程->线程
每一个执行实体可拥有无限的虚拟资源。
每一个执行实体需要变换到有限的物理资源。

Nesting, that is, a resource can be decomposed layer by layer, or can be encapsulated layer by layer; Composition, that is, some resources can be combined with arbitrary input requests. The model performs well in the abstraction of resources such as scheduling time and memory address space.
For example, -> As a decomposition symbol, then you can abstract the scheduling time of the system as: CPU cluster -> SMP/AMP CPU -> Process group time unit -> Process time unit -> thread time unit -> child process time unit -> child thread time unit.
++ as a combination symbol, then the scheduling time of a transaction can be abstracted as: transaction = several sub-process time units ++ several process time units ++ several CPU time units ++...

Memory is the same, decomposition process: NUMA node -> UMA node -> Region -> Free linked list -> page -> object, combination process: transaction = some memory objects ++ some pages ++ some areas ++ some nodes ++...

调度时间和内存二者之间的区别在于：
|资源类型|时间|空间|备注|
|----|----|----|----|
|模型向量类型|优先级向量+尺寸向量|<延迟>映射向量+尺寸向量|转换到另一套数据结构 - 统一序列号|
|资源申请策略|相对公平(轮询+FIFO),不公平(优先级单调抢占):在到达时刻前不可享有|绝对公平(FIFO):内存地址立刻享有或是延迟分配|延时参数|
|资源有限性|无限=>有限+周期|有限=>物理内存+热插拔内存+置换内存|流+窗口|
|资源持有策略|阶梯性(任务创建和销毁或部分时间片手动销毁)|长期性(除非置换/释放, 长期持有)|根据不同约束，可以从持有到非持有，生命周期lcc|
|资源操作|分配+释放+使用计数|分配+释放+使用计数|...|

笛卡尔积：

|模型向量类型|绘制曲线|扩展含义|
|----|----|----|
|序列号向量+尺寸向量转置|CPU利用曲线|CPU利用率, 响应度, 实时性, 负载均衡度等|
|寻址号向量+尺寸向量转置|内存利用曲线|内存利用率, 内存碎片率等|


LLM可以分析用户A语言，从知识库中获取相应的答案，并反馈给用户B；用户A可以是Agent CLI输入的用户需求（自然语言），而用户B可以是资源（物理硬件或Model，'Nested + Combined' Model仅是其中一种）；LLM->用户A可以是解析自然语言的过程，而LLM->用户B则是推理输出机器识别语言的过程（这里以事务表示）。LLM的角色是黑盒。知识库是训练和记忆的结果。
因此，根据当前分析的情况，需要以下基础设施：
1. Agent应用：CLI或人机交互的接口
2. LLM推理框架：解析自然语言，从知识库获取经验或将经验存储知识库
3. 知识库：经验的持久记忆
4. 事务原语：这里非常依赖OS背景经验，精炼成事务原语库（或标准），由事务驱动资源分配和释放等操作
5. 资源模型：该模型作为资源的抽象，可以根据资源属性，将资源数量，资源边界，资源效率等物理概念转换为数学概念
6. 资源驱动：底层OS管理最基本的物理硬件

模式：LLM（训练） ----> NC model ----> LLM'（参数校准）
联动：https://github.com/jiaowushuang/Newlk-OS中资源视图

# install

'Nested + Combined' Model for C:
```
cmake -S. -Bbuild && cmake --build build

./build/Playground_Bin

```

# issue
1. 是否该模型过于理想化了？需要待实验验证
2. 当前重点在于如何在原内存模型的基础上，实现时间模型
假设为无限长的时间约束一个超周期，在超周期内是有限分配的，但是在初始分配时间后，
何时使用该时间，是不确定的；不使用该时间后，是否回收该时间，回收的属性是什么；
回收后需要再次分配吗？
3. 所以，资源分配和使用的范式是会存在的吧？


# your ideas
1. 当前使用C语言编写'Nested + Combined' Model样例
2. 后续改用Rust优化'Nested + Combined' Model样例
3. 当前使用假设环境（CPU，内存，通信，同步等）进行模拟运行
4. 后续移植到Qemu/评估板硬件上进行验证
5. 其他物理资源的模拟 TBD
