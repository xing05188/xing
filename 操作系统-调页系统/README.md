# 调页系统模拟器
- 2351289周慧星

## 一、简介
本调页系统模拟器是一个基于Qt框架开发的图形化页面置换算法模拟程序。它提供了直观的用户界面，允许用户设置模拟参数（如内存块数量、总指令数、置换算法和模拟速度），并实时观察不同页面置换算法（FIFO、LRU、Clock和Random）的工作过程和性能表现。通过可视化展示内存块状态和页表信息，帮助用户深入理解页面置换算法的原理。

## 二、功能特性
1. **多算法支持**：实现了FIFO、LRU、Clock和Random四种常见的页面置换算法。
2. **参数可定制**：用户可以自由调整内存块数量、总指令数和模拟速度。
3. **实时统计**：模拟过程中实时显示已执行指令数、缺页次数和缺页率。
4. **可视化界面**：以图形化方式展示内存块状态和页表信息。
5. **模拟控制**：支持开始、暂停、重置和单步执行模拟操作。

## 三、文件结构
```
.
├── main.cpp             # 程序入口文件
├── simulator.h          # 模拟器类头文件
├── simulator.cpp        # 模拟器类实现文件
├── memorywidget.h       # 内存块可视化控件头文件
├── memorywidget.cpp     # 内存块可视化控件实现文件
├── README.md            # 项目说明文档
```

## 四、运行
### 直接运行 exe
- 解压Release包，双击运行`os2.exe`。
### 本地部署
- 确保已安装Qt开发环境和C++编译器。
- 克隆或下载项目源代码。
- 编译项目。
- 运行生成的可执行文件。

## 五、使用说明
![界面](coses/image.png)
### 界面介绍
- **控制面板**：用于设置模拟参数和控制模拟过程。
  - **内存块数量**：可设置范围为2到8的内存块数量（代码可调）。
  - **指令总数**：可设置范围为100到500的总指令数（代码可调）。
  - **置换算法选择**：提供FIFO、LRU、Clock和Random四种算法供选择。
  - **模拟速度选择**：有慢速、中速和快速三种速度可选。
  - **开始**：启动模拟过程。
  - **暂停**：暂停当前模拟。
  - **重置**：重置模拟状态，包括指令数、缺页次数等。
- **统计信息**：显示模拟过程中的统计数据。
  - **已执行指令数**：显示当前已执行的指令数量。
  - **缺页次数**：显示模拟过程中发生的缺页次数。
  - **缺页率**：显示当前的缺页率。
- **可视化区域**：展示内存块状态和页表信息。
  - **内存块**：通过不同颜色的矩形表示内存块中存储的页面。
  - **当前指令信息**：显示当前执行的指令、所在页面和物理地址。
  - **页表状态**：显示页表中有效页面的映射关系。

### 操作流程
1. 在控制面板中设置所需的模拟参数。
2. 点击“开始”按钮启动模拟。
3. 模拟过程中，可使用“暂停”按钮暂停模拟，使用“重置”按钮重置模拟。
4. 观察统计信息和可视化区域，了解页面置换情况。

## 六、数据结构介绍

### 1. 页表项结构（PageTableEntry）
```cpp
struct PageTableEntry {
    bool valid;         // 有效位，表示该页是否在内存中
    int frame;          // 帧号，对应内存中的物理块
    bool referenceBit;  // 访问位，用于Clock算法
};
```
- **valid**：标记页面是否存在于内存中。
- **frame**：存储该页面在内存中对应的物理块编号。
- **referenceBit**：记录页面是否被访问过，用于Clock算法。

### 2. 内存块表示（memory数组）
```cpp
QVector<int> memory;  // 存储当前内存中的页面
```
- 数组大小等于内存块数量。
- 每个元素代表一个物理块，存储的是页面编号（-1表示空闲块）。

### 3. 算法特定数据结构

#### FIFO算法
```cpp
QList<int> fifoQueue;  // 记录页面进入内存的顺序
```
- 使用队列维护页面进入内存的先后顺序，队首元素是最早进入的页面。

#### LRU算法
```cpp
QMap<int, bool> lruMap;  // 记录页面最近使用情况
```
- 使用映射维护页面的访问顺序，每次访问页面时将其移至映射尾部。

#### Clock算法
```cpp
int clockHand;  // 时钟指针，指向当前检查的物理块
```
- 指针在内存块中循环移动，用于查找可置换的页面。

### 4. 指令序列
```cpp
QVector<int> instructionSequence;  // 存储生成的指令序列
```
- 每个元素表示一条指令对应的页面编号。

### 5. 统计信息
```cpp
int executedInstructions;  // 已执行指令数
int pageFaults;            // 缺页次数
double pageFaultRate;      // 缺页率
```
- 记录模拟过程中的性能指标。


## 七、算法解释

### 1. FIFO（先进先出）算法
FIFO算法的核心思想是，当需要置换页面时，选择最早进入内存的页面进行替换。它基于“先进先出”的原则，就像排队一样，先进入队列的元素会先被移除。

