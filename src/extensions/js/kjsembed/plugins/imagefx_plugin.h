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

#ifndef KJSEMBED_IMAGEFX_PLUGIN_H
#define KJSEMBED_IMAGEFX_PLUGIN_H

#include <kjsembed/jsbindingplugin.h>
#include <kjsembed/jsproxy_imp.h>
#include <kimageeffect.h>

#include <qimage.h>

namespace KJSEmbed {
namespace Bindings {

class ImageFXLoader : public JSBindingPlugin
{
public:
    ImageFXLoader( QObject *parent, const char *name, const QStringList &args );
    KJS::Object createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const;
};
enum BumpmapType {
Linear,
Spherical,
Sinuosidal
};

class ImageFX : JSProxyImp
{
	
	/** Identifiers for the methods provided by this class. */
	enum MethodId {
	// Image effects 
	Methodgradient,
	MethodunbalancedGradient, MethodblendColor,MethodblendImage,
	MethodcomputeDestinationRect, MethodchannelIntensity, Methodfade,
	Methodflatten, Methodhash, Methodintensity, Methodmodulate, MethodtoGray,
	Methoddesaturate, Methoddither, MethodselectedImage, MethodcontrastHSV,
	Methodnormalize, Methodequalize, Methodthreshold, Methodsolarize,
	Methodemboss, Methoddespeckle, Methodcharcoal, Methodcharcoal2,
	Methodrotate, Methodsample, MethodaddNoise, Methodblur, Methodedge,
	Methodimplode, MethodoilPaintConvolve, MethodoilPaint, Methodsharpen,
	Methodsharpen2, Methodspread, Methodshade, Methodswirl, Methodwave,
	Methodcontrast, MethodbumpMap 
	};
	
	public:
		ImageFX( KJS::ExecState *exec, int id );
		virtual ~ImageFX();
		
		static void addBindings( KJS::ExecState *exec, KJS::Object &object );
		
		/** Returns true iff this object implements the call function. */
		virtual bool implementsCall() const { return true; }
		
		/** Invokes the call function. */
		virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );
	
	private:
		// Image effects
		void gradient(const QSize &size, const QColor &ca, const QColor &cb, KImageEffect::GradientType type, int ncols);
		void unbalancedGradient(const QSize &size, const QColor &ca,const QColor &cb, KImageEffect::GradientType type, int xfactor,int yfactor, int ncols);
		void blendColor(const QColor& clr, float opacity);
		void blendImage(QImage& blendImage, float opacity);
		QRect computeDestinationRect(const QSize &lowerSize,KImageEffect::Disposition disposition);
		
		void channelIntensity(float percent,KImageEffect::RGBComponent channel);
		void fade(float val, const QColor &color);
		void flatten(const QColor &ca,const QColor &cb, int ncols);
		void hash(KImageEffect::Lighting lite,uint spacing);
		void intensity(float percent);
		void modulate(QImage &modImage, bool reverse,KImageEffect::ModulationType type, int factor, KImageEffect::RGBComponent channel);
		void toGray(bool fast);
		void desaturate(float desat);
		void contrast(int c);
		void dither(const QColor *palette, int size);
		void selectedImage(  const QColor &col );
		void contrastHSV( bool sharpen);
		void normalize();
		void equalize();
		void threshold( uint value);
		void solarize( double factor);
		void emboss(double radius, double sigma);
		//void emboss();
		void despeckle();
		void charcoal(double radius, double sigma);
		void charcoal(double factor);
		void rotate(KImageEffect::RotateDirection r);
		void sample(int w, int h);
		void addNoise(KImageEffect::NoiseType type);
		void blur(double radius, double sigma);
		void edge(double radius);
		void implode(double factor, uint background );
		void oilPaintConvolve(double radius);
		void oilPaint(int radius);
		void sharpen(double radius, double sigma);
		void sharpen(double factor);
		void spread(uint amount);
		void shade(bool color_shading, double azimuth,double elevation);
		void swirl(double degrees, uint background);
		void wave(double amplitude, double frequency,uint background);
		
		QImage bumpmap(QImage &img, QImage &map, double azimuth=135.0, double elevation=45.0, int depth=3, int xofs=0, int yofs=0, int waterlevel=0, int ambient=0, bool compensate=false, bool invert=false, BumpmapType type=Linear, bool tiled=false);
			
		int mid;
};


} // namespace
} // namespace

#endif // KJSEMBED_IMAGEFX_PLUGIN_H
