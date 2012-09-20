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

#ifndef QCP_PLOTTABLE_CURVE_H
#define QCP_PLOTTABLE_CURVE_H

#include "../global.h"
#include "../range.h"
#include "../plottable.h"

class QCPPainter;
class QCPAxis;

class QCP_LIB_DECL QCPCurveData
{
public:
  QCPCurveData();
  QCPCurveData(double t, double key, double value);
  double t, key, value;
};
Q_DECLARE_TYPEINFO(QCPCurveData, Q_MOVABLE_TYPE);

/*! \typedef QCPCurveDataMap
  Container for storing QCPCurveData items in a sorted fashion. The key of the map
  is the t member of the QCPCurveData instance.
  
  This is the container in which QCPCurve holds its data.
  \see QCPCurveData, QCPCurve::setData
*/

typedef QMap<double, QCPCurveData> QCPCurveDataMap;
typedef QMapIterator<double, QCPCurveData> QCPCurveDataMapIterator;
typedef QMutableMapIterator<double, QCPCurveData> QCPCurveDataMutableMapIterator;


class QCP_LIB_DECL QCPCurve : public QCPAbstractPlottable
{
  Q_OBJECT
public:
  /*!
    Defines how the curve's line is represented visually in the plot. The line is drawn with the
    current pen of the curve (\ref setPen).
    \see setLineStyle
  */
  enum LineStyle { lsNone, ///< No line is drawn between data points (e.g. only scatters)
                   lsLine  ///< Data points are connected with a straight line
                 };
  explicit QCPCurve(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPCurve();
  
  // getters:
  const QCPCurveDataMap *data() const { return mData; }
  QCP::ScatterStyle scatterStyle() const { return mScatterStyle; }
  double scatterSize() const { return mScatterSize; }
  QPixmap scatterPixmap() const { return mScatterPixmap; }
  LineStyle lineStyle() const { return mLineStyle; }
  
  // setters:
  void setData(QCPCurveDataMap *data, bool copy=false);
  void setData(const QVector<double> &t, const QVector<double> &key, const QVector<double> &value);
  void setData(const QVector<double> &key, const QVector<double> &value);
  void setScatterStyle(QCP::ScatterStyle style);
  void setScatterSize(double size);
  void setScatterPixmap(const QPixmap &pixmap);
  void setLineStyle(LineStyle style);
  
  // non-property methods:
  void addData(const QCPCurveDataMap &dataMap);
  void addData(const QCPCurveData &data);
  void addData(double t, double key, double value);
  void addData(double key, double value);
  void addData(const QVector<double> &ts, const QVector<double> &keys, const QVector<double> &values);
  void removeDataBefore(double t);
  void removeDataAfter(double t);
  void removeData(double fromt, double tot);
  void removeData(double t);
  virtual void clearData();
  virtual double selectTest(const QPointF &pos) const;
  
protected:
  QCPCurveDataMap *mData;
  QCP::ScatterStyle mScatterStyle;
  double mScatterSize;
  QPixmap mScatterPixmap;
  LineStyle mLineStyle;
  
  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRect &rect) const;
  // drawing helpers:
  virtual void drawScatterPlot(QCPPainter *painter, const QVector<QPointF> *pointData) const;
  
  // helper functions:
  void getCurveData(QVector<QPointF> *lineData) const;
  double pointDistance(const QPointF &pixelPoint) const;

  QPointF outsideCoordsToPixels(double key, double value, int region) const;
  virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  
  friend class QCustomPlot;
  friend class QCPLegend;
};

#endif // QCP_PLOTTABLE_CURVE_H
