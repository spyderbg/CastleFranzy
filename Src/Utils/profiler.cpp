
#include "stdafx.h"
#ifndef WIN32
#include <sys/time.h>
#endif

#include "Utils/profiler.h"
#include <cassert>
#include <vector>
#include <stack>
#include <algorithm>
#include <cstdio>
#include <cmath>

#include "Core/irrlicht.h"
#include "Core/Application.h"

const int LEFT = 0;
const int RIGHT = 1;

void DrawSimpleString (int x, int y, int align, int color, const char* str, ...)
{
	Application* app = Application::GetInstance(); if (!app) return;
	IGUIEnvironment* env = app->GetGUIEnv(); if (!env) return;
	IGUISkin* skin = env->getSkin(); if (!skin) return;
	IGUIFont* font = skin->getFont(); if (!font) return;
	
	char buf[1024];
	va_list args;
	va_start(args, str);
	vsprintf(buf, str, args);
	va_end(args);

	wchar_t buf2[1024];
	int i;
	for (i = 0; i < 1024 && buf[i]; ++i) buf2[i] = buf[i];
	buf2[i] = 0;

	dimension2di dim = font->getDimension(buf2);
	if (align == RIGHT) x -= dim.Width;
	dim.Width += 1;
	rect<s32> rc(position2di(x, y), dim);
	app->GetDriver()->draw2DRectangle(SColor(255,0,0,0), rc);
	font->draw(buf2, rc, SColor(color));
}

namespace Profiler
{
	static i64 GetCounter ()
	{
	#if defined(WIN32)
		LARGE_INTEGER li;
		QueryPerformanceCounter( &li );
		return li.QuadPart;
	#else
		struct timeval tp;
		gettimeofday(&tp, 0);
		return (i64)tp.tv_sec * 1000000 + (i64)tp.tv_usec;
	#endif
	}

	static ZoneScope* top = 0;
	static std::vector<Zone*> zones;

	Zone::Zone (const char* name) {
		this->name = name;

		self_time = 0;
		hier_time = 0;
		self_start = 0;
		hier_start = 0;
		enter_count = 0;
		recursion_count = 0;

		self_show = 0.0;
		hier_show = 0.0;
		count_show = 0.0;

		self_variance = 0.0;
		hier_variance = 0.0;
		count_variance = 0.0;

		zones.push_back(this);
	}

	void Zone::Enter (Zone* parent) {
		i64 now = GetCounter();
		if (parent) parent->self_time += now - parent->self_start;
		self_start = now;
		if (recursion_count++ == 0) hier_start = now;
		enter_count++;
	}

	void Zone::Exit (Zone* parent) {
		i64 now = GetCounter();
		self_time += now - self_start;
		if (parent) parent->self_start = now;
		if (--recursion_count == 0) hier_time += now - hier_start;
	}

	ZoneScope::ZoneScope (Zone* zone) {
		m_zone = zone;
		m_parent = top;
		top = this;
		m_zone->Enter(m_parent ? m_parent->m_zone : 0);
	}

	ZoneScope::~ZoneScope () {
        Release();
	}
    
    void ZoneScope::Release () {
        if (m_zone)
        {
            top = m_parent;
            m_zone->Exit(m_parent ? m_parent->m_zone : 0);
            m_zone = 0;
        }
    }

	std::vector<Info> zone_infos;

	struct HierCompare {
		bool operator() (const Info& lhs, const Info& rhs) {
			return lhs.hier > rhs.hier;
		}
	};

	struct SelfCompare {
		bool operator() (const Info& lhs, const Info& rhs) {
			return lhs.self > rhs.self;
		}
	};

	static double time_since_last = 0.0;
	static double time_since_last_variance = 0.0;
	static double dt = 1.0;
	static double time = 0.0;
	static int last_sort = 0;

	template <typename T, typename U>
	void SmoothUpdate (T& val, const U& new_val, double factor) {
		val = factor * val + (1.0 - factor) * new_val;
	}

	double Clamp (double x) {
		if (x > 1.0) return 1.0;
		if (x < 0.0) return 0.0;
		return x;
	}
	
	static double mymax (double x, double y) {
		if (x > y) return x;
		return y;
	}

	double GetTemp (double mean, double variance) {
		double stddev = std::sqrt(mymax(variance - mean * mean, 0.0));
		return Clamp(2.0 * stddev / std::abs(mean));
	}

	int GetColor (double mean, double variance) {
		double t = GetTemp(mean, variance);
		int r = 0xff << 16;
		int g = 0xff << 8;
		int b = 0x00 << 0;
		if (t <= 0.33) r = (int(t * 3 * 255.0) & 0xff) << 16;
		if (t >= 0.67) g = (int((1.0 - t) * 3 * 255.0) & 0xff) << 8;
		return 0xff000000 | r | g | b;
	}

