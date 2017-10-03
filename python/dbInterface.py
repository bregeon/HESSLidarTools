
# Code stolen from Luca.Baldini@pi.infn.it

import MySQLdb
import time
import os
import pickle
from copy import copy

# All Columns "RunQuality"
# | Entry | RunNumber | Telescope | TriggerRate | CorrectedTriggerRate
# | DaqDeadTime | DeadTime    | LiveTimeFraction 
# | MedianNSB | NSBRelativeVariation | NSBRelativeDispersion
# | CTRadiometer_MeanTemp | CTRadiometer_TempRelativeVariation | CTRadiometer_TempRelativeDispersion
# | MedianScaler | ScalerRelativeVariation | ScalerRelativeDispersion
# | MedianTriggerRate | TriggerRateVariation | TriggerRateDispersion
# | Tracking_ZenithMin | Tracking_ZenithMax | Tracking_ZenithMean | Tracking_AzimuthMin | Tracking_AzimuthMax | Tracking_AzimuthMean
# | Tracking_RaOffset | Tracking_DecOffset | Tracking_RaOffset_RMS | Tracking_DecOffset_RMS
# | BrokenPixelsNumber | HiBrokenPixelsNumber | LoBrokenPixelsNumber | MaxBrokenPixelsNumber | MaxHiBrokenPixelsNumber | MaxLoBrokenPixelsNumber
# | EventCount | HVCount | ScalerCount | SectorCount | ZeroTimeEventCount | BadTimeEventCount | EmptyEventCount | MissingEventCount | TelOrphanedEventCount | OutOfTimeEventCount | UnlockGPSEventCount
# | GhostsFraction | PixelThreshold | SectorThreshold | CameraTemperature
# | MeanCentralGPSTimeOffset | RMSCentralGPSTimeOffset | AvgMissingDrawerCount | MaxMissingDrawerCount | LargeMissingDrawerEventCount
# | Focus_Min | Focus_Max | Focus_Mean | Focus_Error_Mean | Focus_Error_RMS
# | Thermal_Expansion_Mean | Thermal_Expansion_RMS |
# SELECT RunNumber,Telescope,TriggerRate, CorrectedTriggerRate FROM RunQuality WHERE RunNumber>=67217 AND RunNumber<67230;
# Select RunNumber,Telescope,TriggerRate, CorrectedTriggerRate, MedianNSB, NSBRelativeVariation, NSBRelativeDispersion, CTRadiometer_MeanTemp, CTRadiometer_TempRelativeVariation, CTRadiometer_TempRelativeDispersion, MedianScaler, ScalerRelativeVariation, ScalerRelativeDispersion,  MedianTriggerRate, TriggerRateVariation, TriggerRateDispersion, Tracking_ZenithMean from RunQuality WHERE RunNumber=70605 AND Telescope=1 ;

# All Columns RunQuality_Set
# | RunNumber | RunDate | RunDuration
# | GlobalEventCount | EventBeforeRunCount | OrphanedEventCount
# | GlobalBunchCount | MissingBunchCount | BackBunchCount | BackEventCount | BunchBeforeRunCount
# | FirstBunchNumber | LastBunchNumber | FirstBunchTimeOffset
# | TelsInRun
# | Target | Nominal_Target_Ra | Nominal_Target_Dec | Nominal_Target_GLong | Nominal_Target_GLat
# | Nominal_WoobleOffset_Ra | Nominal_WoobleOffset_Dec
# | Meteo_MeanBackScatterSum | Meteo_RMSBackScatterSum | Meteo_MeanBackScatterSum1km | Meteo_RMSBackScatterSum1km
# | Meteo_MeanWindSpeed | Meteo_MeanWindDirection | Meteo_MeanRelativeHumidity | Meteo_RMSRelativeHumidity
# | Meteo_MeanPressure | Meteo_RMSPressure | Meteo_MeanTemperature | Meteo_RMSTemperature
# | Lidar_Opacity_355nm | Lidar_Opacity_532nm
# | 2FoldLiveFraction | 3FoldLiveFraction | 4FoldLiveFraction
# | MeanSystemRate | SystemRateVariation | SystemRateDispersion
# | ZenithEqMeanSystemRate | ZenithEqSystemRateVariation | ZenithEqSystemRateDispersion
# | Comments | CodeVersion    |


