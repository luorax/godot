/*************************************************************************/
/*  color.cpp                                                            */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2018 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2018 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "color.h"

#include "color_names.inc"
#include "map.h"
#include "math_funcs.h"
#include "print_string.h"

uint32_t Color::to_argb32() const {

	uint32_t c = (uint8_t)(a * 255);
	c <<= 8;
	c |= (uint8_t)(r * 255);
	c <<= 8;
	c |= (uint8_t)(g * 255);
	c <<= 8;
	c |= (uint8_t)(b * 255);

	return c;
}

uint32_t Color::to_abgr32() const {
	uint32_t c = (uint8_t)(a * 255);
	c <<= 8;
	c |= (uint8_t)(b * 255);
	c <<= 8;
	c |= (uint8_t)(g * 255);
	c <<= 8;
	c |= (uint8_t)(r * 255);

	return c;
}

uint32_t Color::to_rgba32() const {

	uint32_t c = (uint8_t)(r * 255);
	c <<= 8;
	c |= (uint8_t)(g * 255);
	c <<= 8;
	c |= (uint8_t)(b * 255);
	c <<= 8;
	c |= (uint8_t)(a * 255);

	return c;
}

float Color::get_h() const {

	float min = MIN(r, g);
	min = MIN(min, b);
	float max = MAX(r, g);
	max = MAX(max, b);

	float delta = max - min;

	if (delta == 0)
		return 0;

	float h;
	if (r == max)
		h = (g - b) / delta; // between yellow & magenta
	else if (g == max)
		h = 2 + (b - r) / delta; // between cyan & yellow
	else
		h = 4 + (r - g) / delta; // between magenta & cyan

	h /= 6.0;
	if (h < 0)
		h += 1.0;

	return h;
}

float Color::get_s() const {

	float min = MIN(r, g);
	min = MIN(min, b);
	float max = MAX(r, g);
	max = MAX(max, b);

	float delta = max - min;

	return (max != 0) ? (delta / max) : 0;
}

float Color::get_v() const {

	float max = MAX(r, g);
	max = MAX(max, b);
	return max;
}

void Color::set_hsv(float p_h, float p_s, float p_v, float p_alpha) {

	int i;
	float f, p, q, t;
	a = p_alpha;

	if (p_s == 0) {
		// acp_hromatic (grey)
		r = g = b = p_v;
		return;
	}

	p_h *= 6.0;
	p_h = Math::fmod(p_h, 6);
	i = Math::floor(p_h);

	f = p_h - i;
	p = p_v * (1 - p_s);
	q = p_v * (1 - p_s * f);
	t = p_v * (1 - p_s * (1 - f));

	switch (i) {
		case 0: // Red is the dominant color
			r = p_v;
			g = t;
			b = p;
			break;
		case 1: // Green is the dominant color
			r = q;
			g = p_v;
			b = p;
			break;
		case 2:
			r = p;
			g = p_v;
			b = t;
			break;
		case 3: // Blue is the dominant color
			r = p;
			g = q;
			b = p_v;
			break;
		case 4:
			r = t;
			g = p;
			b = p_v;
			break;
		default: // (5) Red is the dominant color
			r = p_v;
			g = p;
			b = q;
			break;
	}
}

void Color::invert() {

	r = 1.0 - r;
	g = 1.0 - g;
	b = 1.0 - b;
}
void Color::contrast() {

	r = Math::fmod(r + 0.5, 1.0);
	g = Math::fmod(g + 0.5, 1.0);
	b = Math::fmod(b + 0.5, 1.0);
}

Color Color::hex(uint32_t p_hex) {

	float a = (p_hex & 0xFF) / 255.0;
	p_hex >>= 8;
	float b = (p_hex & 0xFF) / 255.0;
	p_hex >>= 8;
	float g = (p_hex & 0xFF) / 255.0;
	p_hex >>= 8;
	float r = (p_hex & 0xFF) / 255.0;

	return Color(r, g, b, a);
}

