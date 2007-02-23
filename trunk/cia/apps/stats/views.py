#
# XXX: Experimental
#

from django.shortcuts import render_to_response
from django.http import HttpResponse
from cia.esquilax import _fidtool
from cia.apps.api.util import json_result
import os, time, datetime

FID_TESTFILE = 'esquilax/test-data.fid'

def dayGrid(t):
    """Generator for day-aligned grids, starting no later than the given time"""
    date = datetime.datetime.fromtimestamp(t).date()

    while 1:
        yield int(time.mktime(date.timetuple())), _fidtool.GRID_SOLID
        date += datetime.timedelta(days=1)

@json_result
def fidtool_counts(request):
    fd = os.open(FID_TESTFILE, os.O_RDONLY)

    now = int(time.time())
    d = {}

    query_points = (
        ('now', now),
        ('hour_ago', now - 60 * 60),
        ('day_ago', now - 60 * 60 * 24),
        ('week_ago', now - 60 * 60 * 24 * 7),
        ('month_ago', now - 60 * 60 * 24 * 30),
        ('year_ago', now - 60 * 60 * 24 * 365),
        )

    for i, (sample_time, sample_index) in enumerate(
        _fidtool.query_samples(fd, [q[1] for q in query_points])):
        name, query_time = query_points[i]
        d[name] = query_time, sample_time, sample_index

    return d

def fidtool_graph(request):
    width = int(request.GET['w'])
    height = int(request.GET['h'])
    x_scale = int(request.GET['xo']), int(request.GET['xs'])
    y_scale = int(request.GET['ys'])
    y_gridsize = int(request.GET['yg'])

    colors = (
        (1.0, 1.0, 1.0, 1.0),
        (0.0, 0.0, 0.0, 0.2),
        (0.0, 0.0, 1.0, 0.6),
        )

    x_grid = dayGrid(x_scale[0])
    y_grid = ( (y_gridsize*i, _fidtool.GRID_SOLID)
               for i in xrange(1, y_scale // y_gridsize + 1) )

    scales = (
        (x_scale, x_grid),
        (y_scale, y_grid),
        )

    response = HttpResponse(mimetype='image/png')
    fd = os.open(FID_TESTFILE, os.O_RDONLY)
    _fidtool.graph_png(fd, response, (width, height), colors, scales)
    os.close(fd)
    return response

def fidtool_page(request):
    return render_to_response("fidtool_page.html")

