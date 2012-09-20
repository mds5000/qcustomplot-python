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

#ifndef QCP_PLOTTABLE_STATISTICALBOX_H
#define QCP_PLOTTABLE_STATISTICALBOX_H

#include "../global.h"
#include "../range.h"
#include "../plottable.h"

class QCPPainter;
class QCPAxis;

class QCP_LIB_DECL QCPStatisticalBox : public QCPAbstractPlottable
{
  Q_OBJECT
public:
  explicit QCPStatisticalBox(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPStatisticalBox();
  
  // getters:
  double key() const { return mKey; }
  double minimum() const { return mMinimum; }
  double lowerQuartile() const { return mLowerQuartile; }
  double median() const { return mMedian; }
  double upperQuartile() const { return mUpperQuartile; }
  double maximum() const { return mMaximum; }
  QVector<double> outliers() const { return mOutliers; }
  double width() const { return mWidth; }
  double whiskerWidth() const { return mWhiskerWidth; }
  QPen whiskerPen() const { return mWhiskerPen; }
  QPen whiskerBarPen() const { return mWhiskerBarPen; }
  QPen medianPen() const { return mMedianPen; }
  double outlierSize() const { return mOutlierSize; }
  QPen outlierPen() const { return mOutlierPen; }
  QCP::ScatterStyle outlierStyle() const { return mOutlierStyle; }

  // setters:
  void setKey(double key);
  void setMinimum(double value);
  void setLowerQuartile(double value);
  void setMedian(double value);
  void setUpperQuartile(double value);
  void setMaximum(double value);
  void setOutliers(const QVector<double> &values);
  void setData(double key, double minimum, double lowerQuartile, double median, double upperQuartile, double maximum);
  void setWidth(double width);
  void setWhiskerWidth(double width);
  void setWhiskerPen(const QPen &pen);
  void setWhiskerBarPen(const QPen &pen);
  void setMedianPen(const QPen &pen);
  void setOutlierSize(double pixels);
  void setOutlierPen(const QPen &pen);
  void setOutlierStyle(QCP::ScatterStyle style);
  
  // non-property methods:
  virtual void clearData();
  virtual double selectTest(const QPointF &pos) const;
  
protected:
  QVector<double> mOutliers;
  double mKey, mMinimum, mLowerQuartile, mMedian, mUpperQuartile, mMaximum;
  double mWidth;
  double mWhiskerWidth;
  double mOutlierSize;
  QPen mWhiskerPen, mWhiskerBarPen, mOutlierPen, mMedianPen;
  QCP::ScatterStyle mOutlierStyle;
  
  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRect &rect) const;
  
  virtual void drawQuartileBox(QCPPainter *painter, QRectF *quartileBox=0) const;
  virtual void drawMedian(QCPPainter *painter) const;
  virtual void drawWhiskers(QCPPainter *painter) const;
  virtual void drawOutliers(QCPPainter *painter) const;
  virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  
  friend class QCustomPlot;
  friend class QCPLegend;
};

#endif // QCP_PLOTTABLE_STATISTICALBOX_H
