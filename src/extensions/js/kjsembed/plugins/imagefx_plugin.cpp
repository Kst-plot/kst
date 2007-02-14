// -*- c++ -*-

/*
 *  Copyright (C) 2003, Ian Reinhart Geiser <geiseri@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include <kdebug.h>
#include <kglobal.h>
#include <kjsembed/jsopaqueproxy.h>
#include <kjsembed/jsbinding.h>
#include <kjsembed/jsfactory.h>
#include <kjsembed/jsfactory_imp.h>
#include <kjsembed/kjsembedpart.h>
#include <kjsembed/customobject_imp.h>
#include <qvariant.h>
#include <qbrush.h>

#include "imagefx_plugin.h"

namespace KJSEmbed {
namespace Bindings {


ImageFXLoader::ImageFXLoader( QObject *parent, const char *name, const QStringList &args ) :
	JSBindingPlugin(parent, name, args)
{
}

KJS::Object ImageFXLoader::createBinding(KJSEmbedPart */*jspart*/, KJS::ExecState *exec, const KJS::List &/*args*/) const
{
	kdDebug() << "Loading a ImageFX object" << endl;
	JSOpaqueProxy *prx = new JSOpaqueProxy(  (int*)0, "ImageFX" );

	KJS::Object proxyObj(prx);
	ImageFX::addBindings( exec, proxyObj );
	return proxyObj;
}

ImageFX::ImageFX( KJS::ExecState *exec, int id )
    : JSProxyImp(exec), mid(id)
{
}

ImageFX::~ImageFX()
{
}

void ImageFX::addBindings( KJS::ExecState *exec, KJS::Object &object ) {

    kdDebug() << "ImageFX::addBindings()" << endl;
    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( object.imp() );
    if ( !op ) {
        kdWarning() << "ImageFX::addBindings() failed, not a JSOpaqueProxy" << endl;
        return;
    }

    if ( op->typeName() != "ImageFX" ) {
	kdWarning() << "ImageFX::addBindings() failed, type is " << op->typeName() << endl;
	return;
    }

    JSProxy::MethodTable methods[] = {
	{ Methodgradient, "gradient" },
	{ MethodunbalancedGradient, "unbalancedGradient" },
	{ MethodblendColor, "blendColor" },
	{ MethodblendImage, "blendImage" },
	{ MethodcomputeDestinationRect, "computeDestinationRect" },
	{ MethodchannelIntensity, "channelIntensity" },
	{ Methodfade, "fade" },
	{ Methodflatten, "flatten" },
	{ Methodhash, "hash" },
	{ Methodintensity, "intensity" },
	{ Methodmodulate, "modulate" },
	{ MethodtoGray, "toGray" },
	{ Methoddesaturate, "desaturate" },
	{ Methoddither, "dither" },
	{ MethodselectedImage, "selectedImage" },
	{ MethodcontrastHSV, "contrastHSV" },
	{ Methodnormalize, "normalize" },
	{ Methodequalize, "equalize" },
	{ Methodthreshold, "threshold" },
	{ Methodsolarize, "solarize" },
	{ Methodemboss, "emboss" },
	{ Methoddespeckle, "despeckle" },
	{ Methodcharcoal, "charcoal" },
	{ Methodcharcoal2, "charcoal2" },
	{ Methodrotate, "rotate" },
	{ Methodsample, "sample" },
	{ MethodaddNoise, "addNoise" },
	{ Methodblur, "blur" },
	{ Methodedge, "edge" },
	{ Methodimplode, "implode" },
	{ MethodoilPaintConvolve, "oilPaintConvolve" },
	{ MethodoilPaint, "MethodoilPaint" },
	{ Methodsharpen, "sharpen" },
	{ Methodsharpen2, "sharpen2" },
	{ Methodspread, "spread" },
	{ Methodshade, "shade" },
	{ Methodswirl, "swirl" },
	{ Methodwave, "wave" },
	{ Methodcontrast, "contrast" },
	{ MethodbumpMap, "bumpmap" },
	{ 0, 0 }
    };

    int idx = 0;
    do {
        ImageFX *meth = new ImageFX( exec, methods[idx].id );
        object.put( exec , methods[idx].name, KJS::Object(meth) );
        ++idx;
    } while( methods[idx].id );

    //
    // Define the enum constants
    //
    struct EnumValue {
	const char *id;
	int val;
    };

    EnumValue enums[] = {
	// GradiantType
	{ "VerticalGradient", 0 },
	{ "HorizontalGradient", 1 },
	{ "DiagonalGradient", 2 },
	{ "CrossDiagonalGradient", 3 },
	{ "PyramidGradient", 4 },
	{ "RectangleGradient", 5 },
	{ "PipeCrossGradient", 6 },
	{ "EllipticGradient", 7 },
	// RGBComponent
	{ "Red", 0 },
	{ "Green", 1 },
	{ "Blue", 2 },
	{ "Gray", 3 },
	{ "All", 4 },
        // Lighting
	{ "NorthLite", 0 },
	{ "NWLite", 1 },
	{ "WestLite", 2 },
	{ "SWLite", 3 },
	{ "SouthLite", 4 },
	{ "SELite", 5 },
	{ "EastLite", 6 },
	{ "NELite", 7 },
        // ModulationType
	{ "Intensity", 0 },
	{ "Saturation", 1 },
	{ "HueShift", 2 },
	{ "Contrast", 3 },
        // NoiseType
	{ "UniformNoise", 0 },
	{ "GaussianNoise", 1 },
	{ "MultiplicativeGaussianNoise", 2 },
	{ "ImpulseNoise", 3 },
	{ "LaplacianNoise", 4 },
	{ "PoissonNoise", 5 },
        // RotateDirection
	{ "Rotate90", 0 },
	{ "Rotate180", 1 },
	{ "Rotate270", 2 },
	// BumpmapType
	{ "Linear", 0},
	{ "Spherical", 1},
	{ "Sinuosidal", 2},
	{ 0, 0 }
    };

    int enumidx = 0;
    do {
        object.put( exec, enums[enumidx].id, KJS::Number(enums[enumidx].val), KJS::ReadOnly );
        ++enumidx;
    } while( enums[enumidx].id );
}

