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

#ifndef QCP_ITEM_TRACER_H
#define QCP_ITEM_TRACER_H

#include "../global.h"
#include "../item.h"

class QCPPainter;
class QCustomPlot;
class QCPGraph;

class QCP_LIB_DECL QCPItemTracer : public QCPAbstractItem
{
  Q_OBJECT
public:
  /*!
    The different visual appearances a tracer item can have. Some styles size may be controlled with \ref setSize.
    
    \see setStyle
  */
  enum TracerStyle { tsNone        ///< The tracer is not visible
                     ,tsPlus       ///< A plus shaped crosshair with limited size
                     ,tsCrosshair  ///< A plus shaped crosshair which spans the complete axis rect
                     ,tsCircle     ///< A circle
                     ,tsSquare     ///< A square
                   };
  Q_ENUMS(TracerStyle)

  QCPItemTracer(QCustomPlot *parentPlot);
  virtual ~QCPItemTracer();

  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QBrush brush() const { return mBrush; }
  QBrush selectedBrush() const { return mSelectedBrush; }
  double size() const { return mSize; }
  TracerStyle style() const { return mStyle; }
  QCPGraph *graph() const { return mGraph; }
  double graphKey() const { return mGraphKey; }
  bool interpolating() const { return mInterpolating; }

  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  void setSize(double size);
  void setStyle(TracerStyle style);
  void setGraph(QCPGraph *graph);
  void setGraphKey(double key);
  void setInterpolating(bool enabled);

  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  void updatePosition();

  QCPItemPosition * const position;

protected:
  QPen mPen, mSelectedPen;
  QBrush mBrush, mSelectedBrush;
  double mSize;
  TracerStyle mStyle;
  QCPGraph *mGraph;
  double mGraphKey;
  bool mInterpolating;

  virtual void draw(QCPPainter *painter);

  // helper functions:
  QPen mainPen() const;
  QBrush mainBrush() const;
};

#endif // QCP_ITEM_TRACER_H
