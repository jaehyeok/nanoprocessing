#include <glob.h>
#include "TString.h" 

using namespace std;

// list of files in a directory
std::vector<TString> globVector(const TString& pattern){
  glob_t glob_result;
  glob(pattern.Data(),GLOB_TILDE,NULL,&glob_result);
  std::vector<TString> files;
  for(unsigned int i=0;i<glob_result.gl_pathc;++i){
    files.push_back(TString(glob_result.gl_pathv[i]));
  }
  globfree(&glob_result);
  return files;
}

std::vector<TString> getFileListFromFile(const TString flist)
{
  vector<TString> files;  
  fstream fin(flist.Data());
  string line;
  if(fin.is_open()) {
    while(fin.good()){
      getline(fin, line);
      if(line.find("#")!=string::npos ) continue;
      if(line!="") files.push_back(line); 
    }
  }
  fin.close();
  return files;
}

// deltaR
float deltaR(float deltaEta, float deltaPhi)
{ 
  return TMath::Sqrt(deltaEta*deltaEta+deltaPhi*deltaPhi);
}

float deltaR(float eta1, float eta2, float phi1, float phi2)
{ 
  return  deltaR(eta1-eta2, phi1-phi2);
}