	void GatherInfo () {
		PROFILER_SCOPE(prof_gather);

		const double time_to_reach_90_percent = 0.5; // in seconds
		const double k = std::pow(0.1, 1.0 / time_to_reach_90_percent);
		const double factor = std::pow(k, dt);

		SmoothUpdate(time_since_last, dt, factor);
		SmoothUpdate(time_since_last_variance, dt * dt, factor);

		//zone_infos.clear();
		for (unsigned i = 0; i < zones.size(); ++i) {
			SmoothUpdate(zones[i]->self_show, zones[i]->self_time, factor);
			SmoothUpdate(zones[i]->hier_show, zones[i]->hier_time, factor);
			SmoothUpdate(zones[i]->count_show, zones[i]->enter_count, factor);

			SmoothUpdate(zones[i]->self_variance, zones[i]->self_time * zones[i]->self_time, factor);
			SmoothUpdate(zones[i]->hier_variance, zones[i]->hier_time * zones[i]->hier_time, factor);
			SmoothUpdate(zones[i]->count_variance, zones[i]->enter_count * zones[i]->enter_count, factor);

			Info info = { zones[i]->name, zones[i]->self_show, zones[i]->hier_show, zones[i]->count_show,
				zones[i]->self_variance, zones[i]->hier_variance, zones[i]->count_variance };
			bool found = false;
			for (unsigned j = 0; j < zone_infos.size(); ++j) {
				if (zone_infos[j].name == info.name) {
					zone_infos[j] = info;
					found = true;
					break;
				}
			}
			if (!found) {
				zone_infos.push_back(info);
			}

			zones[i]->self_time = 0;
			zones[i]->hier_time = 0;
			zones[i]->enter_count = 0;
		}

		// only sort once every second
		if (last_sort + 1 < time) {
			last_sort = (int)time;
			std::sort(zone_infos.begin(), zone_infos.end(), HierCompare());
		}

		Update_dt();
		time += dt;
	}

	void DrawInfo () {
		PROFILER_SCOPE(prof_draw);

#if defined(WIN32)
		LARGE_INTEGER freq1;
		QueryPerformanceFrequency(&freq1);
		i64 freq = freq1.QuadPart;
#else
		i64 freq = 1000000;
#endif

		int sx = 10;
		int sy = 150;
		int h = 10;

		int name_x = sx;
		int self_x = sx + 100;
		int hier_x = self_x + 50;
		int count_x = hier_x + 50;

		int y = sy;
		DrawSimpleString(self_x, y, RIGHT, GetColor(time_since_last, time_since_last_variance),
			"%.1f ms", 1000.0 * time_since_last);
		DrawSimpleString(count_x, y, RIGHT, GetColor(time_since_last, time_since_last_variance),
			"%.0f fps", 1.0 / time_since_last);
		y += h;

		DrawSimpleString(name_x, y, LEFT, 0xffffffff, "name");
		DrawSimpleString(self_x, y, RIGHT, 0xffffffff, "self");
		DrawSimpleString(hier_x, y, RIGHT, 0xffffffff, "total");
		DrawSimpleString(count_x, y, RIGHT, 0xffffffff, "count");
		y += h;

		bool percent = true;
		
		for (unsigned i = 0; i < zone_infos.size(); ++i) {
			const Info& info = zone_infos[i];

			DrawSimpleString(name_x, y, LEFT, 0xffffffff, "%s", info.name);
			if (percent) {
				
				DrawSimpleString(self_x, y, RIGHT, GetColor(info.self, info.self_var),
					"%.0f%%", 100.0 * info.self / (freq * time_since_last));
				DrawSimpleString(hier_x, y, RIGHT, GetColor(info.hier, info.hier_var),
					"%.0f%%", 100.0 * info.hier / (freq * time_since_last));
			} else {
				DrawSimpleString(self_x, y, RIGHT, GetColor(info.self, info.self_var),
					"%.1f", 1000.0 * info.self / freq);
				DrawSimpleString(hier_x, y, RIGHT, GetColor(info.hier, info.hier_var),
					"%.1f", 1000.0 * info.hier / freq);
			}
			DrawSimpleString(count_x, y, RIGHT, GetColor(info.count, info.count_var),
				"%.0f", info.count);

			y += h;
		}
	}

	i64 last_frame = GetCounter();

	void Update_dt () {
#if defined(WIN32)
		LARGE_INTEGER freq1;
		QueryPerformanceFrequency(&freq1);
		i64 freq = freq1.QuadPart;
#else
		i64 freq = 1000000;
#endif

		i64 now = GetCounter();
		if (last_frame == 0) {
			dt = 0;
		} else {
			dt = double(now - last_frame) / freq;
		}
		last_frame = now;
	}
}
