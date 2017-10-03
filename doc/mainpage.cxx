/**

\if LOCAL_DOC
\mainpage LidarTools Documentation
\anchor LidarTools_Main
\endif

\ifnot LOCAL_DOC
\defgroup LidarTools_Main lidartools - LidarTools documentation
\brief Lidar data analysis tools
\ingroup 
\endif

LidarTools is the module used to reduce and analyse Lidar data.\n
A number of classes, examples and test scripts are available:\n

\b Classes
\li
\li LidarTools::LidarFile handles Lidar data I/O
\li LidarTools::LidarFileSet
\li
\li LidarTools::Analyser reduces and analyse Lidar data
\li LidarTools::Overlap reads the Lidar geometrical overlap function from a text file
\li LidarTools::ConfigHandler handles the data analysis configuration
\li LidarTools::Plotter plots data analysis results
\li LidarTools::LidarProcessor is a proxy to run a full analysis of a Lidar run
\li
\li LidarTools::RayleighScattering estimates the Rayleigh scattering for given conditions
\li LidarTools::AtmoAbsorption
\li LidarTools::AtmoPlotter
\li LidarTools::AtmoProfile
\li
\li LidarTools::GlidingAveFilter
\li LidarTools::SavGolFilter

\b Scripts
\li test_LidarFile.C uses the Analyser to open a file and run a data analysis
\li test_Processor.C runs a full data reduction and analysis, and display results on a ROOT canvas
\li test_AtmoPlotter.C
\li test_AtmoProfile.C
\li test_ConfigHandler.C
\li test_GlidingFilter.C
\li test_LidarFileSet.C
\li test_Overlap.C
\li test_Plotter.C
\li test_Rayleigh.C
\li test_SavGolFilter.C

*/
