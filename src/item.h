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

#ifndef QCP_ITEM_H
#define QCP_ITEM_H

#include "global.h"
#include "layer.h"

class QCPPainter;
class QCustomPlot;
class QCPItemPosition;
class QCPAbstractItem;
class QCPAxis;

class QCP_LIB_DECL QCPItemAnchor
{
public:
  QCPItemAnchor(QCustomPlot *parentPlot, QCPAbstractItem *parentItem, const QString name, int anchorId=-1);
  virtual ~QCPItemAnchor();
  
  QString name() const { return mName; }
  virtual QPointF pixelPoint() const;
  
protected:
  QCustomPlot *mParentPlot;
  QCPAbstractItem *mParentItem;
  int mAnchorId;
  QString mName;
  // non-property members:
  QSet<QCPItemPosition*> mChildren;
  
  void addChild(QCPItemPosition* pos); // called from pos when this anchor is set as parent
  void removeChild(QCPItemPosition *pos); // called from pos when its parent anchor is reset or pos deleted
  
private:
  Q_DISABLE_COPY(QCPItemAnchor)
  
  friend class QCPItemPosition;
};



class QCP_LIB_DECL QCPItemPosition : public QCPItemAnchor
{
public:
  /*!
    Defines the ways an item position can be specified. Thus it defines what the numbers passed to
    \ref setCoords actually mean.
    
    \see setType
  */
  enum PositionType { ptAbsolute        ///< Static positioning in pixels, starting from the top left corner of the viewport/widget.
                      ,ptViewportRatio  ///< Static positioning given by a ratio of the current viewport (coordinates 0 to 1).
                      ,ptAxisRectRatio  ///< Static positioning given by a ratio of the current axis rect (coordinates 0 to 1).
                      ,ptPlotCoords     ///< Dynamic positioning at a plot coordinate defined by two axes (see \ref setAxes).
                    };
  
  QCPItemPosition(QCustomPlot *parentPlot, QCPAbstractItem *parentItem, const QString name);
  virtual ~QCPItemPosition();
  
  // getters:
  PositionType type() const { return mPositionType; }
  QCPItemAnchor *parentAnchor() const { return mParentAnchor; }
  double key() const { return mKey; }
  double value() const { return mValue; }
  QPointF coords() const { return QPointF(mKey, mValue); }
  QCPAxis *keyAxis() const { return mKeyAxis; }
  QCPAxis *valueAxis() const { return mValueAxis; }
  virtual QPointF pixelPoint() const;
  
  // setters:
  void setType(PositionType type);
  bool setParentAnchor(QCPItemAnchor *parentAnchor, bool keepPixelPosition=false);
  void setCoords(double key, double value);
  void setCoords(const QPointF &coords);
  void setAxes(QCPAxis* keyAxis, QCPAxis* valueAxis);
  void setPixelPoint(const QPointF &pixelPoint);
  
protected:
  PositionType mPositionType;
  QCPAxis *mKeyAxis, *mValueAxis;
  double mKey, mValue;
  QCPItemAnchor *mParentAnchor;
  
private:
  Q_DISABLE_COPY(QCPItemPosition)
  
};


class QCP_LIB_DECL QCPAbstractItem : public QCPLayerable
{
  Q_OBJECT
public:
  QCPAbstractItem(QCustomPlot *parentPlot);
  virtual ~QCPAbstractItem();
  
  // getters:
  bool clipToAxisRect() const { return mClipToAxisRect; }
  QCPAxis *clipKeyAxis() const { return mClipKeyAxis; }
  QCPAxis *clipValueAxis() const { return mClipValueAxis; }
  bool selectable() const { return mSelectable; }
  bool selected() const { return mSelected; }
  
  // setters:
  void setClipToAxisRect(bool clip);
  void setClipAxes(QCPAxis *keyAxis, QCPAxis *valueAxis);
  void setClipKeyAxis(QCPAxis *axis);
  void setClipValueAxis(QCPAxis *axis);
  void setSelectable(bool selectable);
  void setSelected(bool selected);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const = 0;
  QList<QCPItemPosition*> positions() const { return mPositions; }
  QList<QCPItemAnchor*> anchors() const { return mAnchors; }
  QCPItemPosition *position(const QString &name) const;
  QCPItemAnchor *anchor(const QString &name) const;
  bool hasAnchor(const QString &name) const;
  
protected:
  bool mClipToAxisRect;
  QCPAxis *mClipKeyAxis, *mClipValueAxis;
  bool mSelectable, mSelected;
  QList<QCPItemPosition*> mPositions;
  QList<QCPItemAnchor*> mAnchors;
  
  virtual QRect clipRect() const;
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  virtual void draw(QCPPainter *painter) = 0;
  
  // helper functions for subclasses:
  double distSqrToLine(const QPointF &start, const QPointF &end, const QPointF &point) const;
  double rectSelectTest(const QRectF &rect, const QPointF &pos, bool filledRect) const;
  
  // anchor/position interface:
  virtual QPointF anchorPixelPoint(int anchorId) const;
  QCPItemPosition *createPosition(const QString &name);
  QCPItemAnchor *createAnchor(const QString &name, int anchorId);
  
signals:
  void selectionChanged(bool selected);
  
private:
  Q_DISABLE_COPY(QCPAbstractItem)
  
  friend class QCustomPlot;
  friend class QCPItemAnchor;
};

#endif // QCP_ITEM_H
