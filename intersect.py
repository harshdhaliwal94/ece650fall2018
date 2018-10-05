from collections import defaultdict 
import string

class Point(object):
    def __init__ (self, x, y):
        self.x = float(x)
        self.y = float(y)
    def __str__ (self):
        return '(' + str(self.x) + ',' + str(self.y) + ')'

class Line(object):
    def __init__ (self, src, dst):
        self.src = src
        self.dst = dst

    def __str__(self):
        return str(self.src) + '-->' + str(self.dst)

def intersect (l1, l2):
    x1, y1 = l1.src.x, l1.src.y
    x2, y2 = l1.dst.x, l1.dst.y
    x3, y3 = l2.src.x, l2.src.y
    x4, y4 = l2.dst.x, l2.dst.y

    par_lines=abs(((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4)))
    l1_perp=abs(x1-x2)
    l2_perp=abs(x3-x4)
    if par_lines<=0.0001:
        #parallel lines
        if l1_perp<=0.0001:
            #both perpendicular
            if abs(x1-x3)<=0.0001:
                #same lines since no-overlap make it simple
                if abs(y1-y3)<=0.0001:
                    return Point(x1,y1)
                elif abs(y1-y4)<=0.0001:
                    return Point(x1,y1)
                elif abs(y2-y3)<=0.0001:
                    return Point(x2,y2)
                elif abs(y2-y4)<=0.0001:
                    return Point(x2,y2)
                else:
                    return Point(-500,-500)
            else:
                return Point(-500,-500)

        else:
            #parallel but not perpendicular no overlap
            if abs(y1-y3)<=0.0001 and abs(x1-x3)<=0.0001:
                return Point(x1,y1)
            elif abs(y1-y4)<=0.0001 and abs(x1-x4)<=0.0001:
                return Point(x1,y1)
            elif abs(y2-y3)<=0.0001 and abs(x2-x3)<=0.0001:
                return Point(x2,y2)
            elif abs(y2-y4)<=0.0001 and abs(x2-x4)<=0.0001:
                return Point(x2,y2)
            else:
                return Point(-500,-500)

    else:
        # both not parallel nor perpendicular
        xnum = ((x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4))
        xden = ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4))
        xcoor =  xnum / xden

        ynum = (x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4)
        yden = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4)
        ycoor = ynum / yden

        #check whether intersect is valid and check edge case 0.0001
        if min(l1.src.x,l1.dst.x)<=xcoor and (min(l1.src.x,l1.dst.x)+abs(l1.dst.x-l1.src.x))>=xcoor and min(l2.src.x,l2.dst.x)<=xcoor and (min(l2.src.x,l2.dst.x)+abs(l2.dst.x-l2.src.x))>=xcoor:
            if min(l1.src.y,l1.dst.y)<=ycoor and (min(l1.src.y,l1.dst.y)+abs(l1.dst.y-l1.src.y))>=ycoor and min(l2.src.y,l2.dst.y)<=ycoor and (min(l2.src.y,l2.dst.y)+abs(l2.dst.y-l2.src.y))>=ycoor:
                if abs(l1.src.x-xcoor)<=0.0001 and abs(l1.src.y-ycoor)<=0.0001:
                    return Point(l1.src.x, l1.src.y)
                elif abs(l1.dst.x-xcoor)<=0.0001 and abs(l1.dst.y-ycoor)<=0.0001:
                    return Point(l1.dst.x, l1.dst.y)
                elif abs(l2.src.x-xcoor)<=0.0001 and abs(l2.src.y-ycoor)<=0.0001:
                    return Point(l2.src.x, l2.src.y)
                elif abs(l2.dst.x-xcoor)<=0.0001 and abs(l2.dst.y-ycoor)<=0.0001:
                    return Point(l2.dst.x, l2.dst.y)
                else:
                    return Point ("{0:.2f}".format(xcoor), "{0:.2f}".format(ycoor))
            else:
                return Point(-500,-500)
        else:
            return Point(-500,-500)

