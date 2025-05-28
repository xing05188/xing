#include "memorywidget.h"
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QFont>

MemoryWidget::MemoryWidget(int blockCount, QWidget* parent)
    : QWidget(parent), blockCount(blockCount), blocks(blockCount, -1)
{
    setMinimumSize(200, 350);
    setStyleSheet("background-color: f0f0f0;");
}

void MemoryWidget::updateBlocks(const QVector<int>& blocks)
{
    this->blocks = blocks;
    update();
}
void MemoryWidget::updateBlockCount(int newCount)
{
    if (newCount == blockCount) return;  // 数量未变，直接返回
    blockCount = newCount;
    blocks.resize(newCount,-1); 
    update();                
    adjustSize();            
}
void MemoryWidget::paintEvent(QPaintEvent* event)   // 绘制事件
{
    int recommendedHeight = 40 + blockCount * 50;  // 每个块高度50，留出边距
    setMinimumHeight(recommendedHeight);
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int width = this->width();
    int height = this->height();
    int blockHeight = (height - 40) / blockCount;

    // 绘制内存块
    for (int i = 0; i < blockCount; ++i) {
        int y = 30 + i * blockHeight;
        painter.setPen(QPen(QColor(150, 150, 150), 2)); 
        painter.drawRect(10, y, width - 20, blockHeight - 5);
        // 填充颜色表示不同页面
        if (blocks[i] != -1) {
            QColor color;
            color.setHsv((blocks[i] * 70) % 360, 150, 200);
            painter.fillRect(11, y + 1, width - 22, blockHeight - 7, color);
            // 显示页面号
            painter.setPen(QPen(Qt::black, 1));
            QFont font;
            font.setBold(true);
            font.setPointSize(12);
            painter.setFont(font);
            painter.drawText(220, y + blockHeight / 2 + 5, QString("Page %1").arg(blocks[i]));
        }
    }

    // 标题
    painter.setPen(QPen(Qt::black, 1));
    QFont font;
    font.setPointSize(14);
    painter.setFont(font);
    painter.drawText(220, 15, "内存块");
}