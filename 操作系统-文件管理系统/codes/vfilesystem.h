#pragma once
#include <QVector>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
// VNode 类表示文件系统中的一个节点，可以是文件或文件夹
struct VNode {
    QString name;   // 节点名称
    bool isDir;     // 是否为文件夹
    QString content; // 文件内容，仅在 isDir 为 false 时有效(即只有文件夹有)
    VNode* parent;   // 父节点指针，根节点的 parent 为 nullptr
    QVector<VNode*> children;   // 子节点列表，存储所有子节点指针
    QDateTime lastModified; // 最后修改时间
    QString type;           // 类型（后缀名或"文件夹"）,从name中获取

    VNode(const QString& name, bool isDir, VNode* parent = nullptr)
        : name(name), isDir(isDir), parent(parent) {
        lastModified = QDateTime::currentDateTime();  // 初始化为当前时间
        type = isDir ? "文件夹" : (name.contains('.') ? name.section('.', -1).toUpper()+" 文件" : "文件");
    }
    ~VNode() { qDeleteAll(children); }
    // 序列化
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = name;
        obj["isDir"] = isDir;
        if (!isDir) obj["content"] = content;
        QJsonArray arr;
        for (const auto* child : children) arr.append(child->toJson());// 递归将子节点序列化为 JSON 数组
        obj["children"] = arr;
        obj["lastModified"] = lastModified.toString(Qt::ISODate); 
        obj["type"] = type; 
        return obj;
    }

    // 反序列化
    static VNode* fromJson(const QJsonObject& obj, VNode* parent = nullptr) {
        VNode* node = new VNode(obj["name"].toString(), obj["isDir"].toBool(), parent);
        if (!node->isDir)
            node->content = obj["content"].toString();
        QJsonArray arr = obj["children"].toArray();
        for (auto childVal : arr) {
            node->children.append(fromJson(childVal.toObject(), node));
        }
        node->lastModified = QDateTime::fromString(obj["lastModified"].toString(), Qt::ISODate);
        node->type = obj["type"].toString();
        return node;
    }
};