def add_inter(st_with_inter,p_inter,list_seg_st,list_seg_end,street_key):
    #print st_with_inter, p_inter, list_seg_st, list_seg_end, street_key
    #add the intersection point coordinate to the street list at appropriate place
    seg_st_index=st_with_inter[street_key].index(list_seg_st)
    seg_end_index=st_with_inter[street_key].index(list_seg_end)
    if int(p_inter.x)==p_inter.x and int(p_inter.y)==p_inter.y:
        p_inter_x = int(p_inter.x)
        p_inter_y = int(p_inter.y)
    else:
        p_inter_x = p_inter.x
        p_inter_y = p_inter.y

    if float(st_with_inter[street_key][seg_st_index][0])<=float(st_with_inter[street_key][seg_end_index][0]):
        if float(st_with_inter[street_key][seg_st_index][1])<=float(st_with_inter[street_key][seg_end_index][1]):
            for i in range((seg_st_index+1),(seg_end_index+1)):
                if p_inter.x<=float(st_with_inter[street_key][i][0]) and p_inter.y<=float(st_with_inter[street_key][i][1]):
                    if p_inter.x==float(st_with_inter[street_key][i][0]) and p_inter.y==float(st_with_inter[street_key][i][1]):
                        break
                    else:
                        st_with_inter[street_key].insert(i,(p_inter_x,p_inter_y))
                        break
        else:
            for i in range((seg_st_index+1),(seg_end_index+1)):
                if p_inter.x<=float(st_with_inter[street_key][i][0]) and p_inter.y>=float(st_with_inter[street_key][i][1]):
                    if p_inter.x==float(st_with_inter[street_key][i][0]) and p_inter.y==float(st_with_inter[street_key][i][1]):
                        break
                    else:
                        st_with_inter[street_key].insert(i,(p_inter_x,p_inter_y))
                        break
    else:
        if float(st_with_inter[street_key][seg_st_index][1])>=float(st_with_inter[street_key][seg_end_index][1]):
            for i in range((seg_st_index+1),(seg_end_index+1)):
                if p_inter.x>=float(st_with_inter[street_key][i][0]) and p_inter.y>=float(st_with_inter[street_key][i][0]):
                    if p_inter.x==float(st_with_inter[street_key][i][0]) and p_inter.y==float(st_with_inter[street_key][i][1]):
                        break
                    else:
                        st_with_inter[street_key].insert(i,(p_inter_x,p_inter_y))
                        break
        else:
            for i in range((seg_st_index+1),(seg_end_index+1)):
                if p_inter.x>=float(st_with_inter[street_key][i][0]) and p_inter.y<=float(st_with_inter[street_key][i][1]):
                    if p_inter.x==float(st_with_inter[street_key][i][0]) and p_inter.y==float(st_with_inter[street_key][i][1]):
                        break
                    else:
                        st_with_inter[street_key].insert(i,(p_inter_x,p_inter_y))
                        break

def street_intersec(streets,st_with_inter,intersec_dict):
    intersec_dict.clear()
    street_keys=streets.keys()
    for i in range(len(street_keys)-1):
        for j in range(len(streets[street_keys[i]])-1):
            for k in range(i+1,len(street_keys)):
                for l in range(len(streets[street_keys[k]])-1):    
                    p1=Point((streets[street_keys[i]][j][0]), (streets[street_keys[i]][j][1]))
                    p2=Point((streets[street_keys[i]][j+1][0]), (streets[street_keys[i]][j+1][1]))
                    p3=Point((streets[street_keys[k]][l][0]), (streets[street_keys[k]][l][1]))
                    p4=Point((streets[street_keys[k]][l+1][0]), (streets[street_keys[k]][l+1][1]))
                    l1=Line(p1,p2)
                    l2=Line(p3,p4)
                    p_inter=intersect(l1,l2)
                    #print 'intersect is ',intersect(l1,l2)
                    if p_inter.x==-500.0 and p_inter.y==-500.0:
                        continue
                    else:
                        if int(p_inter.x)==p_inter.x and int(p_inter.y)==p_inter.y:
                            intersec_key=(int(p_inter.x),int(p_inter.y))
                        else:
                            intersec_key=(p_inter.x,p_inter.y)
                        if intersec_key not in intersec_dict.keys():
                            intersec_dict[intersec_key].extend([street_keys[i],street_keys[k]])
                            add_inter(st_with_inter,p_inter,streets[street_keys[i]][j],streets[street_keys[i]][j+1],street_keys[i])
                            add_inter(st_with_inter,p_inter,streets[street_keys[k]][l],streets[street_keys[k]][l+1],street_keys[k])
                        else:
                            intersec_dict[intersec_key].append(street_keys[k])
                            add_inter(st_with_inter,p_inter,streets[street_keys[k]][l],streets[street_keys[k]][l+1],street_keys[k])

if __name__ == '__main__':
    p1 = Point (1, 4)
    p2 = Point (5, 9)
    p3 = Point (5, 9)
    p4 = Point (8, 7)

    l1 = Line (p1, p2)
    l2 = Line (p3, p4)
    print 'Intersect of', l1, 'with', l2, 'is', intersect(l1, l2)