class dbInterface:
    """ Light interface to the calibration meta-database.
    """
    
    HOST_NAME = 'ccmyhess.in2p3.fr'
    HOST_PORT = 3316
    DB_NAME = 'ANALYSIS'
    
    QUALITYSET_LIST_OF_COLUMNS=[ 'RunNumber','RunDate','RunDuration','TelsInRun','Target',\
              'Meteo_MeanTemperature','Meteo_MeanWindSpeed','Meteo_MeanWindDirection','Meteo_MeanRelativeHumidity',\
              'Meteo_MeanPressure','Lidar_Opacity_355nm','Lidar_Opacity_532nm',\
              'MeanSystemRate','ZenithEqMeanSystemRate','ZenithEqMeanSystemRate/MeanSystemRate']

    QUALITYSET_COLS_TO_DUMP=[ 'RunNumber', 'Meteo_MeanTemperature','Meteo_MeanWindSpeed','Meteo_MeanWindDirection','Meteo_MeanRelativeHumidity',\
              'Meteo_MeanPressure','Lidar_Opacity_355nm','Lidar_Opacity_532nm',\
              'MeanSystemRate','ZenithEqMeanSystemRate','ZenithEqMeanSystemRate/MeanSystemRate']

    QUALITY_LIST_OF_COLUMNS =['RunNumber','Telescope','TriggerRate', 'CorrectedTriggerRate',\
              'MedianNSB', 'NSBRelativeVariation', 'NSBRelativeDispersion',\
              'CTRadiometer_MeanTemp', 'CTRadiometer_TempRelativeVariation', 'CTRadiometer_TempRelativeDispersion',\
              'MedianScaler', 'ScalerRelativeVariation', 'ScalerRelativeDispersion',\
              'MedianTriggerRate', 'TriggerRateVariation', 'TriggerRateDispersion',\
              'Tracking_ZenithMean']

              
    def __init__(self, username, password):
        """ Constructor.
        """
        self.DataBase = MySQLdb.connect(host = self.HOST_NAME,
                                        db = self.DB_NAME,
                                        user = username,
                                        passwd = password,
                                        port = self.HOST_PORT)
                                        
    def setupQuery(self, table="RunQuality_Set", runmin=67217, runmax=67229, columnList=QUALITYSET_LIST_OF_COLUMNS):
        """ Setup the default query parameters
        """
        self.Table=table
        self.RunMin=runmin
        self.RunMax=runmax
        self.ColumnList=columnList


    def query(self, text):
        """ Run a generic query.

        MySQLdb is not very well documented, but here's a few snippets.
        
        The first thing you should know is, fetch_row() takes some additional
        parameters. The first one is, how many rows (maxrows) should be
        returned. By default, it returns one row. It may return fewer rows
        than you asked for, but never more. If you set maxrows=0, it returns
        all rows of the result set. If you ever get an empty tuple back, you
        ran out of rows.

        The second parameter (how) tells it how the row should be represented.
        By default, it is zero which means, return as a tuple. how=1 means,
        return it as a dictionary, where the keys are the column names, or
        table.column if there are two columns with the same name (say, from a
        join). how=2 means the same as how=1 except that the keys are always
        table.column; this is for compatibility with the old Mysqldb module.
        """
        if not text.endswith(';'):
            text = '%s;' % text
        print 'About to run query "%s"...' % text
        self.DataBase.query(text)
        result = self.DataBase.store_result()
        print 'Done.'
        return result.fetch_row(0, 1)

    def queryRunsMinMax(self):
        """ Query all runs from RunMin to RunMax for the default set of parameters
        
        DB table is the first parameter : RunQuality_Set or RunQuality
        Columns required are passed through a list of strings
        Returns a tuple of dictionnaries (one per run)
        """
        text = 'SELECT '
        for var in self.ColumnList[:-1]:
            text+='%s, '%var
        text+= self.ColumnList[-1]
        text += ' from %s'%self.Table
        text += ' WHERE RunNumber>= %d' % self.RunMin
        text += ' AND RunNumber <= %d' % self.RunMax
        text += ' ORDER BY RunNumber'
        print(text) 
        return self.query(text)


    def queryToPickle(self, outputFilePath = None):
        """ Dump standard query to a pickle file
        """
        timestamp = time.asctime()
        results = self.queryRunsMinMax()
        if outputFilePath is None:            
            outputFileName = 'calibdb_%s.pickle' % (timestamp.replace(' ', '_'))
            outputFilePath = os.path.join('dumps', outputFileName)
        print 'Writing query to %s...' % outputFilePath
        outputFile = file(outputFilePath, 'w')
        pickle.dump(timestamp, outputFile)
        pickle.dump(results, outputFile)
        outputFile.close()
        print 'Done.'
        return results

    def queryToTxt(self, colsToDump=QUALITYSET_COLS_TO_DUMP, outputFilePath='dump.txt'):
        results = self.queryRunsMinMax()
        txt='#'
        for col in colsToDump[:-1]:
            txt+='%s\t'%col
        txt+='%s\n'%colsToDump[-1]
        for one in results:
            txt+='%s\t'%one[colsToDump[0]]
            for col in colsToDump[1:-1]:
                try:
                    txt+='%.3f\t'%one[col]
                except:
                    txt+='0.\t'
            try:
                txt+='%.3f\n'%one[colsToDump[-1]]
            except:
                txt+='0.\n'
        print txt
        open(outputFilePath,'w').writelines(txt)
        return txt
        
    
if __name__ == '__main__':
    from dbInterface import *

    from optparse import OptionParser

    parser = OptionParser(usage = 'usage: %prog [options] -u user -p passwd')
    parser.add_option('-u', '--user', dest = 'u', default = None, type = str,
                  help = 'the username for the db connection')
    parser.add_option('-p', '--passwd', dest = 'p', default = None, type = str,
                  help = 'the password for the db connection')

    (opts, args) = parser.parse_args()

    if opts.u is None or opts.p is None:
        parser.print_help()
        parser.error('Please provide full access credentials.')

#    db = dbInterface(opts.u, opts.p)
#    db.setupQuery(table="RunQuality_Set", runmin=68880, runmax=70605, columnList=db.QUALITYSET_LIST_OF_COLUMNS)
#    db.queryToTxt(colsToDump=db.QUALITYSET_COLS_TO_DUMP, outputFilePath='dumpQualitySet.txt')

#    db2 = dbInterface(opts.u, opts.p)
#    db2.setupQuery(table="RunQuality", runmin=68880, runmax=70605, columnList=db.QUALITY_LIST_OF_COLUMNS)
#    db2.queryToTxt(colsToDump=db.QUALITY_LIST_OF_COLUMNS, outputFilePath='dumpQuality.txt')

    db3 = dbInterface(opts.u, opts.p)
    result=db3.query('select RunNumber,RunDate from RunQuality_Set WHERE RunNumber>=45000 ORDER BY RunNumber;')
    #print result
    for d in result:
      date=str(d['RunDate'])
      print d['RunNumber'], date.replace(' ','_') 
    
