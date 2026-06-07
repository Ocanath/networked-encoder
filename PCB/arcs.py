import numpy as np

def triad_to_arc_text_primitives(start,midpoint,end):
    printstr = "(gr_arc (start "
    printstr = printstr + str(start[0]) + " "
    printstr = printstr + str(start[1]) + ") (mid "
    printstr = printstr + str(midpoint[0]) + " "
    printstr = printstr + str(midpoint[1]) + ") (end "
    printstr = printstr + str(end[0]) + " "    
    printstr = printstr + str(end[1]) + ")\n"
    printstr = printstr + "  (stroke (width 0.0254) (type default)) (layer \"Edge.Cuts\") (tstamp fdfa577c-7491-4ab3-8c84-ccdebd1992fe))"

    return printstr


"""
    Calculates the start, mid, and end points for a filleted 90degree angle TOP RIGHT corner
    corner which converges at the point "meetingpoint" and has radius "radius". Units are arbitrary.
"""
def corner_top_right(meetingpoint, radius):
    start = meetingpoint - np.array([radius, 0])
    end = meetingpoint + np.array([0, radius])
    center = meetingpoint + np.array([-radius, radius])
    midpoint = center + np.array([radius*np.cos(np.pi/4), -radius*np.sin(np.pi/4) ])    #subtract y because in kicad, +y is down. fucking hell
    return start,midpoint,end


"""
    Calculates the start, mid, and end points for a filleted 90degree angle BOTTOM RIGHT corner
    corner which converges at the point "meetingpoint" and has radius "radius". Units are arbitrary.
"""
def corner_bottom_right(meetingpoint, radius):
    start = meetingpoint + np.array([0, -radius])
    end = meetingpoint + np.array([-radius, 0])
    center = meetingpoint + np.array([-radius, -radius])
    midpoint = center + np.array([radius*np.cos(np.pi/4), radius*np.sin(np.pi/4) ])   #add both from center
    return start,midpoint,end


meetingpoint = [126.686, 103.8199]
radius = 0.3
s,m,e = corner_bottom_right(meetingpoint, radius)
pstr = triad_to_arc_text_primitives(s,m,e)
print(pstr)

