# -*- coding: gb18030 -*-
import re

f= open(r'C:\Users\summer\Desktop\handwriting_research\error_count\pdf_position.txt', 'r')
new_path = open(r'C:\Users\summer\Desktop\handwriting_research\error_count\word_length.txt','w')
line = f.readline()
while line:
    thisLine = line.split()
    new_path.write(thisLine[0]+' ' )
    del thisLine[0]
    length_of_thisLine = len(thisLine)
    for index,num in enumerate(thisLine):
        if index+1 < length_of_thisLine:
            pos_this = int(num)
            pos_next = int(thisLine[index+1])
            new_path.write(str(pos_next-pos_this)+' ')
    new_path.write('\n')
    line=f.readline()






