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

#ifndef QCP_LINEENDING_H
#define QCP_LINEENDING_H

#include "global.h"

class QCPPainter;

class QCP_LIB_DECL QCPLineEnding
{
public:
  /*!
    Defines the type of ending decoration for line-like items, e.g. an arrow.
    
    \image html QCPLineEnding.png
    
    The width and length of these decorations can be controlled with the functions \ref setWidth
    and \ref setLength. Some decorations like \ref esDisc, \ref esSquare, \ref esDiamond and \ref esBar only
    support a width, the length property is ignored.
    
    \see QCPItemLine::setHead, QCPItemLine::setTail, QCPItemCurve::setHead, QCPItemCurve::setTail
  */
  enum EndingStyle { esNone          ///< No ending decoration
                     ,esFlatArrow    ///< A filled arrow head with a straight/flat back (a triangle)
                     ,esSpikeArrow   ///< A filled arrow head with an indented back
                     ,esLineArrow    ///< A non-filled arrow head with open back
                     ,esDisc         ///< A filled circle
                     ,esSquare       ///< A filled square
                     ,esDiamond      ///< A filled diamond (45° rotated square)
                     ,esBar          ///< A bar perpendicular to the line
                   };
  
  QCPLineEnding();
  QCPLineEnding(EndingStyle style, double width=8, double length=10, bool inverted=false);
  
  // getters:
  EndingStyle style() const { return mStyle; }
  double width() const { return mWidth; }
  double length() const { return mLength; }
  bool inverted() const { return mInverted; }
  
  // setters:
  void setStyle(EndingStyle style);
  void setWidth(double width);
  void setLength(double length);
  void setInverted(bool inverted);
  
  // non-property methods:
  double boundingDistance() const;
  void draw(QCPPainter *painter, const QVector2D &pos, const QVector2D &dir) const;
  void draw(QCPPainter *painter, const QVector2D &pos, double angle) const;
  
protected:
  EndingStyle mStyle;
  double mWidth, mLength;
  bool mInverted;
};
Q_DECLARE_TYPEINFO(QCPLineEnding, Q_MOVABLE_TYPE);

#endif // QCP_LINEENDING_H
