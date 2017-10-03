#!/bin/env python

from dbInterface import dbInterface
from copy import copy


class MergedQuery:
    ''' Merge info from different database tables and TC text file
    '''
    
    MERGE_COLS_TO_DUMP=['RunNumber', 'RunDate', 'Meteo_MeanTemperature','Meteo_MeanWindSpeed','Meteo_MeanWindDirection','Meteo_MeanRelativeHumidity',\
              'Meteo_MeanPressure','Lidar_Opacity_355nm','Lidar_Opacity_532nm', 'TC','Crab',\
              'MeanSystemRate','ZenithEqMeanSystemRate','ZenithEqMeanSystemRate/MeanSystemRate',\
              'Telescope','TriggerRate', 'CorrectedTriggerRate',\
              'MedianNSB', 'NSBRelativeVariation', 'NSBRelativeDispersion',\
              'CTRadiometer_MeanTemp', 'CTRadiometer_TempRelativeVariation', 'CTRadiometer_TempRelativeDispersion',\
              'MedianScaler', 'ScalerRelativeVariation', 'ScalerRelativeDispersion',\
              'MedianTriggerRate', 'TriggerRateVariation', 'TriggerRateDispersion',\
              'Tracking_ZenithMean']
              
    def __init__(self, username, password, RunMin, RunMax, outFilePath):
        self.RunMin=RunMin
        self.RunMax=RunMax
        self.OutFile=outFilePath
        self.UserName=username
        self.Password=password
        self.TCDict=None
        self.CrabDict=None
        self.JunkDict={}
    
    def query(self):
        db = dbInterface(self.UserName, self.Password)
        db.setupQuery(table="RunQuality_Set", runmin=self.RunMin, runmax=self.RunMax, columnList=db.QUALITYSET_LIST_OF_COLUMNS)
        QualitySet=db.queryRunsMinMax()

        db2 = dbInterface(self.UserName, self.Password)
        db2.setupQuery(table="RunQuality", runmin=self.RunMin, runmax=self.RunMax, columnList=db.QUALITY_LIST_OF_COLUMNS)
        Quality=db2.queryRunsMinMax()
        
        self.ResultsDict={}
        qDict={}
        for runDict in QualitySet:
            RunNumber=runDict['RunNumber']
            for one in Quality:
                if one['RunNumber']==RunNumber and one['Telescope']==1:
                    qDict=one
                    break
            fullDict=copy(runDict)
            fullDict.update(qDict)
            self.ResultsDict[RunNumber]=fullDict
            
        return self.ResultsDict

    def readTC(self, inputFile):
        tcDict={}
        all=open(inputFile,'r').readlines()
        for line in all:
            runnumber=int(line.split()[0])
            tc=float(line.split()[-1])
            tcDict[runnumber]=tc
        self.TCDict=tcDict
        return self.TCDict
        
    def addTC(self, inputFile):
        if self.TCDict is None:
            self.readTC(inputFile)
        for run in self.ResultsDict.keys():
            if self.TCDict.has_key(run):
                self.ResultsDict[run]['TC']=self.TCDict[run]
            else:
                self.ResultsDict[run]['TC']=0.
        return self.ResultsDict

    def readCrab(self, inputFile):
        # Use a dict just for symmetry
        crabDict={}
        all=open(inputFile,'r').readlines()
        for line in all:
            if line.find('#')<0:
                runnumber=int(line.split()[0])                
                crabDict[runnumber]=1
        self.CrabDict=crabDict
        return self.CrabDict

    def addCrab(self, inputFile):
        if self.CrabDict is None:
            self.readCrab(inputFile)
        for run in self.ResultsDict.keys():
            if self.CrabDict.has_key(run):
                self.ResultsDict[run]['Crab']=self.CrabDict[run]
            else:
                self.ResultsDict[run]['Crab']=0
        return self.ResultsDict

    def cleanUp(self, crabOnly=False):
        for run,results in self.ResultsDict.items():
            lidar355=results['Lidar_Opacity_355nm']
            lidar532=results['Lidar_Opacity_532nm']
            tc=results['TC']
            crab=results['Crab']
            if lidar355 is None or lidar532 is None or tc is None:
                self.ResultsDict.pop(run)
                self.JunkDict[run]=results
            # either no Lidar data, or bad lidar data (mis-agligned Lidar/dish, should be cut on raw signal)
            elif lidar355*lidar532*tc == 0 or lidar532<0.03 or lidar355<0.15:
                self.ResultsDict.pop(run)
                self.JunkDict[run]=results
            elif crabOnly and crab==0:
                self.ResultsDict.pop(run)
                self.JunkDict[run]=results
                

    def zeroLidar(self):
        for run,results in self.ResultsDict.items():
            lidar355=results['Lidar_Opacity_355nm']
            lidar532=results['Lidar_Opacity_532nm']
            if lidar355 is None:
                results['Lidar_Opacity_355nm']=0.
            if lidar532 is None:
                results['Lidar_Opacity_532nm']=0.
                
        
    def dumpResults(self, outputFilePath='fulldump.txt'):
        txt='#'
        for col in self.MERGE_COLS_TO_DUMP[:-1]:
            txt+='%s\t'%col
        txt+='%s\n'%self.MERGE_COLS_TO_DUMP[-1]
       
        for run,results in self.ResultsDict.items():            
            txt+='%s\t'%run
            date=str(results[self.MERGE_COLS_TO_DUMP[1]])
            date=date.replace(' ','_')
            txt+='%s\t'%date
            for col in self.MERGE_COLS_TO_DUMP[2:-1]:
                try:                    
                    txt+='%.5f\t'%results[col]
                except:
                    txt+='0.\t'
            try:
                txt+='%.5f\n'%results[self.MERGE_COLS_TO_DUMP[-1]]
            except:
                txt+='0.\n'

        print txt
        open(outputFilePath,'w').writelines(txt)
        return txt

    
            
if __name__ == '__main__':

    from MergedQuery import *
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

#    mq = MergedQuery(opts.u, opts.p, 68880, 70605, 'pipo.txt')
#    mq = MergedQuery(opts.u, opts.p, 65159, 70605, 'pipo.txt')
    mq = MergedQuery(opts.u, opts.p, 45000, 115000, 'pipo.txt')
    mq.query()
#    mq.addTC('../data/TC_Raquel_Jan2014.txt')
#    mq.addCrab('../data/CrabList_loose_13032014.txt')
#    mq.cleanUp(crabOnly=True)
#    mq.zeroLidar()
#    mq.dumpResults(outputFilePath='all_runs_65159_70605_18022014.txt')
    mq.dumpResults(outputFilePath='LidarMeteo_45000_115000_23022016.txt')
