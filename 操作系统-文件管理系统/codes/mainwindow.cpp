#include "mainwindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QFileDialog>
#include <QCloseEvent>
#include <QMenu>
#include <QHeaderView>

#define SAVE_FILENAME "data.json"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    loadVFS();
    vfsModel = new VFileSystemModel(vfsRoot, this);
    folderModel = new VFolderOnlyModel(vfsRoot, this);
    this->resize(850, 500);
    setupUI();
    currentIndex = vfsModel->index(0, 0, QModelIndex()); // 默认显示根目录
    updateListView(currentIndex);
    updatePathEdit(currentIndex);
}

MainWindow::~MainWindow() {
    saveVFS();
    delete vfsRoot;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    saveVFS();
    event->accept();
}

void MainWindow::setupUI() {
    // 工具栏
    toolbar = addToolBar("MainToolBar");
    toolbar->addAction(QIcon("icons/back.png"), "回退", this, &MainWindow::onBack);
    toolbar->addAction(QIcon("icons/forward.png"), "前进", this, &MainWindow::onForward);
    toolbar->addAction(QIcon("icons/up.png"), "上移", this, &MainWindow::onUp);
    toolbar->addAction(QIcon("icons/refresh.png"), "刷新", this, &MainWindow::onRefresh);
    toolbar->addAction(QIcon("icons/open.png"), "打开文件", this, &MainWindow::openCurrentItem);
    toolbar->addSeparator();
    QAction* spacer = new QAction("   ", this);
    spacer->setEnabled(false); // 禁用点击
    toolbar->addAction(spacer);
    toolbar->addSeparator();
    toolbar->addAction(QIcon("icons/folder.png"), "新建文件夹", this, [this]() { onNew(true); });
    toolbar->addAction(QIcon("icons/file.png"), "新建文件", this, [this]() { onNew(false); });
    toolbar->addAction(QIcon("icons/cut.png"), "剪切", this, [this]() { onCutOrCopy(false); });
    toolbar->addAction(QIcon("icons/copy.png"), "复制", this, [this]() { onCutOrCopy(true); });
    toolbar->addAction(QIcon("icons/paste.png"), "粘贴", this, &MainWindow::onPaste);
    toolbar->addAction(QIcon("icons/rename.png"), "重命名", this, &MainWindow::onRename);
    toolbar->addAction(QIcon("icons/delete.png"), "删除", this, &MainWindow::onDelete);
    toolbar->addAction(QIcon("icons/sort.png"), "排序", this, &MainWindow::onSort);
    toolbar->addSeparator();
    QAction* spacer1 = new QAction("   ", this);
    spacer1->setEnabled(false); // 禁用点击
    toolbar->addAction(spacer1);
    toolbar->addSeparator();

    // 路径编辑框
    pathEdit = new QLineEdit(this);
    pathEdit->setReadOnly(true);
    toolbar->addWidget(pathEdit);
    toolbar->addSeparator();
    QAction* spacer2 = new QAction("   ", this);
    spacer2->setEnabled(false); // 禁用点击
    toolbar->addAction(spacer2);
    toolbar->addSeparator();

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("在C:中搜索");
    toolbar->addWidget(searchEdit);
    toolbar->addAction(QIcon("icons/search.png"), "搜索", this, &MainWindow::onSearch); // 搜索按钮

    //树视图
    treeView = new QTreeView(this);
    treeView->setModel(folderModel);
    treeView->setHeaderHidden(true);
    treeView->setRootIndex(QModelIndex());
    connect(treeView, &QTreeView::clicked, this, &MainWindow::onTreeClicked);  // 单击选择文件夹
    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    treeView->setColumnHidden(1, true); // 隐藏“修改时间”
    treeView->setColumnHidden(2, true); // 隐藏“类型”
    //列表视图
    listView = new QTreeView(this);
    listView->setModel(vfsModel);
    listView->setRootIndex(currentIndex);
    listView->setHeaderHidden(false); // 显示表头
    listView->setContextMenuPolicy(Qt::CustomContextMenu); // 右键菜单
    connect(listView, &QTreeView::doubleClicked, this, &MainWindow::openCurrentItem);  // 双击打开文件或文件夹
    connect(listView, &QTreeView::customContextMenuRequested, this, &MainWindow::showContextMenu); // 右键菜单
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    listView->header()->setSectionResizeMode(QHeaderView::Stretch); 
    listView->setSelectionBehavior(QAbstractItemView::SelectRows);

    // 设置分割器
    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(treeView);
    splitter->addWidget(listView);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 4);
    setCentralWidget(splitter);
}

