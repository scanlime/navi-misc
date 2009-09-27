#include <iostream>
#include "log_reader.h"

using namespace std;

int
main()
{
  MemTransfer mt;
  LogReader reader;
  wxString fileName(wxT("datamgmt-sd-insert.trace.raw"));

  if (!reader.Open(fileName)) {
    cerr << "Error opening input file: " << fileName.c_str() << endl;
    return 1;
  }

  while (reader.Read(mt)) {
    cout << "offs = " << mt.LogOffset() << " t = " << mt.type << " len = " << mt.byteCount << endl;
    reader.Next(mt);
  }

  return 0;
}
