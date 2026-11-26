#pragma once
#include <QMainWindow>
#include <QStringList>

class QLabel;
class QLineEdit;
class QSpinBox;
class QPushButton;

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
private slots:
    void chooseFolder();
    void previousGroup();
    void nextGroup();
    void acceptGroup();
    void markReview();
    void openOutput();
private:
    void buildUi();
    void loadImages();
    void showGroup();
    QStringList currentGroup() const;
    QString normalizedCode() const;
    bool validCode(const QString &code) const;
    QString sha256(const QString &path) const;
    bool safeCopy(const QString &source, const QString &target, QString *error) const;
    QString nextTarget(const QString &code, const QString &suffix, int offset, int count) const;
    void appendManifest(const QString &sourceName, const QString &targetName,
                        const QString &code, const QString &status, const QString &hash);

    QString folder_;
    QStringList files_;
    int index_ = 0;
    QLabel *imageLabel_ = nullptr;
    QLabel *counterLabel_ = nullptr;
    QLabel *statusLabel_ = nullptr;
    QLineEdit *codeEdit_ = nullptr;
    QSpinBox *groupSize_ = nullptr;
    QPushButton *acceptButton_ = nullptr;
};
