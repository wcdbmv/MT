#pragma once

#include <optional>

#include <QMainWindow>
#include <QObject>
#include <QtCore>

#include "modeling/cylinder_plasma.h"

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

 Q_SIGNALS:
  void XeSolved();

 public:
  Ui::MainWindow* ui;
  std::optional<CylinderPlasma::Result> xe_res;
};
