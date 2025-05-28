#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

#include <QWidget>
#include <QVector>

class MemoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MemoryWidget(int blockCount = 4, QWidget* parent = nullptr);  // 构造函数，默认4个内存块
    void updateBlocks(const QVector<int>& blocks);     // 更新内存块状态
    void updateBlockCount(int newCount);  

protected:
    void paintEvent(QPaintEvent* event) override;  // 绘制事件

private:
    int blockCount;      // 内存块数量
    QVector<int> blocks;  // 内存块状态，-1表示空块
};

#endif 
