#ifndef XE_PLOT_WIDGET_H
#define XE_PLOT_WIDGET_H

#include <QWidget>

class XePlotWidget : public QWidget {
  Q_OBJECT
 public:
  explicit XePlotWidget(QWidget* parent = nullptr);

 protected:
  void paintEvent(QPaintEvent* event) override;
};

#endif  // XE_PLOT_WIDGET_H
