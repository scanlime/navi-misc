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
    index.Open(&reader);
  }
};

IMPLEMENT_APP(MyApp);
