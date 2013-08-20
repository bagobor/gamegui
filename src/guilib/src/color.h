#pragma once


namespace gui
{
//!< 32 bit ARGB representation of a Color.
typedef unsigned int argb_t;    

/*!
\brief
	Class representing Color values within the system.
*/
class  Color
{
public:
	/*************************************************************************
		Construction & Destruction
	*************************************************************************/
	Color(void);
	Color(const Color& val);
	Color(float red, float green, float blue, float alpha = 1.0f);
	Color(argb_t argb);

	/*************************************************************************
		Accessors
	*************************************************************************/
	__inline argb_t	getARGB() const	{	return m_argb;	}
	
	__inline float	getAlpha() const	{return m_alpha;}
	__inline float	getRed() const		{return m_red;}
	__inline float	getGreen() const	{return m_green;}
	__inline float	getBlue() const		{return m_blue;}

	float	getHue() const;
	float	getSaturation() const;
	float	getLumination() const;


	/*************************************************************************
		Manipulators
	*************************************************************************/
	void	setARGB(argb_t argb);
	inline void setAlpha(float alpha)
    {
        m_alpha = alpha;
		calculateARGB();
    }

	inline void setRed(float red)
    {   
        m_red = red;
		calculateARGB();
    }

	inline void setGreen(float green)
    {
        m_green = green;
		calculateARGB();
    }

	inline void setBlue(float blue)
    {
        m_blue = blue;
		calculateARGB();
    }

	inline void set(float red, float green, float blue, float alpha = 1.0f)
    {
        m_alpha = alpha;
        m_red = red;
        m_green = green;
        m_blue = blue;
		calculateARGB();
    }

	inline void setRGB(float red, float green, float blue)
    {
        m_red = red;
        m_green = green;
        m_blue = blue;
		calculateARGB();
    }

	inline void setRGB(const Color& val)
    {
        m_red = val.m_red;
        m_green = val.m_green;
        m_blue = val.m_blue;
        m_argb = (m_argb & 0xFF000000) | (val.m_argb & 0x00FFFFFF);
    }

	void	setHSL(float hue, float saturation, float luminance, float alpha = 1.0f);

	void	invertColour();
	void	invertColourWithAlpha();

	/*************************************************************************
		Operators
	*************************************************************************/
	inline Color& operator=(argb_t val)
    {
        setARGB(val);
        return *this;
    }

	inline Color& operator=(const Color& val)
    {
        m_alpha = val.m_alpha;
        m_red   = val.m_red;
        m_green = val.m_green;
        m_blue  = val.m_blue;
        m_argb  = val.m_argb;

        return *this;
    }

	inline Color& operator&=(argb_t val)
    {
        setARGB(getARGB() & val);
        return *this;
    }

	inline Color& operator&=(const Color& val)
    {
        setARGB(getARGB() & val.getARGB());
        return *this;
    }

	inline Color& operator|=(argb_t val)
    {
        setARGB(getARGB() | val);
        return *this;
    }

	inline Color& operator|=(const Color& val)
    {
        setARGB(getARGB() | val.getARGB());
        return *this;
    }

	inline Color& operator<<=(int val)
    {
        setARGB(getARGB() << val);
        return *this;
    }

	inline Color& operator>>=(int val)
    {
        setARGB(getARGB() >> val);
        return *this;
    }

	inline Color operator+(const Color& val) const
    {
        return Color(
            m_red   + val.m_red, 
            m_green + val.m_green, 
            m_blue  + val.m_blue,
            m_alpha + val.m_alpha
        );
    }

	inline Color operator-(const Color& val) const
    {
        return Color(
            m_red   - val.m_red,
            m_green - val.m_green,
            m_blue  - val.m_blue,
            m_alpha - val.m_alpha
        );
    }

	inline Color operator*(const float val) const
    {       
        return Color(
            m_red   * val, 
            m_green * val, 
            m_blue  * val,
            m_alpha * val 
        );  
    }

    inline Color& operator*=(const Color& val)
    {
        m_red *= val.m_red;
        m_blue *= val.m_blue;
        m_green *= val.m_green;
        m_alpha *= val.m_alpha;

		calculateARGB();

        return *this;
    }

	/*************************************************************************
		Compare operators
	*************************************************************************/
	inline bool operator==(const Color& rhs) const
    {
        return m_argb  == rhs.m_argb;
    }

	inline bool operator!=(const Color& rhs) const
    {
        return !(*this == rhs);
    }

	//
	// Conversion operators
	//
	operator argb_t() const		{return getARGB();}

private:
	/*************************************************************************
		Implementation Methods
	*************************************************************************/
	/*!
	\brief
		calculate and return the ARGB value based on the current Color component values.
	*/
	void calculateARGB() const;

	/*************************************************************************
		Implementation Data
	*************************************************************************/
	float m_alpha, m_red, m_green, m_blue;		//!< Color components.
	mutable argb_t m_argb;						//!< Color as ARGB value.
};

__inline void Color::calculateARGB() const
{
	m_argb = (
#ifdef __BIG_ENDIAN__
	
		static_cast<unsigned char>(m_blue * 255) << 24 |
		static_cast<unsigned char>(m_green * 255) << 16 |
		static_cast<unsigned char>(m_red * 255) << 8 |
		static_cast<unsigned char>(m_alpha * 255)
		
#else
		static_cast<unsigned char>(m_alpha * 255) << 24 |
		static_cast<unsigned char>(m_red * 255) << 16 |
		static_cast<unsigned char>(m_green * 255) << 8 |
		static_cast<unsigned char>(m_blue * 255)
#endif
		);
}


}