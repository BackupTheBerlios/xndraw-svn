#!/usr/bin/env python
# -*- encoding:utf-8 mode:python -*-

#usage:
#     python <svgfile> [<outfile>]
#convert (paths from) svg xml to simple values,
#one point _or_ onepoint and 2 handles on aline
#preceded by 'M'(ove), 'L'(ine), and 'C'urve
#and possibly followed by 'Z'(closepath)
#paths separated by blank line
#TODO: a line at top with the path's id or name - prefix with a special character like '@'
#so we can pull out a particular path from a file 
#and avoid xml parsing in c




#inkscape svg document as seen by python/minidom
#<xml.dom.minidom.Document instance at 0x7fd7544d04d0>
#>>> svgnodes = svgdata.childNodes
#>>> svg = svgnodes[1]
#<DOM Element: svg at 0x7fd7544d0878>
#>>> defs = svg.childNodes[1]
#<DOM Element: defs at 0x7fd7544d6638>
#>>> namedview= svg.childNodes[3]
#<DOM Element: sodipodi:namedview at 0x7fd7544d70e0>
#>>> meta=svg.childNodes[5] 
#<DOM Element: metadata at 0x7fd7544d9560> #svg:metadata
#>>> g_node=svg.childNodes[7]
#[<DOM Element: g at 0x7fd7544d9ef0>] #svg:g
#>>> g_node.getElementsByTagName('path')
#[<DOM Element: path at 0x7fd7544dd368>]#svg:path


##############
## problem:
#######
## svg y-axis counts from bottom
#cairo and hence xndraw counts y-axis from top
#does this create weird bezier effect?
import os, sys, traceback
from xml.dom import minidom
from xml.parsers.expat import ExpatError

if sys.getdefaultencoding() != 'utf-8':
        #need to resurrect this little nasty at least for now
    reload(sys)
    sys.setdefaultencoding('utf-8')

class PathElement(object):
    pass

class LineClose(PathElement):
    def __str__(self):
        return '<LineClose>'
    def __repr__(self):
        return self.__str__()
    def toxml(self):
        return '<lineclose/>'
class Pt(object):
    """class Pt
    wraps a point 
    self.flip_y : whether to add or subtract y values with __add__()
class wide behaviour -static -- and broken
    """
    flip_y=False
    def __init__(self, x, y):
        self.x=x
        self.y=y
    def set_flip_y(self, val=True):
        if val:
            self.flip_y=True
        else:
            self.flip_y=False
    def toxml(self):
        return '<point>\n<x>'+str(self.x)+ '</x>\t<y>'+str(y)+'</y>\n</point>'
    def __add__(self, other):
        x= self.x + other.x
        y=None
        if self.flip_y:
            y= self.y - other.y
        else:
            y= self.y + other.y
        return Pt(x, y)
    def __str__(self):
        return "( "+ str(self.x) +", "+str(self.y)+" )"
    def __repr__(self):
        return self.__str__()
class Bspline_Pt(PathElement):
    def _create_bspline(kls, previouspt, rel_pt, rel_h1=None, rel_h2=None):
        pt = rel_pt + previouspt
        h1=rel_h1
        if h1:
            h1 += previouspt
        h2=rel_h2
        if h2:
            h2 += previouspt
        return kls(pt, h1, h2)
    create_bspline =classmethod( _create_bspline)
    def __init__(self, pt,h1=None, h2=None):
        self.pt=pt
#reorder handles to see if they work better 2.3.7
        self.handle1 = h2
        self.handle2 = h1
    def toxml(self):
        s = '<bsplinepoint>\n<xy>\n'+ self.pt.toxml()+'\n</xy>\n'
        if self.handle1:
            s += '<handle id="0">\n<xy>\n'+self.handle1.toxml()+'\n</xy>\n'
        if self.handle2:
            s += '<handle id="1">\n<xy>\n'+self.handle2.toxml()+'\n</xy>\n'
        return s + '</bsplinepoint>'
    def __repr__(self):
        s = "Bspline_Pt: " +str(self.pt)
        if self.handle1:
            s+= " <=> handles: "+str(self.handle1)
        if self.handle2:
            s += ", "+str(self.handle2)
        return s +'\n'
    def __str__(self_):
        return repr(self)
def get_image_elements(svgdoc):
    svg=None
    for element in svgdoc.childNodes:
    	if element.nodeType == minidom.Node.ELEMENT_NODE:
            if element.tagName == 'svg':
                svg = element
                break
    for item in svg.childNodes:
        if item.nodeType == minidom.Node.ELEMENT_NODE:
            if item.tagName == 'g':
                return item
            


def get_paths(g):
    return g.getElementsByTagName('path')

def get_points_string(path):
    return path.getAttribute('d')

def get_pointlist(s_points):
    res=[]
    x,y= None, None
    start = end =0
    while end < len(s_points):
        while end < len(s_points) and s_points[end].isalpha():
            end +=1
        if end == len(s_points):
            break
        start = end
        while end < len(s_points) and s_points[end] != ',':
            end +=1
        if s_points[end] != ',':
            raise RuntimeError("expecting a comma")
        x = float(s_points[start:end])
        end +=1
        start = end
        while end < len(s_points) and not s_points[end].isspace():
            end +=1
        if start == end:
            raise RuntimeError("need y")
        y = float(s_points[start:end])
        res.append(Pt(x, y))
    return res

