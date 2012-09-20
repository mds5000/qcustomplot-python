/***************************************************************************
**                                                                        **
**  QCustomPlot, a simple to use, modern plotting widget for Qt           **
**  Copyright (C) 2011, 2012 Emanuel Eichhammer                           **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.WorksLikeClockwork.com/                   **
**             Date: 09.06.12                                             **
****************************************************************************/

#ifndef QCP_PAINTER_H
#define QCP_PAINTER_H

#include "global.h"

/*
class QCP_LIB_DECL QCPScatter
{
public:
  QCPScatter(QCP::ScatterStyle style)
  ~QCPScatter();
  
  void scatter
};
*/

class QCP_LIB_DECL QCPPainter : public QPainter
{
public:
  QCPPainter();
  QCPPainter(QPaintDevice *device);
  ~QCPPainter();
  
  // getters:
  QPixmap scatterPixmap() const { return mScatterPixmap; }
  bool antialiasing() const { return testRenderHint(QPainter::Antialiasing); }
  bool pdfExportMode() const { return mPdfExportMode; }
  bool scaledExportMode() const { return mScaledExportMode; }
  
  // setters:
  void setScatterPixmap(const QPixmap pm);
  void setAntialiasing(bool enabled);
  void setPdfExportMode(bool enabled);
  void setScaledExportMode(bool enabled);
 
  // methods hiding non-virtual base class functions (QPainter bug workarounds):
  void setPen(const QPen &pen);
  void setPen(const QColor &color);
  void setPen(Qt::PenStyle penStyle);
  void drawLine(const QLineF &line);
  void drawLine(const QPointF &p1, const QPointF &p2) {drawLine(QLineF(p1, p2));}
  void save();
  void restore();

  // helpers:
  void fixScaledPen();
  void drawScatter(double x, double y, double size, QCP::ScatterStyle style);
  
protected:
  QPixmap mScatterPixmap;
  bool mScaledExportMode;
  bool mPdfExportMode;
  bool mIsAntialiasing;
  QStack<bool> mAntialiasingStack;
};

#endif // QCP_PAINTER_H