KJS::Value ImageFX::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {

    kdDebug() << "ImageFX::call() " << mid << endl;
    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );
    if ( !op ) {
        kdWarning() << "ImageFX::call() failed, not a JSOpaqueProxy" << endl;
        return KJS::Value();
    }

    if ( op->typeName() != "ImageFX" ) {
	kdWarning() << "ImageFX::call() failed, type is " << op->typeName() << endl;
	return KJS::Value();
    }


    KJS::Value retValue = KJS::Value();
    switch ( mid ) {
    case Methodgradient: {
            QSize size = extractQSize(exec, args, 0);
            QColor ca = extractQColor(exec, args, 1);
            QColor cb = extractQColor(exec, args, 2);
            int type = extractInt( exec, args, 3);
            int ncols = extractInt( exec, args, 4);
	    QImage img = KImageEffect::gradient(size, ca, cb, (KImageEffect::GradientType)type, ncols);
	    retValue = convertToValue(exec, img);
            break;
        }
    case MethodunbalancedGradient: {
            QSize size = extractQSize(exec, args, 0);
            QColor ca = extractQColor(exec, args, 1);
            QColor cb = extractQColor(exec, args, 2);
            int type = extractInt( exec, args, 3);
            int xfactor = extractInt( exec, args, 4);
            int yfactor = extractInt( exec, args, 5);
            int ncols = extractInt( exec, args, 6);
            QImage img = KImageEffect::unbalancedGradient(size, ca, cb, (KImageEffect::GradientType)type, xfactor, yfactor, ncols);
	    retValue = convertToValue(exec, img);
            break;
        }
    case MethodblendColor: {
            QColor clr = extractQColor(exec, args, 0);
	    QImage dst = extractQImage(exec, args, 1);
            float opacity = (float)extractDouble(exec, args, 2);
	    QImage img = KImageEffect::blend(clr, dst, opacity);
	    retValue = convertToValue(exec, img);
            break;
        }
    case MethodblendImage: {
            QImage src = extractQImage(exec, args, 0);
	    QImage dst = extractQImage(exec, args, 1);
            float opacity = (float)extractDouble(exec, args, 2);
	    QImage img = KImageEffect::blend(src, dst, opacity);
	    retValue = convertToValue(exec, img);
            break;
        }
    case MethodcomputeDestinationRect: {
            QSize lowerSize = extractQSize(exec, args, 0);
            int disposition = extractInt(exec, args, 1);
	    QImage upper = extractQImage(exec, args, 2);
	    QRect rect = KImageEffect::computeDestinationRect(lowerSize,  (KImageEffect::Disposition) disposition, upper);
            retValue = convertToValue(exec, rect);
	    break;
        }
    case MethodchannelIntensity: {
    	    QImage image = extractQImage(exec, args, 0);
            float percent = (float)extractDouble(exec, args, 1);
            int channel = extractInt(exec, args, 2);
	    QImage img = KImageEffect::channelIntensity(image, percent, (KImageEffect::RGBComponent)channel);
            retValue = convertToValue(exec, img);
	    break;
        }
    case Methodfade: {
            QImage image = extractQImage(exec, args, 0);
            float val = (float)extractDouble(exec, args, 1);
            QColor color = extractQColor(exec, args, 2);
	    QImage img = KImageEffect::fade(image, val, color);
            retValue = convertToValue(exec, img);
            break;
        }
    case Methodflatten: {
            QImage image = extractQImage(exec, args, 0);
	    QColor ca = extractQColor(exec, args, 1);
            QColor cb = extractQColor(exec, args, 2);
            int ncols = extractInt(exec, args, 3);
	    QImage img = KImageEffect::flatten(image, ca, cb, ncols);
            retValue = convertToValue(exec, img);
            break;
        }
    case Methodhash: {

            QImage image = extractQImage(exec, args, 0);
	    int lite  = extractInt(exec, args, 1);
            int spacing = extractInt(exec, args, 2);
	    QImage img = KImageEffect::hash(image, (KImageEffect::Lighting)lite, spacing);
            retValue = convertToValue(exec, img);
            break;
        }
    case Methodintensity: {

	    QImage image = extractQImage(exec, args, 0);
	    float percent = (float)extractDouble(exec, args, 1);
            QImage img = KImageEffect::intensity(image, percent);
	    retValue = convertToValue(exec, img);
            break;
        }
    case Methodmodulate: {
            QImage image = extractQImage(exec, args, 0);
	    QImage modImage = extractQImage(exec, args, 0);
            bool reverse = extractBool(exec, args, 1);
            int type = extractInt(exec, args, 2);
            int factor = extractInt(exec, args, 3);
            int channel = extractInt(exec, args, 4);
	    QImage img = KImageEffect::modulate(image, modImage, reverse, (KImageEffect::ModulationType)type, factor, (KImageEffect::RGBComponent)channel);
            retValue = convertToValue(exec, img);
            break;
        }

    case MethodtoGray: {
            QImage image = extractQImage(exec, args, 0);
	    bool fast = extractBool(exec, args, 1);
            QImage img = KImageEffect::toGray(image, fast);
	    retValue = convertToValue(exec, img);
            break;
        }
    case Methoddesaturate: {
            QImage image = extractQImage(exec, args, 0);
	    float desat = (float)extractDouble(exec, args, 1);
            QImage img = KImageEffect::desaturate(image, desat);
	    retValue = convertToValue(exec, img);
            break;
        }
    case Methoddither: {

            //dither( palette, size)
            break;
        }
    case MethodselectedImage: {

            QImage image = extractQImage(exec, args, 0);
	    QColor col = extractQColor(exec, args, 1);
            QImage img = KImageEffect::selectedImage(image, col);
	    retValue = convertToValue(exec, img);
            break;
        }
    case MethodcontrastHSV: {
	    QImage image = extractQImage(exec, args, 0);
	    bool sharpen = extractBool(exec, args, 1);
            KImageEffect::contrastHSV(image, sharpen);
	    retValue = convertToValue(exec, image);
            break;
        }
    case Methodnormalize: {
            QImage image = extractQImage(exec, args, 0);
            KImageEffect::normalize(image);
	    retValue = convertToValue(exec, image);
            break;
        }
    case Methodequalize: {
            QImage image = extractQImage(exec, args, 0);
            KImageEffect::equalize(image);
	    retValue = convertToValue(exec, image);
            break;
        }
    case Methodthreshold: {
            QImage image = extractQImage(exec, args, 0);
	    uint value = extractUInt(exec, args, 1);
            KImageEffect::threshold(image, value);
	    retValue = convertToValue(exec, image);
            break;
        }
    case Methodsolarize: {
            QImage image = extractQImage(exec, args, 0);
            double factor = extractDouble(exec, args, 1);
            KImageEffect::solarize(image, factor);
	    retValue = convertToValue(exec, image);
            break;
        }
    case Methodemboss: {
            QImage image = extractQImage(exec, args, 0);
	    double radius = extractDouble(exec, args, 1);
            double sigma = extractDouble(exec, args, 2);
            QImage img = KImageEffect::emboss(image, radius, sigma);
	    retValue = convertToValue(exec, img);
            break;
        }
    case Methoddespeckle: {
            QImage image = extractQImage(exec, args, 0);
	    QImage img = KImageEffect::despeckle(image);
	    retValue = convertToValue(exec, img);
            break;
        }
    case Methodcharcoal: {
            QImage image = extractQImage(exec, args, 0);
	    double factor = extractDouble(exec, args, 1);
            QImage img = KImageEffect::charcoal( image, factor);
	    retValue = convertToValue(exec, img);
            break;
        }
    case Methodcharcoal2: {
            QImage image = extractQImage(exec, args, 0);
	    double radius = extractDouble(exec, args, 1);
            double sigma = extractDouble(exec, args, 2);
            QImage img = KImageEffect::charcoal(image, radius, sigma);
	    retValue = convertToValue(exec, img);
            break;
        }
    case Methodrotate: {
            QImage image = extractQImage(exec, args, 0);
	    int r = extractInt(exec, args, 1);
            QImage img = KImageEffect::rotate(image, (KImageEffect::RotateDirection) r);
	    retValue = convertToValue(exec, img);
            break;
        }
    case Methodsample: {
            QImage image = extractQImage(exec, args, 0);
            int width = extractInt(exec, args, 1);
            int height = extractInt(exec, args, 2);
            QImage img = KImageEffect::sample(image, width, height);
	    retValue = convertToValue(exec, img);
            break;
        }
    case MethodaddNoise: {
            QImage image = extractQImage(exec, args, 0);
            int type = extractInt(exec, args, 1);
            QImage img = KImageEffect::addNoise(image, (KImageEffect::NoiseType) type);
	    retValue = convertToValue(exec, img);
            break;
        }
    case Methodblur: {
            QImage image = extractQImage(exec, args, 0);
	    double radius = extractDouble(exec, args, 1);
            double sigma = extractDouble(exec, args, 2);
            QImage img = KImageEffect::blur(image, radius, sigma);
	    retValue = convertToValue(exec, img);
            break;
        }
    case Methodedge: {
            QImage image = extractQImage(exec, args, 0);
	    double radius = extractDouble(exec, args, 1);
            QImage img = KImageEffect::edge(image, radius);
            retValue = convertToValue(exec, img);
	    break;
        }
    case Methodimplode: {
            QImage image = extractQImage(exec, args, 0);
	    double factor = extractDouble(exec, args, 1);
            uint background = extractUInt(exec, args, 2);
            QImage img = KImageEffect::implode(image, factor, background);
            retValue = convertToValue(exec, img);
	    break;
        }
    case MethodoilPaintConvolve: {
            QImage image = extractQImage(exec, args, 0);
	    double radius = extractDouble(exec, args, 1);
            QImage img = KImageEffect::oilPaintConvolve(image, radius);
	    retValue = convertToValue(exec, img);
            break;
        }
    case MethodoilPaint: {
            QImage image = extractQImage(exec, args, 0);
            int radius = extractInt(exec, args, 1);
            QImage img = KImageEffect::oilPaint(image, radius);
	    retValue = convertToValue(exec, img);
            break;
        }


    case Methodsharpen: {
            QImage image = extractQImage(exec, args, 0);
	    double factor = extractDouble(exec, args, 1);
            QImage img = KImageEffect::sharpen(image, factor);
	    retValue = convertToValue(exec, img);
            break;
        }
    case Methodsharpen2: {
            QImage image = extractQImage(exec, args, 0);
	    double radius = extractDouble(exec, args, 1);
            double sigma = extractDouble(exec, args, 2);
            QImage img = KImageEffect::sharpen(image, radius, sigma);
	    retValue = convertToValue(exec, img);
            break;
        }
    case Methodspread: {
            QImage image = extractQImage(exec, args, 0);
	    uint amount = extractUInt(exec, args, 1);
            QImage img = KImageEffect::spread(image, amount);
            retValue = convertToValue(exec, img);
	    break;
        }
    case Methodshade: {
            QImage image = extractQImage(exec, args, 0);
	    bool color_shading = extractBool(exec, args, 1);
            double azimuth = extractDouble(exec, args, 2);
            double elevation = extractDouble(exec, args, 3);
            QImage img = KImageEffect::shade(image, color_shading, azimuth, elevation);
            retValue = convertToValue(exec, img);
	    break;
        }
    case Methodswirl: {
            QImage image = extractQImage(exec, args, 0);
	    double degrees  = extractDouble(exec, args, 1);
            uint background = extractUInt(exec, args, 2);
            QImage img = KImageEffect::swirl(image, degrees, background);
            retValue = convertToValue(exec, img);
	    break;
        }
    case Methodwave: {
            QImage image = extractQImage(exec, args, 0);
	    double amplitude  = extractDouble(exec, args, 1);
            double frequency  = extractDouble(exec, args, 2);
            uint background = extractUInt(exec, args, 3);
            QImage img = KImageEffect::wave(image, amplitude, frequency, background);
            retValue = convertToValue(exec, img);
	    break;
        }
    case Methodcontrast: {
            QImage image = extractQImage(exec, args, 0);
	    int c = extractInt(exec, args, 1);
            QImage img = KImageEffect::contrast(image, c);
            retValue = convertToValue(exec, img);
	    break;
        }
    case MethodbumpMap: {
        QImage mask = extractQImage(exec, args, 0);
        QImage img = bumpmap(img,
	                                        mask,
						extractDouble(exec, args, 1),
						extractDouble(exec, args, 2),
						extractInt(exec, args, 3),
						extractInt(exec, args, 4),
						extractInt(exec, args, 5),
						extractInt(exec, args, 6),
						extractInt(exec, args, 7),
						extractBool(exec, args, 8),
						extractBool(exec, args, 9),
						(BumpmapType) extractInt(exec, args, 10),
						extractBool(exec, args, 11));

	retValue = convertToValue(exec, img);
	break;
    }
    default:
        kdWarning() << "ImageFX has no method " << mid << endl;
        break;
    }

    return retValue;
}

