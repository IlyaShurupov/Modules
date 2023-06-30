
#include "MathCommon.hpp"

#include "ContainersCommon.hpp"

static tp::ModuleManifest* sModuleDependencies[] = {
	&tp::gModuleContainers,
	nullptr
};

tp::ModuleManifest tp::gModuleMath = ModuleManifest("Math", nullptr, nullptr, sModuleDependencies);

tp::alnf std_sin(tp::alnf radians);
tp::alnf std_tan(tp::alnf radians);
tp::alnf std_cos(tp::alnf radians);
tp::alnf std_acos(tp::alnf val);
tp::alnf std_sqrt(tp::alnf val);
tp::alnf std_rad(tp::alnf val);
tp::alnf std_deg(tp::alnf val);
tp::alnf std_atan2(tp::alnf X, tp::alnf Y);
tp::alnf std_atan(tp::alnf val);

tp::alnf tp::sin(const tp::alnf radians) { return std_sin((halnf) radians); }
tp::alnf tp::tan(const tp::alnf radians) { return std_tan((halnf) radians); }
tp::alnf tp::cos(const tp::alnf radians) { return std_cos((halnf) radians); }
tp::alnf tp::acos(const tp::alnf val) { return std_acos((halnf) val); }
tp::alnf tp::sqrt(const tp::alnf val) { return std_sqrt((halnf) val); }
tp::alnf tp::rad(const tp::alnf val) { return val * (PI / 180.f); }
tp::alnf tp::deg(const tp::alnf val) { return val * (180.f / PI); }
tp::alnf tp::atan2(const tp::alnf X, const tp::alnf Y) { return std_atan2((halnf) X, (halnf) Y); }
tp::alnf tp::atan(const tp::alnf val) { return std_atan((halnf) val); }

#include <cmath>

tp::alnf std_sin(const tp::alnf radians) { return sinf((tp::halnf)radians); }
tp::alnf std_tan(const tp::alnf radians) { return tanf((tp::halnf)radians); }
tp::alnf std_cos(const tp::alnf radians) { return cosf((tp::halnf)radians); }
tp::alnf std_acos(const tp::alnf val) { return acos((tp::halnf)val); }
tp::alnf std_sqrt(const tp::alnf val) { return sqrt((tp::halnf)val); }
tp::alnf std_rad(const tp::alnf val) { return val * (PI / 180.f); }
tp::alnf std_deg(const tp::alnf val) { return val * (180.f / PI); }
tp::alnf std_atan2(const tp::alnf X, const tp::alnf Y) { return atan2((tp::halnf)X, (tp::halnf)Y); }
tp::alnf std_atan(const tp::alnf val) { return atan((tp::halnf)val); }