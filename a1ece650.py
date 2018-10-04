import sys
import string
import copy
import numpy as np 
import re
from intersect import *
# import dictionary for graph 
from collections import defaultdict 

class ParseException(Exception):
    def __init__(self, value):
        self._value = value

    def __str__(self):
        return str(self._value)

def parse_line(line):
    if not line.startswith('a ') and not line.startswith('c ') and not line.startswith('r ') and not line.startswith('g ') :
        raise ParseException("does not start with commands 'a' or 'c' or 'r' or 'g'")
    else:
        pass

def str_to_tup(str_lst,tup_lst):
	# str list element format is '(-d,-d)' where '-' is optional
	#print str_lst
	for x in str_lst:
		if x[1].isdigit() and x[3].isdigit():
			tup_lst.append((int(x[1]),int(x[3])))
		elif x[1].isdigit() and x[4].isdigit():
			tup_lst.append((int(x[1]),-int(x[4])))
		elif x[2].isdigit() and x[4].isdigit():
			tup_lst.append((-int(x[2]),int(x[4])))
		elif x[2].isdigit() and x[5].isdigit():
			tup_lst.append((-int(x[2]),-int(x[5])))
		else:
			print "something went wrong in str_to_tup \n"
			sys.exit(1)

def main():
    ### YOUR MAIN CODE GOES HERE

    ### sample code to read from stdin.
    ### make sure to remove all spurious print statements as required
    ### by the assignment
    r = '^\s*([a-z])\s*"(\s*[a-zA-Z]+\s*[a-zA-Z]+\s*)"\s*(\(.*\))\s*'
    p = re.compile(r)
    r2 = '(\(-*[\d]+,-*[\d]+\))'
    p2 = re.compile(r2)
    #Regex for command and street name and the rest of the line
    r3 = '^\s*([a-z])\s+"([a-zA-Z ]+)"(.*)'
    p3 = re.compile(r3)
    allowed = set('(' + string.digits + '-' + ',' + ')')
    coord_list_tup=[]
    streets = defaultdict(list)
    st_with_inter = defaultdict(list)
    intersec_dict = defaultdict(list)
    #intersec_dict_prev = defaultdict(list)
    intersec_edges = defaultdict(list)
    vtx_with_id = defaultdict(list)
    edge_list = []
    vtx_list = []
    vtx_id=1
    while True:
        try:
        	line = raw_input()
        except EOFError:
        	break;
        #print 'read a line:', line
        match = p3.search(line)
        if match:
        	if match.lastindex!=3:
        		line=line.strip()
        		if (len(line)==1):
        			if line[0]=='g':
        				sys.stdout.write("Error: Branch cant be taken \n")
        			else:
        				sys.stdout.write("Error: incorrect command format \n")
        				continue
        		else:
        			sys.stdout.write("Error: incorrect input format \n")
        			continue
        	else:
        		#insert match.group(3) is upper function
        		street_name = match.group(2).upper()
        		if match.group(1)=='r':
        			if street_name not in streets.keys():
        				sys.stdout.write("Error: street does not exist in the database \n")
        				continue
        			else:
        				del streets[street_name]    #Add proper delete function
        		else:
        			coord = match.group(3)
	        		coord=coord.replace(' ','')
	        		coord_flag=set(coord) <= allowed
	        		if coord_flag:
	        			if match.group(1)=='a':
	        				if street_name in streets.keys():
	        					sys.stdout.write("Error: street already exists in database \n")
	        					continue
	        				else:
	        					coord_list = p2.findall(coord)
	        					coord_temp = 0
	        					for i in coord_list:
	        						coord_temp=coord_temp+len(i)
	        					if coord_temp==0:
	        						sys.stdout.write("Error: wrong coordinate format \n")
	        						continue
	        					else:
	        						if coord_temp!=len(coord):
	        							sys.stdout.write("Error: wrong coordinate format \n")
	        							continue
	        						else:
	        							del coord_list_tup[:]
	        							str_to_tup(coord_list,coord_list_tup)
	        							streets[street_name].extend(coord_list_tup)
	        			elif match.group(1)=='c':
	        				if street_name not in streets.keys():
	        					sys.stdout.write("Error: street does not exist in the database \n")
	        					continue
	        				else:
	        					del streets[street_name][:]
	        					coord_list = p2.findall(coord)
	        					coord_temp = 0
	        					for i in coord_list:
	        						coord_temp=coord_temp+len(i)
	        					if coord_temp==0:
	        						sys.stdout.write("Error: wrong coordinate format \n")
	        						continue
	        					else:
	        						if coord_temp!=len(coord):
	        							sys.stdout.write("Error: wrong coordinate format \n")
	        							continue
	        						else:
	        							del coord_list_tup[:]
	        							str_to_tup(coord_list,coord_list_tup)
	        							streets[street_name].extend(coord_list_tup)
        			else:
        				print coord
        				sys.stdout.write("Error: incorrect input format \n")
        				continue
        else:
        	line=line.strip()
        	if (len(line)==1):
        		if line[0]=='g':
        			#print "Generating the graph \n"
        			#intersec_dict_prev = copy.deepcopy(intersec_dict)
        			st_with_inter = copy.deepcopy(streets)
        			street_intersec(streets,st_with_inter,intersec_dict)
        			#Time to create graph dictionaries
        			intersec_edges.clear()
        			del vtx_list[:]
        			del edge_list[:]
        			for key in intersec_dict.keys():
        				for x in intersec_dict[key]:
        					if st_with_inter[x].index(key)>0:
        						intersec_edges[key].append(st_with_inter[x][st_with_inter[x].index(key)-1])
        					if st_with_inter[x].index(key)<(len(st_with_inter[x])-1):
        						intersec_edges[key].append(st_with_inter[x][st_with_inter[x].index(key)+1])
        				intersec_edges[key]=set((intersec_edges[key]))
        			for key in intersec_edges.keys():
        				if key not in vtx_with_id.keys():
        					vtx_with_id[key].append(vtx_id)
        					vtx_list.append((vtx_id,key))
        					vtx_id=vtx_id+1
        				else:
        					vtx_list.append((vtx_with_id[key][0],key))
        				for x in intersec_edges[key]:
        					if x not in vtx_with_id.keys():
        						vtx_with_id[x].append(vtx_id)
        						vtx_list.append((vtx_id,x))
        						vtx_id=vtx_id+1
        					else:
        						vtx_list.append((vtx_with_id[x][0],x))
        			
        			for key in intersec_edges.keys():
        				for x in intersec_edges[key]:
        					if (vtx_with_id[key][0],vtx_with_id[x][0]) not in edge_list and (vtx_with_id[x][0],vtx_with_id[key][0]) not in edge_list:
        						edge_list.append((vtx_with_id[key][0],vtx_with_id[x][0]))
        			#printinng graph
        			print "V = {"
        			for x in vtx_list:
        				print str(x[0]) +': ' + '(' + str(x[1][0]) +',' +str(x[1][1]) + ')'
        			print "}"
        			print "E = {"
        			for x in edge_list:
        				#print x
        				if edge_list.index(x)<(len(edge_list)-1):
        					print '<'+str(x[0])+','+str(x[1])+'>'+','
        				else:
        					print '<'+str(x[0])+','+str(x[1])+'>'
        			print "}"
        			#print intersec_edges
        			#print vtx_with_id
        		else:
        			sys.stdout.write("Error: incorrect command format \n")
        			continue
        	else:
        		sys.stdout.write("Error: incorrect input format \n")
        		continue
    #print streets
    #print 'Finished reading input \n'
    # return exit code 0 on successful termination
    sys.exit(0)

if __name__ == '__main__':
    main()  