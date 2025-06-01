#pragma once
#include <QMainWindow>
#include <QTreeView>
#include <QLineEdit>
#include <QToolBar>
#include <QTextEdit>
#include <QSplitter>
#include <QStack>
#include "vfilesystem.h"
#include "vfilesystemmodel.h"
#include "fileeditdialog.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent* event) override;  // 窗口关闭事件处理
private slots:
    void onTreeClicked(const QModelIndex&);  // 树形视图点击事件处理
    void openCurrentItem(); // 打开当前选中的文件或文件夹
    void onNew(bool);  // 新建文件或文件夹
    void onDelete();   // 删除文件或文件夹
    void onSort();    // 排序文件或文件夹
    void sortCurrentDir();  // 对当前目录进行排序
    void onRename();   // 重命名文件或文件夹
    void onCutOrCopy(bool); // 剪切或复制文件或文件夹
    void onPaste();   // 粘贴文件或文件夹
    void onRefresh();   // 刷新视图
    void onBack();     // 后退
    void onForward();   // 前进
    void onUp();      // 上一级目录
    void recursiveMatch(const QModelIndex& , const QString& , QModelIndexList& ); // 递归匹配文件或文件夹
    void onSearch();   // 搜索文件或文件夹
    void showContextMenu(const QPoint&);  // 显示右键菜单

private:
    enum SortField { SortByName, SortByTime, SortByType };// 排序字段
    SortField currentSortField = SortByName;  // 排序方式
    Qt::SortOrder currentSortOrder = Qt::AscendingOrder;  // 当前排序顺序
    VNode* vfsRoot;   // 根节点
    VFileSystemModel* vfsModel;  // list模型
    VFolderOnlyModel* folderModel; // tree模型
    QTreeView* treeView;  // tree视图
    QTreeView* listView;  // list视图
    QLineEdit* pathEdit;  // 路径编辑框
    QLineEdit* searchEdit; // 搜索编辑框
    QToolBar* toolbar;      // 工具栏
    QModelIndex currentIndex;  // 当前选中的索引
    QList<VNode*> clipboardNodes; // 批量复制/剪切
    bool isCopy = true;   // true为复制，false为剪切

    QStack<QModelIndex> backStack;    // 后退栈
    QStack<QModelIndex> forwardStack;  // 前进栈

    void setupUI();  // 设置UI界面
    void updateListView(const QModelIndex&); // 更新list列表视图
    void updatePathEdit(const QModelIndex&); // 更新路径编辑框
    QModelIndex getParentIndex(const QModelIndex& idx) const;  // 获取父节点索引
    void saveVFS();  // 保存虚拟文件系统
    void loadVFS();  // 加载虚拟文件系统
};