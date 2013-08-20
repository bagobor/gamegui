#include "stdafx.h"

#include "color.h"


namespace gui
{
/*************************************************************************
	Construction & Destruction
*************************************************************************/
Color::Color() :
	m_alpha(1.0f),
	m_red(0.0f),
	m_green(0.0f),
	m_blue(0.0f),
	m_argb(0xFF000000)
{	
}


Color::Color(const Color& val)
{
	this->operator=(val);
}


Color::Color(float red, float green, float blue, float alpha) :
	m_alpha(alpha),
	m_red(red),
	m_green(green),
	m_blue(blue)
{
	calculateARGB();
}


Color::Color(argb_t argb)
{
	setARGB(argb);
}


float Color::getHue(void) const
{
	float pRed = m_red;
	float pGreen = m_green;
	float pBlue = m_blue;

	float pMax = std::max(std::max(m_red, m_green), m_blue);
	float pMin = std::min(std::min(m_red, m_green), m_blue);

	float pHue;

	if( pMax == pMin )
	{
		pHue = 0;
	}
	else
	{
		if( pMax == pRed )
		{
			pHue = (pGreen - pBlue) / (pMax - pMin);
		}
		else if( pMax == pGreen )
		{
			pHue = 2 + (pBlue - pRed) / (pMax - pMin);
		}
		else
		{
			pHue = 4 + (pRed - pGreen) / (pMax - pMin);
		}
	}

	float Hue = pHue / 6;
	if( Hue < 0 )
		Hue += 1;

	return Hue;
}


float Color::getSaturation(void) const
{
	float pMax = std::max(std::max(m_red, m_green), m_blue);
	float pMin = std::min(std::min(m_red, m_green), m_blue);

	float pLum = (pMax + pMin) / 2;
	float pSat;

	if( pMax == pMin )
	{
		pSat = 0;
	}
	else
	{
		if( pLum < 0.5 )
			pSat = (pMax - pMin) / (pMax + pMin);
		else
			pSat = (pMax - pMin) / (2 - pMax - pMin);
	}

	return pSat;
}


float Color::getLumination(void) const
{
	float pMax = std::max(std::max(m_red, m_green), m_blue);
	float pMin = std::min(std::min(m_red, m_green), m_blue);

	float pLum = (pMax + pMin) / 2;
	return pLum;
}


void Color::setARGB(argb_t argb)
{
	m_argb = argb;

#ifdef __BIG_ENDIAN__
    m_alpha	= static_cast<float>(argb & 0xFF) / 255.0f;
	argb >>= 8;
	m_red	= static_cast<float>(argb & 0xFF) / 255.0f;
	argb >>= 8;
	m_green	= static_cast<float>(argb & 0xFF) / 255.0f;
	argb >>= 8;
	m_blue	= static_cast<float>(argb & 0xFF) / 255.0f;
#else
	m_blue	= static_cast<float>(argb & 0xFF) / 255.0f;
	argb >>= 8;
	m_green	= static_cast<float>(argb & 0xFF) / 255.0f;
	argb >>= 8;
	m_red	= static_cast<float>(argb & 0xFF) / 255.0f;
	argb >>= 8;
	m_alpha	= static_cast<float>(argb & 0xFF) / 255.0f;
#endif
}


void Color::setHSL(float hue, float saturation, float luminance, float alpha)
{
	m_alpha = alpha;

	float temp3[3];

	float pHue = hue;
	float pSat = saturation;
	float pLum = luminance;

	if( pSat == 0 )
	{
		m_red = pLum;
		m_green = pLum;
		m_blue = pLum;
	}
	else
	{
		float temp2;
		if( pLum < 0.5f )
		{
			temp2 = pLum * (1 + pSat);
		}
		else
		{
			temp2 = pLum + pSat - pLum * pSat;
		}

		float temp1 = 2 * pLum - temp2;

		temp3[0] = pHue + (1.0f / 3);
		temp3[1] = pHue;
		temp3[2] = pHue - (1.0f / 3);

		for( int n = 0; n < 3; n ++ )
		{
			if( temp3[n] < 0 )
				temp3[n] ++;
			if( temp3[n] > 1 )
				temp3[n] --;

			if( (temp3[n] * 6) < 1 )
			{
				temp3[n] = temp1 + (temp2 - temp1) * 6 * temp3[n];
			}
			else
			{
				if( (temp3[n] * 2) < 1 )
				{
					temp3[n] = temp2;
				}
				else
				{
					if( (temp3[n] * 3) < 2 )
					{
						temp3[n] = temp1 + (temp2 - temp1) * ((2.0f / 3) - temp3[n]) * 6;
					}
					else
					{
						temp3[n] = temp1;
					}
				}
			}
		}

		m_red = temp3[0];
		m_green = temp3[1];
		m_blue = temp3[2];
	}

	calculateARGB();
}

void Color::invertColour()
{
	m_red	= 1.0f - m_red;
	m_green	= 1.0f - m_green;
	m_blue	= 1.0f - m_blue;
	calculateARGB();
}


void Color::invertColourWithAlpha()
{
	m_alpha	= 1.0f - m_alpha;
	m_red	= 1.0f - m_red;
	m_green	= 1.0f - m_green;
	m_blue	= 1.0f - m_blue;
	calculateARGB();
}

}