/***********************************************************************
 * Here's a pretty fast bumpmap implementation.
 * NOTE: remind me to move it to KImageEffects after 3.2.
 */
#define DegreesToRadians(x) ((x)*M_PI/180.0)
#define MOD(x, y) ((x) < 0 ? ((y) - 1 - ((y) - 1 - (x)) % (y)) : (x) % (y))

/**
 * NOTE: kclamp needs to be moved to kglobals.h along kmin and kmax
 */
#define KCLAMP(x,low,high) kClamp(x,low,high)
template<class T>
inline const T& kClamp( const T& x, const T& low, const T& high )
{
    if ( x < low )
        return low;
    else if ( x > high )
        return high;
    else
        return x;
}

static inline unsigned int intensityValue( unsigned int color ) {
    return (unsigned int)( (0.299*qRed( color ) +
                            0.587*qGreen( color ) +
                            0.1140000000000001*qBlue( color ) ) );
}

struct BumpmapParams {
    BumpmapParams( double bm_azimuth, double bm_elevation,
                   int bm_depth, BumpmapType bm_type,
                   bool invert ) {
         /* Convert to radians */
        double azimuth = DegreesToRadians( bm_azimuth );
        double elevation = DegreesToRadians( bm_elevation );

        /* Calculate the light vector */
        lx = (int)( cos(azimuth) * cos(elevation) * 255.0 );
        ly = (int)( sin(azimuth) * cos(elevation) * 255.0 );
        int lz         = (int)( sin(elevation) * 255.0 );

        /* Calculate constant Z component of surface normal */
        int nz  = (6 * 255) / bm_depth;
        nz2     = nz * nz;
        nzlz    = nz * lz;

        /* Optimize for vertical normals */
        background = lz;

        /* Calculate darkness compensation factor */
        compensation = sin(elevation);

        /* Create look-up table for map type */
        for (int i = 0; i < 256; i++)
        {
            double n = 0;
            switch (bm_type)
            {
            case Spherical:
                n = i / 255.0 - 1.0;
                lut[i] = (int) (255.0 * sqrt(1.0 - n * n) + 0.5);
                break;

            case Sinuosidal:
                n = i / 255.0;
                lut[i] = (int) (255.0 * (sin((-M_PI / 2.0) + M_PI * n) + 1.0) /
                                        2.0 + 0.5);
                break;

            case Linear:
            default:
                lut[i] = i;
            }

            if (invert)
                lut[i] = 255 - lut[i];
        }
    }
    int lx,  ly;
    int nz2, nzlz;
    int background;
    double compensation;
    uchar lut[256];
};


