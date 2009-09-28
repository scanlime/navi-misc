#include <iostream>
#include <wx/wx.h>
#include "log_reader.h"
#include "log_index.h"

using namespace std;


class MyApp : public wxApp {

  LogReader reader;
  LogIndex index;
  
  virtual bool OnInit() {
    reader.Open(wxT("datamgmt-sd-insert.trace.raw"));
    //reader.Open(wxT("mem-20090905-browser-install-1.raw.part00"));
    index.Open(&reader);
    printf("Application init'ed\n");
  }
};

IMPLEMENT_APP(MyApp);
