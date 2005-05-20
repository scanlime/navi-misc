all:	AMCEditor.exe DumpDOF.exe DOFHistogram.exe

AMCEditor.exe:	AMCEditor.cs AMC.cs Color.cs CurveEditor.cs amc-editor.glade
	mcs -out:AMCEditor.exe -debug -target:exe AMCEditor.cs AMC.cs Color.cs CurveEditor.cs `pkg-config --libs gtk-sharp-2.0 glade-sharp-2.0` -resource:amc-editor.glade

DumpDOF.exe:	DumpDOF.cs AMC.cs
	mcs -out:DumpDOF.exe -debug -target:exe DumpDOF.cs AMC.cs `pkg-config --libs gtk-sharp-2.0 glade-sharp-2.0`

DOFHistogram.exe:	DOFHistogram.cs AMC.cs dof-histogram.glade
	mcs -out:DOFHistogram.exe -debug -target:exe DOFHistogram.cs AMC.cs `pkg-config --libs gtk-sharp-2.0 glade-sharp-2.0` -resource:dof-histogram.glade -r:lib/NPlot.dll -r:lib/NPlot.Gtk.dll