static void bumpmap_convert_row( uint *row,
                                 int    width,
                                 int    bpp,
                                 int    has_alpha,
                                 uchar *lut,
                                 int waterlevel )
{
  uint *p;

  p = row;

  has_alpha = has_alpha ? 1 : 0;

  if (bpp >= 3)
      for (; width; width--)
      {
          if (has_alpha) {
              unsigned int idx = (unsigned int)(intensityValue( *row ) + 0.5);
              *p++ = lut[(unsigned int) ( waterlevel +
                                          ( ( idx -
                                              waterlevel) * qBlue( *row )) / 255.0 )];
          } else {
              unsigned int idx = (unsigned int)(intensityValue( *row ) + 0.5);
              *p++ = lut[idx];
          }

          ++row;
      }
}

static void bumpmap_row( uint           *src,
                         uint           *dest,
                         int              width,
                         int              bpp,
                         int              has_alpha,
                         uint           *bm_row1,
                         uint           *bm_row2,
                         uint           *bm_row3,
                         int              bm_width,
                         int              bm_xofs,
                         bool          tiled,
                         bool          row_in_bumpmap,
                         int           ambient,
                         bool          compensate,
                         BumpmapParams *params )
{
    int xofs1, xofs2, xofs3;
    int shade;
    int ndotl;
    int nx, ny;
    int x;
    int pbpp;
    int tmp;

    if (has_alpha)
        pbpp = bpp - 1;
    else
        pbpp = bpp;

    tmp = bm_xofs;
    xofs2 = MOD(tmp, bm_width);

    for (x = 0; x < width; x++)
    {
        /* Calculate surface normal from bump map */

        if (tiled || (row_in_bumpmap &&
                      x >= - tmp && x < - tmp + bm_width)) {
            if (tiled) {
                xofs1 = MOD(xofs2 - 1, bm_width);
                xofs3 = MOD(xofs2 + 1, bm_width);
	    } else {
                xofs1 = KCLAMP(xofs2 - 1, 0, bm_width - 1);
                xofs3 = KCLAMP(xofs2 + 1, 0, bm_width - 1);
	    }
            nx = (bm_row1[xofs1] + bm_row2[xofs1] + bm_row3[xofs1] -
                  bm_row1[xofs3] - bm_row2[xofs3] - bm_row3[xofs3]);
            ny = (bm_row3[xofs1] + bm_row3[xofs2] + bm_row3[xofs3] -
                  bm_row1[xofs1] - bm_row1[xofs2] - bm_row1[xofs3]);
	} else {
            nx = ny = 0;
        }

      /* Shade */

        if ((nx == 0) && (ny == 0))
            shade = params->background;
        else {
            ndotl = nx * params->lx + ny * params->ly + params->nzlz;

            if (ndotl < 0)
                shade = (int)( params->compensation * ambient );
            else {
                shade = (int)( ndotl / sqrt(nx * nx + ny * ny + params->nz2) );

                shade = (int)( shade + KMAX(0.0, (255 * params->compensation - shade)) *
                               ambient / 255 );
	    }
	}

        /* Paint */

        /**
         * NOTE: if we want to work with non-32bit images the alpha handling would
         * also change
         */
        if (compensate) {
            int red = (int)((qRed( *src ) * shade) / (params->compensation * 255));
            int green = (int)((qGreen( *src ) * shade) / (params->compensation * 255));
            int blue = (int)((qBlue( *src ) * shade) / (params->compensation * 255));
            int alpha = (int)((qAlpha( *src ) * shade) / (params->compensation * 255));
            ++src;
            *dest++ = qRgba( red, green, blue, alpha );
        } else {
            int red = qRed( *src ) * shade / 255;
            int green = qGreen( *src ) * shade / 255;
            int blue = qBlue( *src ) * shade / 255;
            int alpha = qAlpha( *src ) * shade / 255;
            ++src;
            *dest++ = qRgba( red, green, blue, alpha );
        }

        /* Next pixel */

        if (++xofs2 == bm_width)
            xofs2 = 0;
    }
}

