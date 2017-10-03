#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;
#pragma link C++ namespace LidarTools;

#pragma link C++ class LidarTools::LidarFile+;
#pragma link C++ class LidarTools::LidarFileSet+;
#pragma link C++ class LidarTools::Analyser+;
#pragma link C++ class LidarTools::ConfigHandler+;
#pragma link C++ class LidarTools::Plotter+;
#pragma link C++ class LidarTools::LidarProcessor+;
#pragma link C++ class LidarTools::RayleighScattering+;
#pragma link C++ class LidarTools::Overlap+;
#pragma link C++ class LidarTools::AtmoProfile+;
#pragma link C++ class LidarTools::AtmoAbsorption+;
#pragma link C++ class LidarTools::AtmoPlotter+;
#pragma link C++ class LidarTools::GlidingAveFilter+;
#pragma link C++ class LidarTools::SavGolFilter+;

#pragma link C++ class map<string,string>;
#pragma link C++ class pair<string,string>;

#ifdef ROOT_HAVE_TEMPLATE_MEMBER

#else

#endif

#endif
