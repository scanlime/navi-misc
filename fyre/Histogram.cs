using Gdk;

public struct HistogramPlot
{
	public int[] histogram;
	public int hist_width;
	public int density;
	public long plot_count;
}

public class Histogram
{
	int width, height;
	double exposure, gamma;
	Gdk.Color fgcolor, bgcolor;
	bool render_dirty_flag;

	long total_points_plotted;
	long peak_density;
	int[] histogram;

	Gdk.Pixbuf image;

	struct color_table
	{
		int allocated_size;
		int filled_size;
		int[] table;
	}

	public void UpdateImage ()
	{
	}

	public void SaveImageFile (string filename)
	{
	}

	private void CheckDirtyFlags ()
	{
	}

	private void RequireHistogram ()
	{
	}

	public void PreparePlots (HistogramPlot plot)
	{
		CheckDirtyFlags ();
		RequireHistogram ();
		plot.histogram = histogram;
		plot.hist_width = width;
		plot.density = 0;
		plot.plot_count = 0;
	}

	public void FinishPlots (HistogramPlot plot)
	{
		total_points_plotted += plot.plot_count;
		if (plot.density > peak_density)
			peak_density = plot.density;
	}

	public void Plot (HistogramPlot plot, int x, int y)
	{
		int bucket;
		plot.plot_count++;
		bucket = ++plot.histogram[x + plot.hist_width * y];
		if (bucket > plot.density)
			plot.density = bucket;
	}

	static void Main ()
	{
	}
};
