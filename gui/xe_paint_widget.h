#ifndef XE_PAINT_WIDGET_H
#define XE_PAINT_WIDGET_H

#include <QWidget>

class XePaintWidget : public QWidget {
  Q_OBJECT
 public:
  explicit XePaintWidget(QWidget* parent = nullptr);

 protected:
  void paintEvent(QPaintEvent* event) override;
};

#endif  // XE_PAINT_WIDGET_H
