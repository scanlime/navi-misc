all:	AMCEditor.exe DumpDOF.exe DOFHistogram.exe

AMC.dll:	AMC.cs Graph.cs
	mcs -out:AMC.dll -debug -target:library AMC.cs Graph.cs

AMCEditor.exe:	AMCEditor.cs Color.cs CurveEditor.cs amc-editor.glade AMC.dll
	mcs -out:AMCEditor.exe -debug -target:exe AMCEditor.cs Color.cs CurveEditor.cs -r:AMC.dll `pkg-config --libs gtk-sharp-2.0 glade-sharp-2.0` -resource:amc-editor.glade

DumpDOF.exe:	DumpDOF.cs AMC.dll
	mcs -out:DumpDOF.exe -debug -target:exe DumpDOF.cs -r:AMC.dll `pkg-config --libs gtk-sharp-2.0 glade-sharp-2.0`

DOFHistogram.exe:	DOFHistogram.cs AMC.cs dof-histogram.glade AMC.dll
	mcs -out:DOFHistogram.exe -debug -target:exe DOFHistogram.cs -r:AMC.dll `pkg-config --libs gtk-sharp-2.0 glade-sharp-2.0` -resource:dof-histogram.glade -r:lib/NPlot.dll -r:lib/NPlot.Gtk.dll