void MainWindow::onTreeClicked(const QModelIndex& idx) {
    VNode* selNode = static_cast<VFileSystemModel*>(treeView->model())->nodeFromIndex(idx);
    QModelIndex vfsIdx = vfsModel->indexFromNode(selNode);
    if (vfsIdx == currentIndex) return;
    backStack.push(currentIndex);
    forwardStack.clear();
    currentIndex = vfsIdx;
    updateListView(vfsIdx);
    updatePathEdit(vfsIdx);
}

void MainWindow::openCurrentItem() {
    QModelIndex idx = listView->currentIndex();
    if (!idx.isValid()) return;
    VNode* node = vfsModel->nodeFromIndex(idx);
    if (!node) return;
    if (node->isDir) 
        onTreeClicked(idx);  // 如果是文件夹就切换视图
    else {
        // 如果是文件就编辑
        FileEditDialog dlg(node->name, node->content, this);
        if (dlg.exec() == QDialog::Accepted) {
            node->lastModified = QDateTime::currentDateTime();
            vfsModel->refresh(currentIndex);
        }
    }
}

void MainWindow::onNew(bool isFolder) {
    VNode* parentNode = vfsModel->nodeFromIndex(currentIndex);
    if (!parentNode) return;
    QString name;
    bool ok = false;
    while (true) {
        if(isFolder)
            name = QInputDialog::getText(this, "新建文件夹", "文件夹名：", QLineEdit::Normal, "", &ok);
		else
            name = QInputDialog::getText(this, "新建文件", "文件名：", QLineEdit::Normal, "", &ok);
        if (!ok || name.isEmpty()) return;
        // 判断重名
        bool exists = false;
        for (const VNode* node : parentNode->children) {
            if (node->name == name) {
                exists = true;
                break;
            }
        }
        if (exists)
            if(isFolder)
                QMessageBox::warning(this, "警告", "该目录下已存在同名文件夹，请重命名！");
			else
                QMessageBox::warning(this, "警告", "该目录下已存在同名文件，请重命名！");
        else 
            break; // 名字可用
    }
    vfsModel->beginModelChange();
    parentNode->children.append(new VNode(name, isFolder, parentNode));
    vfsModel->endModelChange();
    folderModel->layoutChanged();
    updateListView(currentIndex);
}

void MainWindow::onDelete() {
    QModelIndexList indexes = listView->selectionModel()->selectedRows();
    if (indexes.isEmpty()) return;
    // 按 row 倒序，防止删除时下标错乱
    std::sort(indexes.begin(), indexes.end(), [](const QModelIndex& a, const QModelIndex& b) {
        return a.row() > b.row();
        });
    vfsModel->beginModelChange();
    for (const QModelIndex& idx : indexes) {
        VNode* node = vfsModel->nodeFromIndex(idx);
        if (!node || !node->parent) continue;
        // 检查 currentIndex 是否在被删节点下
        VNode* curNode = vfsModel->nodeFromIndex(currentIndex);
        if (curNode == node || (curNode && curNode->parent == node)) {
            currentIndex = vfsModel->indexFromNode(node->parent);
            updateListView(currentIndex);
            updatePathEdit(currentIndex);
        }
        // 检查 clipboardNodes 里是否有被删节点或其子节点，移除
        for (int i = clipboardNodes.size() - 1; i >= 0; --i) {
            VNode* temp = clipboardNodes[i];
            while (temp) {
                if (temp == node) {
                    clipboardNodes.removeAt(i);
                    break;
                }
                temp = temp->parent;
            }
        }
        VNode* parent = node->parent;
        parent->children.removeOne(node);
        delete node; // 递归释放
    }
    vfsModel->endModelChange();
    folderModel->layoutChanged();
    updateListView(currentIndex);
}
void MainWindow::onRename() {
    QModelIndexList indexes = listView->selectionModel()->selectedRows();
    if (indexes.isEmpty()) return;
    QModelIndex curDirIndex = listView->rootIndex(); // 记录当前目录
    // 支持批量重命名（弹多次输入框）
    for (const QModelIndex& idx : indexes) {
        VNode* node = vfsModel->nodeFromIndex(idx);
        if (!node || !node->parent) continue;
        QString newName;
        bool ok = false;
        while (true) {
            newName = QInputDialog::getText(this, "重命名", QString("新名称（原：%1）：").arg(node->name), QLineEdit::Normal, node->name, &ok);
            if (!ok || newName.isEmpty()) break; // 取消或空，退出
            // 检查同目录下是否有重名，排除自己
            bool exists = false;
            for (const VNode* sibling : node->parent->children) {
                if (sibling != node && sibling->name == newName) {
                    exists = true;
                    break;
                }
            }
            if (exists)
                QMessageBox::warning(this, "警告", "该目录下已存在同名项目，请重命名！");
            else {
                vfsModel->beginModelChange();
                node->name = newName;
                node->lastModified = QDateTime::currentDateTime();
                node->type = node->isDir ? "文件夹" : (node->name.contains('.') ? node->name.section('.', -1).toUpper() + " 文件" : "文件");
                vfsModel->endModelChange();
                folderModel->layoutChanged();
                updateListView(curDirIndex);
                break; // 名字可用，退出循环
            }
        }
    }
}

