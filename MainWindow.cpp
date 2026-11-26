#include "MainWindow.h"
#include <QBoxLayout>
#include <QCryptographicHash>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QImageReader>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollArea>
#include <QSpinBox>
#include <QStandardPaths>
#include <QTextStream>
#include <QUrl>

namespace {
const QStringList kExtensions = {"*.jpg","*.jpeg","*.png","*.webp","*.tif","*.tiff","*.bmp"};
QString csvEscape(QString value) {
    value.replace('"', "\"\"");
    return '"' + value + '"';
}
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("PetriRenamer C++ 0.1");
    resize(1180, 760);
    setMinimumSize(900, 650);
    buildUi();
}

void MainWindow::buildUi() {
    auto *central = new QWidget(this);
    auto *root = new QVBoxLayout(central);
    auto *toolbar = new QHBoxLayout;
    auto *choose = new QPushButton("1. Chọn thư mục ảnh");
    groupSize_ = new QSpinBox;
    groupSize_->setRange(1, 6); groupSize_->setValue(2);
    auto *output = new QPushButton("Mở thư mục kết quả");
    toolbar->addWidget(choose); toolbar->addWidget(new QLabel("Số ảnh mỗi đĩa:"));
    toolbar->addWidget(groupSize_); toolbar->addStretch(); toolbar->addWidget(output);
    root->addLayout(toolbar);

    auto *body = new QHBoxLayout;
    auto *left = new QVBoxLayout;
    imageLabel_ = new QLabel("Chọn thư mục ảnh để bắt đầu");
    imageLabel_->setAlignment(Qt::AlignCenter); imageLabel_->setMinimumSize(600, 480);
    imageLabel_->setStyleSheet("background:#202020;color:white;");
    auto *scroll = new QScrollArea; scroll->setWidget(imageLabel_); scroll->setWidgetResizable(true);
    left->addWidget(scroll, 1);
    auto *nav = new QHBoxLayout;
    auto *prev = new QPushButton("◀ Nhóm trước"); auto *next = new QPushButton("Nhóm tiếp ▶");
    counterLabel_ = new QLabel("0 / 0"); counterLabel_->setAlignment(Qt::AlignCenter);
    nav->addWidget(prev); nav->addWidget(counterLabel_, 1); nav->addWidget(next); left->addLayout(nav);

    auto *right = new QVBoxLayout;
    auto *title = new QLabel("Tên chuẩn"); QFont f=title->font(); f.setPointSize(13); f.setBold(true); title->setFont(f);
    codeEdit_ = new QLineEdit; QFont mono("Consolas", 15); codeEdit_->setFont(mono);
    acceptButton_ = new QPushButton("Chấp nhận, sao chép và sang nhóm tiếp");
    auto *review = new QPushButton("Đánh dấu cần xem lại");
    right->addWidget(title); right->addWidget(codeEdit_);
    right->addWidget(new QLabel("Ví dụ: AA-W05LB-8 hoặc AA-W10LB-48-LGI"));
    right->addSpacing(10); right->addWidget(acceptButton_); right->addWidget(review); right->addStretch();
    right->addWidget(new QLabel("Bản C++ prototype\n• Không sửa ảnh gốc\n• Xác minh SHA-256\n• Xuất manifest CSV\n• Chưa tích hợp OCR"));
    body->addLayout(left, 4); body->addLayout(right, 2); root->addLayout(body, 1);
    statusLabel_ = new QLabel("Sẵn sàng."); statusLabel_->setFrameStyle(QFrame::Panel|QFrame::Sunken); root->addWidget(statusLabel_);
    setCentralWidget(central);

    connect(choose,&QPushButton::clicked,this,&MainWindow::chooseFolder);
    connect(prev,&QPushButton::clicked,this,&MainWindow::previousGroup);
    connect(next,&QPushButton::clicked,this,&MainWindow::nextGroup);
    connect(acceptButton_,&QPushButton::clicked,this,&MainWindow::acceptGroup);
    connect(review,&QPushButton::clicked,this,&MainWindow::markReview);
    connect(output,&QPushButton::clicked,this,&MainWindow::openOutput);
    connect(groupSize_,qOverload<int>(&QSpinBox::valueChanged),this,[this]{ index_=0; showGroup(); });
}

