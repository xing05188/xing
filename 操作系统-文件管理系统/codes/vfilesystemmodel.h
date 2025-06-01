#pragma once
#include <QAbstractItemModel>
#include "vfilesystem.h"

class VFileSystemModel : public QAbstractItemModel {
    Q_OBJECT
public:
    VFileSystemModel(VNode* root, QObject* parent = nullptr);
    ~VFileSystemModel() override;
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;  // 获取索引
    QModelIndex parent(const QModelIndex& child) const override;   // 获取父索引
    int rowCount(const QModelIndex& parent) const override;   // 获取行数
    int columnCount(const QModelIndex& parent) const override;  // 获取列数
    QVariant data(const QModelIndex& index, int role) const override;  // 获取数据
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override; // 列标题
    Qt::ItemFlags flags(const QModelIndex& index) const override;  // 获取标志
    VNode* nodeFromIndex(const QModelIndex& index) const;  // 从索引获取节点
    QModelIndex indexFromNode(VNode* node) const;   // 从节点获取索引

    // 文件系统操作
    VNode* root() const { return m_root; }  // 获取根节点
    void refresh(const QModelIndex& idx = QModelIndex());  // 刷新指定索引或根节点
    void beginModelChange();  // 开始模型变更
    void endModelChange();  // 结束模型变更
private:
    VNode* m_root;  // 根节点
};

// 只显示文件夹的Model
class VFolderOnlyModel : public VFileSystemModel {
    Q_OBJECT
public:
    using VFileSystemModel::VFileSystemModel;
    // 只统计文件夹
    int rowCount(const QModelIndex& parent) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;  
    void refresh(const QModelIndex& idx);
};