static float _parse_col(const String &p_str, int p_ofs) {

	int ig = 0;

	for (int i = 0; i < 2; i++) {

		int c = p_str[i + p_ofs];
		int v = 0;

		if (c >= '0' && c <= '9') {
			v = c - '0';
		} else if (c >= 'a' && c <= 'f') {
			v = c - 'a';
			v += 10;
		} else if (c >= 'A' && c <= 'F') {
			v = c - 'A';
			v += 10;
		} else {
			return -1;
		}

		if (i == 0)
			ig += v * 16;
		else
			ig += v;
	}

	return ig;
}

Color Color::inverted() const {

	Color c = *this;
	c.invert();
	return c;
}

Color Color::contrasted() const {

	Color c = *this;
	c.contrast();
	return c;
}

Color Color::html(const String &p_color) {

	String color = p_color;
	if (color.length() == 0)
		return Color();
	if (color[0] == '#')
		color = color.substr(1, color.length() - 1);
	if (color.length() == 3 || color.length() == 4) {
		String exp_color;
		for (int i = 0; i < color.length(); i++) {
			exp_color += color[i];
			exp_color += color[i];
		}
		color = exp_color;
	}

	bool alpha = false;

	if (color.length() == 8) {
		alpha = true;
	} else if (color.length() == 6) {
		alpha = false;
	} else {
		ERR_EXPLAIN("Invalid Color Code: " + p_color);
		ERR_FAIL_V(Color());
	}

	int a = 255;
	if (alpha) {
		a = _parse_col(color, 0);
		if (a < 0) {
			ERR_EXPLAIN("Invalid Color Code: " + p_color);
			ERR_FAIL_V(Color());
		}
	}

	int from = alpha ? 2 : 0;

	int r = _parse_col(color, from + 0);
	if (r < 0) {
		ERR_EXPLAIN("Invalid Color Code: " + p_color);
		ERR_FAIL_V(Color());
	}
	int g = _parse_col(color, from + 2);
	if (g < 0) {
		ERR_EXPLAIN("Invalid Color Code: " + p_color);
		ERR_FAIL_V(Color());
	}
	int b = _parse_col(color, from + 4);
	if (b < 0) {
		ERR_EXPLAIN("Invalid Color Code: " + p_color);
		ERR_FAIL_V(Color());
	}

	return Color(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
}

bool Color::html_is_valid(const String &p_color) {

	String color = p_color;

	if (color.length() == 0)
		return false;
	if (color[0] == '#')
		color = color.substr(1, color.length() - 1);

	bool alpha = false;

	if (color.length() == 8) {
		alpha = true;
	} else if (color.length() == 6) {
		alpha = false;
	} else {
		return false;
	}

	int a = 255;
	if (alpha) {
		a = _parse_col(color, 0);
		if (a < 0) {
			return false;
		}
	}

	int from = alpha ? 2 : 0;

	int r = _parse_col(color, from + 0);
	if (r < 0) {
		return false;
	}
	int g = _parse_col(color, from + 2);
	if (g < 0) {
		return false;
	}
	int b = _parse_col(color, from + 4);
	if (b < 0) {
		return false;
	}

	return true;
}

Color Color::named(const String &p_name) {
	if (_named_colors.empty()) _populate_named_colors(); // from color_names.inc
	String name = p_name;
	// Normalize name
	name = name.replace(" ", "");
	name = name.replace("-", "");
	name = name.replace("_", "");
	name = name.replace("'", "");
	name = name.replace(".", "");
	name = name.to_lower();

	const Map<String, Color>::Element *color = _named_colors.find(name);
	if (color) {
		return color->value();
	} else {
		ERR_EXPLAIN("Invalid Color Name: " + p_name);
		ERR_FAIL_V(Color());
	}
}

String _to_hex(float p_val) {

	int v = p_val * 255;
	v = CLAMP(v, 0, 255);
	String ret;

	for (int i = 0; i < 2; i++) {

		CharType c[2] = { 0, 0 };
		int lv = v & 0xF;
		if (lv < 10)
			c[0] = '0' + lv;
		else
			c[0] = 'a' + lv - 10;

		v >>= 4;
		String cs = (const CharType *)c;
		ret = cs + ret;
	}

	return ret;
}

String Color::to_html(bool p_alpha) const {

	String txt;
	txt += _to_hex(r);
	txt += _to_hex(g);
	txt += _to_hex(b);
	if (p_alpha)
		txt = _to_hex(a) + txt;
	return txt;
}

Color Color::from_hsv(float p_h, float p_s, float p_v, float p_a) {

	p_h = Math::fmod(p_h * 360.0f, 360.0f);
	if (p_h < 0.0)
		p_h += 360.0f;

	const float h_ = p_h / 60.0f;
	const float c = p_v * p_s;
	const float x = c * (1.0f - Math::abs(Math::fmod(h_, 2.0f) - 1.0f));
	float r, g, b;

	switch ((int)h_) {
		case 0: {
			r = c;
			g = x;
			b = 0;
		} break;
		case 1: {
			r = x;
			g = c;
			b = 0;
		} break;
		case 2: {
			r = 0;
			g = c;
			b = x;
		} break;
		case 3: {
			r = 0;
			g = x;
			b = c;
		} break;
		case 4: {
			r = x;
			g = 0;
			b = c;
		} break;
		case 5: {
			r = c;
			g = 0;
			b = x;
		} break;
		default: {
			r = 0;
			g = 0;
			b = 0;
		} break;
	}

	const float m = p_v - c;
	return Color(m + r, m + g, m + b, p_a);
}

float Color::gray() const {

	return (r + g + b) / 3.0;
}

Color::operator String() const {

	return rtos(r) + ", " + rtos(g) + ", " + rtos(b) + ", " + rtos(a);
}

Color Color::operator+(const Color &p_color) const {

	return Color(
			r + p_color.r,
			g + p_color.g,
			b + p_color.b,
			a + p_color.a);
}

void Color::operator+=(const Color &p_color) {

	r = r + p_color.r;
	g = g + p_color.g;
	b = b + p_color.b;
	a = a + p_color.a;
}

Color Color::operator-(const Color &p_color) const {

	return Color(
			r - p_color.r,
			g - p_color.g,
			b - p_color.b,
			a - p_color.a);
}

void Color::operator-=(const Color &p_color) {

	r = r - p_color.r;
	g = g - p_color.g;
	b = b - p_color.b;
	a = a - p_color.a;
}

Color Color::operator*(const Color &p_color) const {

	return Color(
			r * p_color.r,
			g * p_color.g,
			b * p_color.b,
			a * p_color.a);
}

Color Color::operator*(const real_t &rvalue) const {

	return Color(
			r * rvalue,
			g * rvalue,
			b * rvalue,
			a * rvalue);
}

void Color::operator*=(const Color &p_color) {

	r = r * p_color.r;
	g = g * p_color.g;
	b = b * p_color.b;
	a = a * p_color.a;
}

void Color::operator*=(const real_t &rvalue) {

	r = r * rvalue;
	g = g * rvalue;
	b = b * rvalue;
	a = a * rvalue;
}

Color Color::operator/(const Color &p_color) const {

	return Color(
			r / p_color.r,
			g / p_color.g,
			b / p_color.b,
			a / p_color.a);
}

Color Color::operator/(const real_t &rvalue) const {

	return Color(
			r / rvalue,
			g / rvalue,
			b / rvalue,
			a / rvalue);
}

void Color::operator/=(const Color &p_color) {

	r = r / p_color.r;
	g = g / p_color.g;
	b = b / p_color.b;
	a = a / p_color.a;
}

void Color::operator/=(const real_t &rvalue) {

	if (rvalue == 0) {
		r = 1.0;
		g = 1.0;
		b = 1.0;
		a = 1.0;
	} else {
		r = r / rvalue;
		g = g / rvalue;
		b = b / rvalue;
		a = a / rvalue;
	}
};

Color Color::operator-() const {

	return Color(
			1.0 - r,
			1.0 - g,
			1.0 - b,
			1.0 - a);
}