void MainWindow::chooseFolder() {
    const QString selected=QFileDialog::getExistingDirectory(this,"Chọn thư mục chứa ảnh gốc",folder_);
    if(selected.isEmpty()) return; folder_=selected; loadImages();
}
void MainWindow::loadImages() {
    QDir dir(folder_); files_=dir.entryList(kExtensions,QDir::Files,QDir::Name|QDir::IgnoreCase);
    index_=0; codeEdit_->clear(); showGroup(); statusLabel_->setText(QString("Đã nạp %1 ảnh.").arg(files_.size()));
}
QStringList MainWindow::currentGroup() const { return files_.mid(index_,groupSize_->value()); }
void MainWindow::showGroup() {
    if(files_.isEmpty()){ imageLabel_->setText("Không có ảnh phù hợp."); counterLabel_->setText("0 / 0"); return; }
    const auto group=currentGroup(); QList<QImage> images; int totalW=0,maxH=0;
    for(const auto &name:group){ QImageReader reader(QDir(folder_).filePath(name)); reader.setAutoTransform(true); QImage img=reader.read();
        if(img.isNull()) continue; img=img.scaled(520,540,Qt::KeepAspectRatio,Qt::SmoothTransformation); totalW+=img.width()+12; maxH=qMax(maxH,img.height()+35); images<<img; }
    if(images.isEmpty()){ imageLabel_->setText("Không đọc được ảnh trong nhóm."); return; }
    QImage combined(qMax(1,totalW),qMax(1,maxH),QImage::Format_RGB32); combined.fill(QColor(32,32,32)); QPainter p(&combined); int x=0;
    for(int i=0;i<images.size();++i){ p.drawImage(x,0,images[i]); p.setPen(Qt::white); p.drawText(x+4,images[i].height()+22,group.value(i)); x+=images[i].width()+12; }
    imageLabel_->setPixmap(QPixmap::fromImage(combined)); imageLabel_->resize(combined.size());
    counterLabel_->setText(QString("%1-%2 / %3").arg(index_+1).arg(qMin(index_+group.size(),files_.size())).arg(files_.size()));
}
void MainWindow::previousGroup(){ index_=qMax(0,index_-groupSize_->value()); showGroup(); }
void MainWindow::nextGroup(){ const int n=index_+groupSize_->value(); if(n>=files_.size()){ statusLabel_->setText("Đã đến nhóm cuối."); return;} index_=n; codeEdit_->clear(); showGroup(); }
QString MainWindow::normalizedCode() const { return codeEdit_->text().trimmed().toUpper().remove(' '); }
bool MainWindow::validCode(const QString &c) const { static const QRegularExpression rx("^(AA|AB|AC)-(W|I|P)(05|10|15|20)(LB|GY|LG)-\\d{1,3}(?:-[A-Z0-9]+)?$"); return rx.match(c).hasMatch(); }
QString MainWindow::sha256(const QString &path) const { QFile f(path); if(!f.open(QIODevice::ReadOnly)) return {}; QCryptographicHash h(QCryptographicHash::Sha256); if(!h.addData(&f)) return {}; return h.result().toHex(); }
bool MainWindow::safeCopy(const QString &source,const QString &target,QString *error) const { if(QFile::exists(target)){*error="File đích đã tồn tại: "+target;return false;} if(!QFile::copy(source,target)){*error="Không thể sao chép: "+source;return false;} if(sha256(source)!=sha256(target)){QFile::remove(target);*error="Checksum không khớp: "+source;return false;} return true; }
QString MainWindow::nextTarget(const QString &code,const QString &suffix,int offset,int count) const { QDir out(QDir(folder_).filePath("PetriRenamer_output")); if(count==1){const QString p=out.filePath(code+suffix.toLower());if(!QFile::exists(p))return p;} int n=1; while(QFile::exists(out.filePath(QString("%1（%2）%3").arg(code).arg(n).arg(suffix.toLower()))))++n; return out.filePath(QString("%1（%2）%3").arg(code).arg(n+offset).arg(suffix.toLower())); }
void MainWindow::appendManifest(const QString &src,const QString &dst,const QString &code,const QString &status,const QString &hash){ QDir out(QDir(folder_).filePath("PetriRenamer_output")); out.mkpath("."); QFile f(out.filePath("rename_manifest.csv")); const bool fresh=!f.exists()||f.size()==0; if(!f.open(QIODevice::Append|QIODevice::Text)) return; QTextStream s(&f); s.setEncoding(QStringConverter::Utf8); if(fresh)s<<"original_name,new_name,code,status,sha256\n"; s<<csvEscape(src)<<','<<csvEscape(dst)<<','<<csvEscape(code)<<','<<csvEscape(status)<<','<<csvEscape(hash)<<'\n'; }
void MainWindow::acceptGroup(){ if(folder_.isEmpty())return; const QString code=normalizedCode(); if(!validCode(code)){QMessageBox::critical(this,"Tên chưa hợp lệ","Ví dụ hợp lệ: AA-W05LB-8");return;} QDir out(QDir(folder_).filePath("PetriRenamer_output"));out.mkpath("."); const auto group=currentGroup(); for(int i=0;i<group.size();++i){const QString src=QDir(folder_).filePath(group[i]);const QString target=nextTarget(code,QFileInfo(src).suffix().prepend('.'),i,group.size());QString error;if(!safeCopy(src,target,&error)){QMessageBox::critical(this,"Lỗi",error);return;}appendManifest(group[i],QFileInfo(target).fileName(),code,"accepted",sha256(src));} statusLabel_->setText(QString("Đã sao chép an toàn %1 ảnh.").arg(group.size()));nextGroup();}
void MainWindow::markReview(){if(folder_.isEmpty())return;QDir out(QDir(folder_).filePath("PetriRenamer_output/needs_review"));out.mkpath(".");for(const auto &name:currentGroup()){const QString src=QDir(folder_).filePath(name),dst=out.filePath(name);if(!QFile::exists(dst))QFile::copy(src,dst);appendManifest(name,name,"","needs_review",sha256(src));}nextGroup();}
void MainWindow::openOutput(){if(folder_.isEmpty())return;QDir out(QDir(folder_).filePath("PetriRenamer_output"));out.mkpath(".");QDesktopServices::openUrl(QUrl::fromLocalFile(out.absolutePath()));}