def parse_path(path):
    res = []
    ptstring =  get_points_string(path).lstrip().rstrip()
    pathid=path.getAttribute('id')
    x, y = None, None
    current_point=None
    pointstyle='line'
    startindex=endindex = 0
    s_len = len(    ptstring)
    if ptstring[0] != 'm' and  ptstring[0] != 'M':
        raise RuntimeError("no m at start, instead '"+ ptstring[0]+"'")
    while endindex < s_len:
        while endindex < s_len and not ptstring[endindex ].isspace():
            endindex +=1
        startindex=endindex
        while endindex < s_len and not ptstring[endindex ].isalpha():
            endindex +=1
        sys.stdout.write("list of points" + ptstring[startindex:endindex])
        points = get_pointlist( ptstring[startindex:endindex].lstrip().rstrip())
        if not len(points):
            pass
	    #raise RuntimeError("no points in list?")
        if not current_point:
            current_point = points[0]
            res.append(Bspline_Pt(current_point))
            points = points[1:]
        if pointstyle == 'line':
            for p in points:
                current_point = p +current_point
                res.append(Bspline_Pt(current_point))
        elif pointstyle == 'curve':
            if len(points) % 3:
                raise RuntimeError("curve points in groups of three points\n'"+str(points)
                                   + "'\n has " +str(len(points)) + " points")
            while len(points):
                grup = points[:3]
                points = points[3:]
#is this how it works, or is each point relative to the previous?
                h1= grup[0] + current_point
                h2=grup[1] + current_point
                current_point = grup[2] + current_point
                res.append(Bspline_Pt(current_point, h1, h2))
        if endindex >= s_len:
            break
        if  ptstring[endindex ] == 'c' or ptstring[endindex ] == 'C':
            pointstyle= 'curve'
        elif ptstring[endindex ] == 'l' or  ptstring[endindex ] == 'L' or ptstring[endindex ] == 'm' or ptstring[endindex ] == 'M':
            pointstyle='line'
        elif ptstring[endindex ] == 'z' or  ptstring[endindex ] == 'Z':
            res.append(LineClose())
        else:
            raise RuntimeError("not understood: '"+ptstring[endindex] +"'\nstring remaining:\n'"
                               + ptstring[endindex:]+"'")
    return pathid, res

def write_to_file(pathid, pointlist, f):
    if not f:
        f = sys.stderr
    f.write('@'+ str(pathid)+'\n')
    f.write('M\n')
    cur = 'L'
    f.write(str(pointlist[0].pt.x)+ ' ' + str(pointlist[0].pt.y) +'\n')
    for item in pointlist[1:]:
        if not hasattr(item, 'pt'):
            f.write('Z\n')
        elif not item.handle1:
            if cur == 'C':
                f.write('L\n')
                cur = 'L'
            f.write(str(item.pt.x)+ ' ' + str(item.pt.y) +'\n')
        else:
            if cur == 'M' or cur =='L':
                f.write('C\n')
                cur = 'C'
            f.write(str(item.pt.x)+ ' ' + str(item.pt.y)+ ' ' + str(item.handle1.x)+
                    ' ' +str(item.handle1.y)+ ' ' + str(item.handle2.x)+
                    ' ' +str(item.handle2.y) +'\n')
    f.write('\n')#blank line between paths



def points_from_svgfile(filename):
    if not os.path.exists(os.path.abspath(filename)):
        raise RuntimeError("no such F or D")
    doc = minidom.parse(filename)
    g_node = get_image_elements(doc)
    paths = get_paths(g_node)
    if not len(paths):
        raise RuntimeError("no paths in file?")
    res=[]
    for pat in paths:
        res.append(parse_path(pat))
    return res

if __name__ == '__main__':
    outfile = None
    if len(sys.argv)>1:
        svgfile = sys.argv[1]
        if not os.path.exists(svgfile):
            sys.stderr.write("no such file of directory : "+svgfile)
            traceback.print_stack()
            raise SystemExit(1);
    if len(sys.argv)>2:
        if sys.argv[2].isdigit():
            intfd=int( sys.argv[2])
            try:
            
                outfile =os.fdopen(intfd, "w")
            except OSError:
                e = sys.exc_info()[1]
                if e.errno==9:
                    sys.stderr.write("bad file descriptor");
                traceback.print_exc()
                raise SystemExit(4)
        if os.path.exists(os.path.dirname(sys.argv[2])):
            outfilename = sys.argv[2]
            try:
                outfile = open(outfilename, 'w')
            except:
                sys.stderr.write("opening outfile failed: " + str(sys.exc_info()))
                traceback.print_exc()
                raise SystemExit(2);
    try:
        pointlists=()
        try:
            pointlists = points_from_svgfile(svgfile)
        except ExpatError:
            sys.stderr.write("expat error:\n")
            e_expat = sys.exc_info()[1]
            if e_expat:
                sys.stderr.write("code: " + str(e_expat.code)+ "\nlineno: "+ str(e_expat.lineno) + "\n offset into line of xml: " + str(e_expat.offset))
            raise SystemExit(99)
                
        for pathid, l in pointlists:
            write_to_file(pathid, l, outfile)
            sys.stdout.write("\n" +pathid+'\n'+ str(l))
    except Exception:
        #make sure to exit non-zero on error
        sys.stderr.write("an error occurred trying to read in svg file: "+str(svgfile))
        traceback.print_exc()
        raise SystemExit(3)
