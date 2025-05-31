#include "vfilesystemmodel.h"
#include <QIcon>

VFileSystemModel::VFileSystemModel(VNode* root, QObject* parent)
    : QAbstractItemModel(parent), m_root(root) {}

VFileSystemModel::~VFileSystemModel() {}

//获取索引
QModelIndex VFileSystemModel::index(int row, int column, const QModelIndex& parent) const {
    VNode* parentNode = nodeFromIndex(parent);
    if (!parentNode || row < 0 || row >= parentNode->children.size()) return QModelIndex();
    return createIndex(row, column, parentNode->children[row]);
}

QModelIndex VFileSystemModel::parent(const QModelIndex& child) const {
    if (!child.isValid()) return QModelIndex();
    VNode* node = nodeFromIndex(child);
    if (!node || node == m_root || !node->parent) return QModelIndex();
    VNode* parentNode = node->parent;
    int row = parentNode->parent ? parentNode->parent->children.indexOf(parentNode) : 0;
    return createIndex(row, 0, parentNode);
}

int VFileSystemModel::rowCount(const QModelIndex& parent) const {
    VNode* node = nodeFromIndex(parent);
    if (!node) return 0;
    return node->children.size();
}

int VFileSystemModel::columnCount(const QModelIndex&) const {
    return 3; // 名称、修改时间、类型
}

QVariant VFileSystemModel::data(const QModelIndex& index, int role) const {
    VNode* node = nodeFromIndex(index);
    if (!node) return QVariant();
    if (role == Qt::DisplayRole) {
        if (index.column() == 0) return node->name;
        if (index.column() == 1) return node->lastModified.toString("yyyy-MM-dd HH:mm:ss");
        if (index.column() == 2) return node->type;
    }
    if (role == Qt::DecorationRole && index.column() == 0) {// 图标显示
        if (node->isDir)
            return QIcon("icons/folder1.png");
        else {
            QString ext = node->name.section('.', -1);
            if (ext == "txt")
                return QIcon("icons/txt.png");
            else if (ext == "doc" || ext == "docx")
                return QIcon("icons/word.png");
            else if (ext == "pdf")
                return QIcon("icons/pdf.png");
            else if (ext == "zip")
                return QIcon("icons/zip.png");
            else if (ext == "ai")
                return QIcon("icons/ai.png");
            else if (ext == "mp3")
                return QIcon("icons/mp3.png");
            else if (ext == "xls")
                return QIcon("icons/excel.png");
            else
                return QIcon("icons/unknow.png");
        }
    }
    return QVariant();
}

QVariant VFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) return "      名称";  //为了对齐，前面加空格
        if (section == 1) return "修改日期";
        if (section == 2) return "类型";
    }
    return QVariant();
}

Qt::ItemFlags VFileSystemModel::flags(const QModelIndex& index) const {
    VNode* node = nodeFromIndex(index);
    if (!node) return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

VNode* VFileSystemModel::nodeFromIndex(const QModelIndex& index) const {
    if (index.isValid())
        return static_cast<VNode*>(index.internalPointer());
    return m_root;
}

QModelIndex VFileSystemModel::indexFromNode(VNode* node) const {
    if (!node || node == m_root) return QModelIndex();
    VNode* parentNode = node->parent;
    int row = parentNode->children.indexOf(node);
    return createIndex(row, 0, node);
}

void VFileSystemModel::refresh(const QModelIndex& idx) {
    emit dataChanged(idx, idx);
    emit layoutChanged();
}

void VFileSystemModel::beginModelChange() {
    beginResetModel();
}
void VFileSystemModel::endModelChange() {
    endResetModel();
}

int VFolderOnlyModel::rowCount(const QModelIndex& parent) const{
    VNode* node = nodeFromIndex(parent);
    if (!node) return 0;
    int count = 0;
    for (const VNode* ch : node->children)
        if (ch->isDir) ++count;
    return count;
}

QModelIndex VFolderOnlyModel::index(int row, int column, const QModelIndex& parent) const{
    VNode* parentNode = nodeFromIndex(parent);
    if (!parentNode) return QModelIndex();
    int folderIdx = -1;
    for (int i = 0; i < parentNode->children.size(); ++i) {
        if (parentNode->children[i]->isDir) ++folderIdx;
        if (folderIdx == row)
            return createIndex(row, column, parentNode->children[i]);
    }
    return QModelIndex();
}