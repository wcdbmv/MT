#pragma once

#include <QMainWindow>
#include <QObject>
#include <QtCore>

class QWidget;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

 public:
  Ui::MainWindow* ui;
};
