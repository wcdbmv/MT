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

class XeXeSiO2PlotWidget : public QChartView {
  Q_OBJECT
 public:
  explicit XeXeSiO2PlotWidget(QWidget* parent = nullptr);

  void setAxisY(const QString& title);
  void setData(const std::vector<Float>& plasma,
               const std::vector<Float>& quartz);

 private:
  QChart* chart_;
  QValueAxis* axis_x_;
  QValueAxis* axis_y_;
  QLineSeries* series_;
  QLineSeries* series2_;
};
