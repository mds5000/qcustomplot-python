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

#ifndef QCP_ITEM_BRACKET_H
#define QCP_ITEM_BRACKET_H

#include "../global.h"
#include "../item.h"

class QCPPainter;
class QCustomPlot;

class QCP_LIB_DECL QCPItemBracket : public QCPAbstractItem
{
  Q_OBJECT
public:
  enum BracketStyle { bsSquare  ///< A brace with angled edges
                      ,bsRound  ///< A brace with round edges
                      ,bsCurly  ///< A curly brace
                      ,bsCalligraphic ///< A curly brace with varying stroke width giving a calligraphic impression
  };

  QCPItemBracket(QCustomPlot *parentPlot);
  virtual ~QCPItemBracket();
  
  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  double length() const { return mLength; }
  BracketStyle style() const { return mStyle; }
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setLength(double length);
  void setStyle(BracketStyle style);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  QCPItemPosition * const left;
  QCPItemPosition * const right;
  QCPItemAnchor * const center;
  
protected:
  enum AnchorIndex {aiCenter};
  QPen mPen, mSelectedPen;
  double mLength;
  BracketStyle mStyle;
  
  virtual void draw(QCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // helper functions:
  QPen mainPen() const;
};

#endif // QCP_ITEM_BRACKET_H
