MODULE = LidarTools
LIBVERSION=

SOURCES =  LidarFile LidarFileSet Analyser ConfigHandler Plotter LidarProcessor \
           RayleighScattering Overlap AtmoProfile AtmoAbsorption AtmoPlotter \
           GlidingAveFilter SavGolFilter

INCLUDES = LidarTools sash/Time sash/DataSet sash/HESSArray sashfile/FileHandler\
           atmosphere/LidarEvent

USE_LIBS = rootHSbase rootsmash rootsashfile rootmathutils

LIB = lib/lib${MODULE}${LIBVERSION}.$(DllSuf)
LIBMAP = lib/lib${MODULE}${LIBVERSION}.$(HESSLIBMAP)

ifneq ($(wildcard ${HESSUSER}/sash),)
include ${HESSUSER}/sash/Makefile.sash
else
include ${HESSROOT}/sash/Makefile.sash
endif

${PWD}/${LIB}: ${OBJECTS}
${PWD}/${LIBMAP}: include/LidarToolsLinkDef.hh

# DO NOT DELETE
