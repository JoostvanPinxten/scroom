/*
 * Scroom - Generic viewer for 2D data
 * Copyright (C) 2009-2018 Kees-Jan Dijkzeul
 *
 * SPDX-License-Identifier: LGPL-2.1
 */

#pragma once

#include <cstdint>
#include <cmath>

#include <boost/operators.hpp>

#include <gdk/gdk.h>
#include <cairo.h>

inline uint8_t byteFromDouble(double d) { return uint8_t(255*d); }
inline double doubleFromByte(uint8_t b) { return b/255.0; }
inline uint16_t shortFromDouble(double d) { return uint16_t(0xFFFF*d); }

namespace
{
  // see http://stackoverflow.com/a/3943023
  double computeC(double c)
  {
    return c<=0.03928?c/12.92:pow((c+0.055)/1.055, 2.4);
  }
}

/**
 * Represent a (ARGB) color
 */
class Color : public boost::addable<Color>, public boost::multiplicative<Color,double>
{
public:
  double alpha;  /**< Alpha value */
  double red;    /**< R value */
  double green;  /**< G value */
  double blue;   /**< B value */

public:

  /** Default constructor. Create black */
  Color()
    : alpha(1.0), red(0.0), green(0.0), blue(0.0)
  {}

  /** Constructor. Use the given RGB values */
  Color(double red, double green, double blue, double alpha=1.0)
    : alpha(alpha), red(red), green(green), blue(blue)
  {}

  /** Constructor. Create the given gray value */
  explicit Color(double gray, double alpha=1.0)
    : alpha(alpha), red(gray), green(gray), blue(gray)
  {}

  Color& operator+=(const Color& rhs)
  { alpha += rhs.alpha; red += rhs.red; green += rhs.green; blue+= rhs.blue; return *this; }

  Color& operator/=(double d)
  { alpha /= d; red /= d; green /= d; blue /= d; return *this; }

  Color& operator*=(double d)
  { alpha *= d; red *= d; green *= d; blue *= d; return *this; }

  uint32_t getRGB24() const
  { return 0xFF000000 | byteFromDouble(red)<<16 | byteFromDouble(green)<<8 | byteFromDouble(blue) <<0; }

  uint32_t getARGB32() const
  { return byteFromDouble(alpha)<<24 | byteFromDouble(red)<<16 | byteFromDouble(green)<<8 | byteFromDouble(blue) <<0; }

  void setColor(cairo_t* cr) const
  { cairo_set_source_rgba(cr, red, green, blue, alpha); }

  GdkColor getGdkColor() const
  {
    return { 0, shortFromDouble(red), shortFromDouble(green), shortFromDouble(blue) };
  }

  Color getContrastingBlackOrWhite() const
  {
    // see http://stackoverflow.com/a/3943023
    double L = 0.2126 * computeC(red) + 0.7152 * computeC(green) + 0.0722 * computeC(blue);
    return Color(L>0.179?0:1);
  }

  Color& setAlpha(double alpha)
  { return *this *= alpha; }

  Color setAlpha(double alpha) const
  { return Color(*this).setAlpha(alpha); }
};

inline Color mix(const Color& a, const Color& b, double alpha)
{
  return a*alpha + b*(1.0-alpha);
}