/**
 * A bumpmapping algorithm.
 *
 * @param img the image you want bumpmap
 * @param map the map used
 * @param azimuth azimuth
 * @param elevation elevation
 * @param depth depth (not the depth of the image, but of the map)
 * @param xofs X offset
 * @param yofs Y offset
 * @param waterlevel level that full transparency should represent
 * @param ambient ambient lighting factor
 * @param compensate compensate for darkening
 * @param invert invert bumpmap
 * @param type type of the bumpmap
 *
 * @return The destination image (dst) containing the result.
 * @author Zack Rusin <zack@kde.org>
 */
QImage ImageFX::bumpmap(QImage &img, QImage &map, double azimuth, double elevation,
               int depth, int xofs, int yofs, int waterlevel,
               int ambient, bool compensate, bool invert,
               BumpmapType type, bool tiled)
{
    QImage dst;

    if ( img.depth() != 32 || img.depth() != 32 ) {
        qWarning( "Bump-mapping effect works only with 32 bit images");
        return dst;
    }

    dst.create( img.width(), img.height(), img.depth() );
    int bm_width  = map.width();
    int bm_height = map.height();
    int bm_bpp = map.depth();
    int bm_has_alpha = map.hasAlphaBuffer();

    int yofs1, yofs2, yofs3;

    if ( tiled ) {
        yofs2 = MOD( yofs, bm_height );
        yofs1 = MOD( yofs2 - 1, bm_height);
        yofs3 = MOD( yofs2 + 1, bm_height);
    } else {
        yofs1 = 0;
        yofs2 = 0;
        yofs3 = KCLAMP( yofs2+1, 0, bm_height - 1 );
    }

    BumpmapParams params( azimuth, elevation, depth, type, invert );

    uint* bm_row1 = (unsigned int*)map.scanLine( yofs1 );
    uint* bm_row2 = (unsigned int*)map.scanLine( yofs2 );
    uint* bm_row3 = (unsigned int*)map.scanLine( yofs3 );

    bumpmap_convert_row( bm_row1, bm_width, bm_bpp, bm_has_alpha, params.lut, waterlevel );
    bumpmap_convert_row( bm_row2, bm_width, bm_bpp, bm_has_alpha, params.lut, waterlevel );
    bumpmap_convert_row( bm_row3, bm_width, bm_bpp, bm_has_alpha, params.lut, waterlevel );

    for (int y = 0; y < img.height(); ++y)
    {
        int row_in_bumpmap = (y >= - yofs && y < - yofs + bm_height);

        uint* src_row = (unsigned int*)img.scanLine( y );
        uint* dest_row = (unsigned int*)dst.scanLine( y );

        bumpmap_row( src_row, dest_row, img.width(), img.depth(), img.hasAlphaBuffer(),
                     bm_row1, bm_row2, bm_row3, bm_width, xofs,
                     tiled,
                     row_in_bumpmap, ambient, compensate,
                     &params );

        /* Next line */

        if (tiled || row_in_bumpmap)
	{
            uint* bm_tmprow = bm_row1;
            bm_row1   = bm_row2;
            bm_row2   = bm_row3;
            bm_row3   = bm_tmprow;

            if (++yofs2 == bm_height)
                yofs2 = 0;

            if (tiled)
                yofs3 = MOD(yofs2 + 1, bm_height);
            else
                yofs3 = KCLAMP(yofs2 + 1, 0, bm_height - 1);

            bm_row3 = (unsigned int*)map.scanLine( yofs3 );
            bumpmap_convert_row( bm_row3, bm_width, bm_bpp, bm_has_alpha,
                                 params.lut, waterlevel );
	}
    }
    return dst;
}


} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

#include <kgenericfactory.h>
typedef KGenericFactory<KJSEmbed::Bindings::ImageFXLoader> ImageFXLoaderFactory;
K_EXPORT_COMPONENT_FACTORY( libimagefxplugin, ImageFXLoaderFactory( "ImageFXLoader" ) )
