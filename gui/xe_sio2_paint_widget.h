#pragma once

#include <QObject>
#include <QWidget>

class QPaintEvent;

class XeSiO2PaintWidget : public QWidget {
  Q_OBJECT
 public:
  explicit XeSiO2PaintWidget(QWidget* parent = nullptr);

 protected:
  void paintEvent(QPaintEvent* event) override;
};