```cpp
// 处理FIFO缺页异常
void PageReplacementSimulator::handlePageFaultFIFO(int page)
{
    if (fifoQueue.size() < memoryBlocks) {
        // 有空闲块
        int frame = int(fifoQueue.size());
        memory[frame] = page;
    }
    else {
        // 需要置换
        int victim = fifoQueue.front();
        fifoQueue.pop_front();
        int frame = pageTable[victim].frame;
        pageTable[victim].valid = false;
        memory[frame] = page;
    }
    fifoQueue.push_back(page);
    // 更新页表
    pageTable[page].valid = true;
    pageTable[page].frame = memory.indexOf(page);
}
```
- **空闲块处理**：如果内存中还有空闲块（即`fifoQueue`的大小小于`memoryBlocks`），则将新页面放入空闲块中，并将该页面加入`fifoQueue`的尾部。
- **页面置换**：如果内存已满，从`fifoQueue`的头部取出最早进入的页面（即`victim`），将其从内存中移除（将对应的页表项的`valid`标志置为`false`），然后将新页面放入该位置，并将新页面加入`fifoQueue`的尾部。

### 2. LRU（最近最少使用）算法
LRU算法的核心思想是，当需要置换页面时，选择最近最少使用的页面进行替换。它基于“最近最少使用的页面在未来一段时间内也不太可能被使用”的假设。

```cpp
// 处理LRU缺页异常
void PageReplacementSimulator::handlePageFaultLRU(int page)
{
    if (lruMap.size() < memoryBlocks) {
        // 有空闲块
        int frame = int(lruMap.size());
        memory[frame] = page;
    }
    else {
        // 需要置换
        int victim = lruMap.begin()->first;
        lruMap.erase(lruMap.begin());
        int frame = pageTable[victim].frame;
        pageTable[victim].valid = false;
        memory[frame] = page;
    }
    // 更新页表和LRU记录
    pageTable[page].valid = true;
    pageTable[page].frame = memory.indexOf(page);
    lruMap[page] = true;
}
```
- **空闲块处理**：如果内存中还有空闲块（即`lruMap`的大小小于`memoryBlocks`），则将新页面放入空闲块中，并将该页面加入`lruMap`。
- **页面置换**：如果内存已满，从`lruMap`的头部取出最近最少使用的页面（即`victim`），将其从内存中移除（将对应的页表项的`valid`标志置为`false`），然后将新页面放入该位置，并将新页面加入`lruMap`。每次访问页面时，会将该页面移动到`lruMap`的尾部，表示它是最近使用的页面。

### 3. Clock（时钟算法）
Clock算法是一种近似LRU算法，它通过一个访问位（`referenceBit`）来记录页面是否被访问过，避免了LRU算法中维护页面访问顺序的开销。

```cpp
// 处理Clock缺页异常
void PageReplacementSimulator::handlePageFaultClock(int page) {
    int Frame = memory.indexOf(-1); // 空闲帧的索引
    if (Frame == -1) {
        // 没有空闲帧，使用Clock算法
        while (true) {
            int currentPage = memory[clockHand]; // 当前指针指向的页面
            if (pageTable[currentPage].referenceBit) {
                // 如果访问位为1，清除访问位并移动指针
                pageTable[currentPage].referenceBit = false;
                clockHand = (clockHand + 1) % memoryBlocks;  // 移动指针
            }
            else {
                pageTable[currentPage].valid = false;  // 清除旧页面的信息
                Frame = clockHand;  // 替换的帧位置
                break;
            }
        }
    }  
    // 设置新页面的信息
    memory[Frame] = page;
    pageTable[page].valid = true;
    pageTable[page].frame = Frame;
    pageTable[page].referenceBit = true;  // 设置访问位为1
    clockHand = (clockHand + 1) % memoryBlocks;  // 移动指针到下一个位置
}   
```
- **空闲帧处理**：首先检查内存中是否有空闲帧（即`Frame`是否为`-1`），如果有，则直接将新页面放入空闲帧中。
- **页面置换**：如果没有空闲帧，使用一个指针（`clockHand`）遍历内存中的页面。当指针指向的页面的访问位为`1`时，将该访问位清`0`，并将指针移动到下一个页面；当指针指向的页面的访问位为`0`时，将该页面置换出去，将新页面放入该位置，并将新页面的访问位设置为`1`。最后，将指针移动到下一个位置。

### 4. Random（随机算法）
Random算法的核心思想是，当需要置换页面时，随机选择一个页面进行替换。

```cpp
// RANDOM随机算法
void PageReplacementSimulator::handlePageFaultRANDOM(int page) {
    // 查找空闲帧
    int Frame = memory.indexOf(-1);
    if (Frame == -1) {
        // 没有空闲帧，随机选择一个页面进行替换
        Frame = QRandomGenerator::global()->bounded(0, memoryBlocks);
        int victimPage = memory[Frame];
        // 清除旧页面的信息
        pageTable[victimPage].valid = false;
    }  
    // 设置新页面的信息
    memory[Frame] = page;
    pageTable[page].valid = true;
    pageTable[page].frame = Frame;  
}
```
- **空闲帧处理**：首先检查内存中是否有空闲帧（即`Frame`是否为`-1`），如果有，则直接将新页面放入空闲帧中。
- **页面置换**：如果没有空闲帧，使用随机数生成器随机选择一个页面进行替换，将该页面从内存中移除（将对应的页表项的`valid`标志置为`false`），然后将新页面放入该位置。
