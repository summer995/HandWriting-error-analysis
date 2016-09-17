
f= open(r'C:\Users\summer\Desktop\handwriting_research\error_count\alig2pdf.txt', 'r')
new_path = open(r'C:\Users\summer\Desktop\handwriting_research\error_count\pdf_position.txt','w')
line = f.readline()
while line:
    thisLine = line.split()
    new_path.write(thisLine[0]+' ' )
    del thisLine[0]
    length_of_thisLine = len(thisLine)
    count = 37
    for index,num in enumerate(thisLine):
        if index+1 < length_of_thisLine:
            count +=3
            pdf_this = int(num)
            pdf_next = int(thisLine[index+1])
            if (pdf_this>14 and pdf_next<15) or (pdf_next>14 and pdf_this<15):
                new_path.write(str(count)+' ')
    new_path.write('\n')
    line=f.readline()