void MainWindow::onCutOrCopy(bool CutOrCopy) {
    QModelIndexList indexes = listView->selectionModel()->selectedRows();
    clipboardNodes.clear();
    for (const QModelIndex& idx : indexes) {
        VNode* node = vfsModel->nodeFromIndex(idx);
        if (node) clipboardNodes.append(node);
    }
    isCopy = CutOrCopy; // true为复制，false为剪切
}

void MainWindow::onPaste() {
    if (clipboardNodes.isEmpty()) return;
    VNode* parentNode = vfsModel->nodeFromIndex(currentIndex);
    if (!parentNode || !parentNode->isDir) return;
    vfsModel->beginModelChange();
    if (isCopy) {
        // 复制模式用clone
        std::function<VNode* (VNode*, VNode*)> clone = [&](VNode* src, VNode* parent) -> VNode* {
            VNode* cp = new VNode(src->name, src->isDir, parent);
            cp->content = src->content;
            for (auto* ch : src->children)
                cp->children.append(clone(ch, cp));
            return cp;
            };
        for (VNode* node : clipboardNodes) {
            parentNode->children.append(clone(node, parentNode));
        }
    }
    else {
        // 剪切模式，直接挪动节点
        for (int i = 0; i < clipboardNodes.size(); ++i) {
            VNode* node = clipboardNodes[i];
            if (!node->parent) continue;
            // 从原父节点移除
            node->parent->children.removeOne(node);
            // 设新父节点
            node->parent = parentNode;
            parentNode->children.append(node);
        }
        clipboardNodes.clear();
        isCopy = true; // 恢复默认
    }
    vfsModel->endModelChange();
    folderModel->layoutChanged();
    updateListView(currentIndex);
}
void MainWindow::onRefresh() {
    vfsModel->refresh(currentIndex);
    folderModel->refresh(currentIndex);
    updateListView(currentIndex);
}

void MainWindow::onBack() {
    if (backStack.isEmpty()) return;
    forwardStack.push(currentIndex);
    QModelIndex idx = backStack.pop();
    currentIndex = idx;
    updateListView(currentIndex);
    updatePathEdit(currentIndex);
}

void MainWindow::onForward() {
    if (forwardStack.isEmpty()) return;
    backStack.push(currentIndex);
    QModelIndex idx = forwardStack.pop();
    currentIndex = idx;
    updateListView(currentIndex);
    updatePathEdit(currentIndex);
}
void MainWindow::onUp() {
    if (!currentIndex.isValid()) return; // 已在根目录
    VNode* node = vfsModel->nodeFromIndex(currentIndex);
    if (!node || node == vfsRoot || !node->parent) {
        // 回到根目录
        backStack.push(currentIndex);
        forwardStack.clear();
        currentIndex = QModelIndex();
        updateListView(currentIndex);
        updatePathEdit(currentIndex);
        return;
    }
    QModelIndex parentIdx = vfsModel->indexFromNode(node->parent);
    backStack.push(currentIndex);
    forwardStack.clear();
    currentIndex = parentIdx;
    updateListView(currentIndex);
    updatePathEdit(currentIndex);
}
void MainWindow::onSort() {
    QStringList items = { "名称", "修改时间", "类型" };
    bool ok;
    QString selected = QInputDialog::getItem(this, "排序", "选择排序方式：", items, currentSortField, false, &ok);
    if (!ok) return;
    int idx = items.indexOf(selected);
    if (idx == -1) return;
    currentSortField = static_cast<SortField>(idx);
    QStringList orderItems = { "升序", "降序" };
    QString orderSel = QInputDialog::getItem(this, "排序", "选择排序顺序：", orderItems, currentSortOrder == Qt::AscendingOrder ? 0 : 1, false, &ok);
    if (!ok) return;
    currentSortOrder = (orderSel == "升序") ? Qt::AscendingOrder : Qt::DescendingOrder;
    sortCurrentDir();
}

