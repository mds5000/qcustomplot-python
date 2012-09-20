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

#ifndef QCP_PLOTTABLE_H
#define QCP_PLOTTABLE_H

#include "global.h"
#include "range.h"
#include "layer.h"

class QCPPainter;
class QCPAxis;

class QCP_LIB_DECL QCPAbstractPlottable : public QCPLayerable
{
  Q_OBJECT
public:
  QCPAbstractPlottable(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPAbstractPlottable() {}
  
  // getters:
  QString name() const { return mName; }
  bool antialiasedFill() const { return mAntialiasedFill; }
  bool antialiasedScatters() const { return mAntialiasedScatters; }
  bool antialiasedErrorBars() const { return mAntialiasedErrorBars; }
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QBrush brush() const { return mBrush; }
  QBrush selectedBrush() const { return mSelectedBrush; }
  QCPAxis *keyAxis() const { return mKeyAxis; }
  QCPAxis *valueAxis() const { return mValueAxis; }
  bool selectable() const { return mSelectable; }
  bool selected() const { return mSelected; }
  
  // setters:
  void setName(const QString &name);
  void setAntialiasedFill(bool enabled);
  void setAntialiasedScatters(bool enabled);
  void setAntialiasedErrorBars(bool enabled);
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  void setKeyAxis(QCPAxis *axis);
  void setValueAxis(QCPAxis *axis);
  void setSelectable(bool selectable);
  void setSelected(bool selected);

  // non-property methods:
  void rescaleAxes(bool onlyEnlarge=false) const;
  void rescaleKeyAxis(bool onlyEnlarge=false) const;
  void rescaleValueAxis(bool onlyEnlarge=false) const;
  virtual void clearData() = 0;
  virtual double selectTest(const QPointF &pos) const = 0;
  virtual bool addToLegend();
  virtual bool removeFromLegend() const;
  
signals:
  void selectionChanged(bool selected);
  
protected:
  /*!
    Represents negative and positive sign domain for passing to \ref getKeyRange and \ref getValueRange.
  */
  enum SignDomain { sdNegative  ///< The negative sign domain, i.e. numbers smaller than zero
                    ,sdBoth     ///< Both sign domains, including zero, i.e. all (rational) numbers
                    ,sdPositive ///< The positive sign domain, i.e. numbers greater than zero
                  };
  QString mName;
  bool mAntialiasedFill, mAntialiasedScatters, mAntialiasedErrorBars;
  QPen mPen, mSelectedPen;
  QBrush mBrush, mSelectedBrush;
  QCPAxis *mKeyAxis, *mValueAxis;
  bool mSelected, mSelectable;
  
  virtual QRect clipRect() const;
  virtual void draw(QCPPainter *painter) = 0;
  virtual void drawLegendIcon(QCPPainter *painter, const QRect &rect) const = 0;
  virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain=sdBoth) const = 0;
  virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain=sdBoth) const = 0;
  
  // painting and coordinate transformation helpers:
  void coordsToPixels(double key, double value, double &x, double &y) const;
  const QPointF coordsToPixels(double key, double value) const;
  void pixelsToCoords(double x, double y, double &key, double &value) const;
  void pixelsToCoords(const QPointF &pixelPos, double &key, double &value) const;
  QPen mainPen() const;
  QBrush mainBrush() const;
  void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  void applyFillAntialiasingHint(QCPPainter *painter) const;
  void applyScattersAntialiasingHint(QCPPainter *painter) const;
  void applyErrorBarsAntialiasingHint(QCPPainter *painter) const;
  
  // selection test helpers:
  double distSqrToLine(const QPointF &start, const QPointF &end, const QPointF &point) const;

private:
  Q_DISABLE_COPY(QCPAbstractPlottable)
  
  friend class QCustomPlot;
  friend class QCPPlottableLegendItem;
};

#endif // QCP_PLOTTABLE_H
