#pragma once
#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QCloseEvent>
#include <QLabel>

class FileEditDialog : public QDialog {
    Q_OBJECT
public:
    FileEditDialog(const QString& fileName, QString& content, QWidget* parent = nullptr)
        : QDialog(parent), contentRef(content) {
        setWindowTitle(fileName);
        textEdit = new QTextEdit(this);
        textEdit->setPlainText(content);
        textEdit->moveCursor(QTextCursor::End);
        originalContent = content;

        saveBtn = new QPushButton("保存", this);
        closeBtn = new QPushButton("关闭", this);
        connect(saveBtn, &QPushButton::clicked, this, &FileEditDialog::onSave);
        connect(closeBtn, &QPushButton::clicked, this, &QDialog::close);

        QHBoxLayout* btnLayout = new QHBoxLayout;
        btnLayout->addStretch();
        btnLayout->addWidget(saveBtn);
        btnLayout->addWidget(closeBtn);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(textEdit);
        layout->addLayout(btnLayout);
        setLayout(layout);
        resize(600, 400);
    }
protected:
    void closeEvent(QCloseEvent* event) override {
        QString curContent = textEdit->toPlainText();
        if (curContent != originalContent) {
            int ret = QMessageBox::question(this, "提示", "内容已修改，是否保存？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if (ret == QMessageBox::Yes) {
                onSave();
            }
        }
        event->accept();
    }
private slots:
    void onSave() {
        QString newContent = textEdit->toPlainText();
        if (newContent != contentRef) {
            contentRef = newContent;
            originalContent = newContent;
        }
        QMessageBox::information(this, "提示", "保存成功！");
        accept();
    }
private:
    QTextEdit* textEdit;
    QPushButton* saveBtn;
    QPushButton* closeBtn;
    QString& contentRef;
    QString originalContent;
};