#pragma once

#include <optional>

#include <QMainWindow>
#include <QObject>
#include <QtCore>

#include "modeling/cylinder_plasma.h"
#include "modeling/cylinder_plasma_quartz.h"

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
  std::optional<CylinderPlasma::Params> xe_params;
  std::optional<CylinderPlasmaQuartz::Params> xe_sio2_params;
  std::optional<CylinderPlasma::Result> xe_res;
  std::optional<CylinderPlasmaQuartz::Result> xe_sio2_res;

 private:
  void InitXeTab();
  void InitXeSiO2Tab();
};