void MainWindow::sortCurrentDir() {
    VNode* dir = vfsModel->nodeFromIndex(currentIndex);
    if (!dir || !dir->isDir) return;
    auto cmp = [this](VNode* a, VNode* b) {
        int result = 0;
        switch (currentSortField) {
        case SortByName: result = QString::localeAwareCompare(a->name, b->name); break;
        case SortByTime: result = a->lastModified < b->lastModified ? -1 :
            (a->lastModified > b->lastModified ? 1 : 0); break;
        case SortByType: result = QString::localeAwareCompare(a->type, b->type); break;
        }
        if (currentSortOrder == Qt::DescendingOrder) result = -result;
        return result < 0;
        };
    std::sort(dir->children.begin(), dir->children.end(), cmp);
    vfsModel->refresh(currentIndex);
    updateListView(currentIndex);
}

// 递归匹配函数
void MainWindow::recursiveMatch(const QModelIndex& parent, const QString& searchText, QModelIndexList& matches) {
    int rowCount = vfsModel->rowCount(parent);
    for (int row = 0; row < rowCount; ++row) {
        QModelIndex idx = vfsModel->index(row, 0, parent);
        QString name = vfsModel->data(idx, Qt::DisplayRole).toString();
        if (name.contains(searchText, Qt::CaseInsensitive)) {
            matches << idx;
        }
        // 如果是目录，递归
        if (vfsModel->hasChildren(idx)) {
            recursiveMatch(idx, searchText, matches);
        }
    }
}
//搜索
void MainWindow::onSearch() {
    QString searchText = searchEdit->text().trimmed();
    if (searchText.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入搜索内容");
        return;
    }
    QModelIndexList matches;
    recursiveMatch(QModelIndex(), searchText, matches);
    if (matches.isEmpty())
        QMessageBox::information(this, "提示", "未找到匹配项");
    else {
        // 匹配第一个匹配项，跳转到该文件或文件夹
        QModelIndex matchIdx = matches.first();
        // 展示目录中的内容并高亮选中
        listView->setRootIndex(matchIdx.parent());
        listView->setCurrentIndex(matchIdx);
        treeView->setCurrentIndex(matchIdx); // treeView同步选中
        updatePathEdit(matchIdx);
    }
}

void MainWindow::showContextMenu(const QPoint& pos) {
    QModelIndex idx = listView->indexAt(pos);
    QMenu menu;
    menu.addAction("新建文件夹", this, [this]() { onNew(true); });
    menu.addAction("新建文件", this, [this]() { onNew(false); });
    if (idx.isValid()) {
        menu.addAction("打开", this, &MainWindow::openCurrentItem);
        menu.addAction("剪切", this, [this]() { onCutOrCopy(false); });
        menu.addAction("复制", this, [this]() { onCutOrCopy(true); });
        menu.addAction("重命名", this, &MainWindow::onRename);
        menu.addAction("删除", this, &MainWindow::onDelete); 
    }
    menu.addAction("粘贴", this, &MainWindow::onPaste);
    menu.exec(listView->viewport()->mapToGlobal(pos));
}

void MainWindow::updateListView(const QModelIndex& idx) {
    listView->setRootIndex(idx);
    treeView->setCurrentIndex(idx);
}

void MainWindow::updatePathEdit(const QModelIndex& idx) {
    if (!idx.isValid()) {
        pathEdit->setText("C:\\"); // 根目录
        return;
    }
    VNode* node = vfsModel->nodeFromIndex(idx);
    QStringList parts;
    while (node && node != vfsRoot) {
        parts.push_front(node->name);
        node = node->parent;
    }
    pathEdit->setText("C:\\" + parts.join("\\"));
}

QModelIndex MainWindow::getParentIndex(const QModelIndex& idx) const {
    if (!idx.isValid()) return QModelIndex();
    VNode* node = vfsModel->nodeFromIndex(idx);
    if (!node || node == vfsRoot || !node->parent) return QModelIndex();
    return vfsModel->indexFromNode(node->parent);
}

void MainWindow::saveVFS() {
    QFile f(SAVE_FILENAME);
    if (f.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(vfsRoot->toJson());
        f.write(doc.toJson(QJsonDocument::Indented));
        f.close();
    }
}

void MainWindow::loadVFS() {
    QFile f(SAVE_FILENAME);
    if (f.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        f.close();
        delete vfsRoot;
        vfsRoot = VNode::fromJson(doc.object());
    }
    else 
        vfsRoot = new VNode("root", true, nullptr);
}