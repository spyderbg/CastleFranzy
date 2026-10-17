///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#define ENABLE_PROFILER	(0)

///////////////////////////////////////////////////////////////////////

#include <vector>

///////////////////////////////////////////////////////////////////////

namespace Profiler
{
#if defined(WIN32)
	typedef __int64 i64;
#else
	typedef long long i64;
#endif

	struct Zone {
		i64 self_time;
		i64 hier_time;

		i64 self_start;
		i64 hier_start;

		double self_show;
		double hier_show;
		double count_show;

		double self_variance;
		double hier_variance;
		double count_variance;

		int enter_count;
		int recursion_count;

		const char* name;

		Zone (const char* name);
		void Enter (Zone* parent);
		void Exit (Zone* parent);
	};

	struct ZoneScope {
		ZoneScope (Zone* zone);
		~ZoneScope ();
        
        void Release ();

		Zone* m_zone;
		ZoneScope* m_parent;
	};

	struct Info {
		const char* name;
		double self;
		double hier;
		double count;

		double self_var;
		double hier_var;
		double count_var;
	};
	extern std::vector<Info> zone_infos;

	void GatherInfo ();
	void DrawInfo ();

	void Update_dt ();
}

#if ENABLE_PROFILER

	#define PROFILER_DEFINE_ZONE(zone_name) \
		::Profiler::Zone profiler_zone_##zone_name(#zone_name)

	#define PROFILER_SCOPE(zone_name) \
		static ::Profiler::Zone profiler_zone_##zone_name(#zone_name); \
		::Profiler::ZoneScope profiler_scope_##zone_name(&profiler_zone_##zone_name )

	#define PROFILER_DEFINED_SCOPE(zone_name) \
		::Profiler::ZoneScope profiler_scope_##zone_name(&profiler_zone_##zone_name )

	#define PROFILER_END(zone_name) \
		profiler_scope_##zone_name.Release()

	#define PROFILER_DRAW_INFO()	Profiler::DrawInfo()
	#define PROFILER_GATHER_INFO()	Profiler::GatherInfo()

#else

	#define PROFILER_DEFINE_ZONE(zone_name)

	#define PROFILER_SCOPE(zone_name)
	#define PROFILER_DEFINED_SCOPE(zone_name)
	#define PROFILER_END(zone_name)

	#define PROFILER_DRAW_INFO()
	#define PROFILER_GATHER_INFO()

#endif

