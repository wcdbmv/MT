#pragma once

#include <QChartView>
#include <QObject>
#include <QString>

#include <vector>

#include "base/config/float.h"

class QChart;
class QLineSeries;
class QValueAxis;
class QWidget;

class XeSiO2PlotWidget : public QChartView {
  Q_OBJECT
 public:
  explicit XeSiO2PlotWidget(QWidget* parent = nullptr);

  void setAxisY(const QString& title);
  void setData(const std::vector<Float>& plasma,
               const std::vector<Float>& quartz,
               Float r,
               Float delta);

 private:
  QChart* chart_;
  QValueAxis* axis_x_;
  QValueAxis* axis_y_;
  QLineSeries* series_;
  QLineSeries* vr_;
};
