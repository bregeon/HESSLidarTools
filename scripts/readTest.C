#include "LidarTools/LidarFile.hh"


void test1()
{
// first test
LidarTools::LidarFile *lf = new LidarTools::LidarFile("/data/Hess/data/LidarData_2009_2010_ROOT/run_060241_Lidar_001.root",true);
int rc = lf->Read();
std::cout<<"Return code "<<rc<<std::endl;
delete lf;
}

void test2()
{
// second test
SashFile::FileHandler *fFileHandler = new SashFile::FileHandler();
fFileHandler->AddFile("/data/Hess/data/LidarData_2009_2010_ROOT/run_060241_Lidar_001.root");
Sash::DataSet* lidar = fFileHandler->Get("Lidar");
lidar->Print();
delete fFileHandler;
}

void test3(int entry)
{
//third test
TFile *lidarf = new TFile("/data/Hess/data/LidarData_2009_2010_ROOT/run_060241_Lidar_001.root");
Sash::DataSet* lidar2 = (Sash::DataSet*)lidarf->Get("Lidar");
  if(!lidar2){
      std::cerr<<"lidar2 is NULL"<<std::endl;
      }
std::cout << "N entries " << lidar2->GetEntries()<<std::endl;
std::cout << "Get entry " << entry<<std::endl;
lidar2->GetEntry(entry);  

Sash::HESSArray *hessarray=lidar2->GetHESSArray();

std::cout << "Get LidarEvent" << std::endl;
const Atmosphere::LidarEvent * event = hessarray->Get<Atmosphere::LidarEvent>();
TArrayF fRange=event->GetRange();

std::cout << "Time Stamp " << event->GetTime() << std::endl;
for(int i=0; i<10000; i+=100)
  std::cout << "Range " << fRange[i] << std::endl;
lidarf->Close();
}

void readTest()
{
//test1();
//test2();
test3(0);


}
