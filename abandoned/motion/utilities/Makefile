all:	AMCEditor.exe DumpDOF.exe DOFHistogram.exe

AMCEditor.exe:	AMC.cs AMCEditor.cs Color.cs CurveEditor.cs amc-editor.glade
	mcs -out:AMCEditor.exe -debug -target:exe AMC.cs AMCEditor.cs Color.cs CurveEditor.cs `pkg-config --libs gtk-sharp-2.0 glade-sharp-2.0` -resource:amc-editor.glade

DumpDOF.exe:	AMC.cs DumpDOF.cs
	mcs -out:DumpDOF.exe -debug -target:exe AMC.cs DumpDOF.cs `pkg-config --libs gtk-sharp-2.0 glade-sharp-2.0`

DOFHistogram.exe:	AMC.cs DOFHistogram.cs AMC.cs dof-histogram.glade
	mcs -out:DOFHistogram.exe -debug -target:exe AMC.cs DOFHistogram.cs `pkg-config --libs gtk-sharp-2.0 glade-sharp-2.0` -resource:dof-histogram.glade

clean:
	rm -f *.exe *.exe.mdb
