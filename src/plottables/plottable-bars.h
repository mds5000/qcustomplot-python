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

#ifndef QCP_PLOTTABLE_BARS_H
#define QCP_PLOTTABLE_BARS_H

#include "../global.h"
#include "../range.h"
#include "../plottable.h"

class QCPPainter;
class QCPAxis;

class QCP_LIB_DECL QCPBarData
{
public:
  QCPBarData();
  QCPBarData(double key, double value);
  double key, value;
};
Q_DECLARE_TYPEINFO(QCPBarData, Q_MOVABLE_TYPE);

/*! \typedef QCPBarDataMap
  Container for storing QCPBarData items in a sorted fashion. The key of the map
  is the key member of the QCPBarData instance.
  
  This is the container in which QCPBars holds its data.
  \see QCPBarData, QCPBars::setData
*/
typedef QMap<double, QCPBarData> QCPBarDataMap;
typedef QMapIterator<double, QCPBarData> QCPBarDataMapIterator;
typedef QMutableMapIterator<double, QCPBarData> QCPBarDataMutableMapIterator;


class QCP_LIB_DECL QCPBars : public QCPAbstractPlottable
{
  Q_OBJECT
public:
  explicit QCPBars(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPBars();
  
  // getters:
  double width() const { return mWidth; }
  QCPBars *barBelow() const { return mBarBelow; }
  QCPBars *barAbove() const { return mBarAbove; }
  const QCPBarDataMap *data() const { return mData; }
  
  // setters:
  void setWidth(double width);
  void setData(QCPBarDataMap *data, bool copy=false);
  void setData(const QVector<double> &key, const QVector<double> &value);
  
  // non-property methods:
  void moveBelow(QCPBars *bars);
  void moveAbove(QCPBars *bars);
  void addData(const QCPBarDataMap &dataMap);
  void addData(const QCPBarData &data);
  void addData(double key, double value);
  void addData(const QVector<double> &keys, const QVector<double> &values);
  void removeDataBefore(double key);
  void removeDataAfter(double key);
  void removeData(double fromKey, double toKey);
  void removeData(double key);
  virtual void clearData();
  virtual double selectTest(const QPointF &pos) const;
  
protected:
  QCPBarDataMap *mData;
  double mWidth;
  QCPBars *mBarBelow, *mBarAbove;
  
  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRect &rect) const;
  
  QPolygonF getBarPolygon(double key, double value) const;
  double getBaseValue(double key, bool positive) const;
  static void connectBars(QCPBars* lower, QCPBars* upper);
  virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  
  friend class QCustomPlot;
  friend class QCPLegend;
};

#endif // QCP_PLOTTABLE_BARS_H
